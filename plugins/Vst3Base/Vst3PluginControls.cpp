/*
 * Vst3PluginControls.cpp
 *
 * Copyright (c) 2021 Dominic Clark <mrdomclark$$$gmail.com, $$$=@>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Vst3PluginControls.h"

#include <algorithm>

#include <QDebug>
#include <QString>

#include "pluginterfaces/gui/iplugview.h"

#include "impl/BStream.h"
#include "impl/HostApplication.h"
#include "Vst3Manager.h"
#include "Vst3Plugin.h"
#include "Vst3PluginControlView.h"
#include "Vst3PluginView.h"

namespace lmms
{

Vst3PluginControls::Vst3PluginControls(Vst3Plugin* plugin) :
	m_plugin{plugin},
	m_controller{interface_cast<::Steinberg::Vst::IEditController>(plugin->m_component)}
{
	if (!m_controller)
	{
		const auto controllerUuid = [plugin]
		{
			if (::Steinberg::TUID tuid; plugin->m_component->getControllerClassId(tuid) == ::Steinberg::kResultOk)
			{
				return ::Steinberg::FUID::fromTUID(tuid);
			}
			return ::Steinberg::FUID{};
		}();
		if (!controllerUuid.isValid()) { return; }
		m_controller = plugin->m_module->createInstance<::Steinberg::Vst::IEditController>(controllerUuid);
		if (!m_controller) { return; }
		if (m_controller->initialize(HostApplication::instance()->asFUnknown()) != ::Steinberg::kResultOk) { return; }
	}
	if (const auto compConn = interface_cast<::Steinberg::Vst::IConnectionPoint>(plugin->m_component),
				   contConn = interface_cast<::Steinberg::Vst::IConnectionPoint>(m_controller);
		compConn && contConn)
	{
		compConn->connect(contConn);
		contConn->connect(compConn);
	}
	if (Uncounted<BStream> stream; plugin->m_component->getState(&stream) == ::Steinberg::kResultOk)
	{
		stream.seek(0, ::Steinberg::IBStream::kIBSeekSet);
		m_controller->setComponentState(&stream);
	}
	createParameters();
	m_controller->setComponentHandler(this);
}

Vst3PluginControls::~Vst3PluginControls()
{
	m_controller->terminate();
}


::Steinberg::tresult PLUGIN_API Vst3PluginControls::beginEdit(::Steinberg::Vst::ParamID id)
{
	const auto it = std::find_if(m_parameters.begin(), m_parameters.end(),
		[id](const Parameter& p) { return p.id() == id; });
	if (it == m_parameters.end()) { return ::Steinberg::kInvalidArgument; }
	const auto model = it->model();
	model->addJournalCheckPoint();
	model->saveJournallingState(false);
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API Vst3PluginControls::performEdit(::Steinberg::Vst::ParamID id,
	::Steinberg::Vst::ParamValue valueNormalized)
{
	const auto it = std::find_if(m_parameters.begin(), m_parameters.end(),
		[id](const Parameter& p) { return p.id() == id; });
	if (it == m_parameters.end()) { return ::Steinberg::kInvalidArgument; }
	const auto model = it->model();
	model->setValue(m_controller->normalizedParamToPlain(id, valueNormalized));
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API Vst3PluginControls::endEdit(::Steinberg::Vst::ParamID id)
{
	const auto it = std::find_if(m_parameters.begin(), m_parameters.end(),
		[id](const Parameter& p) { return p.id() == id; });
	if (it == m_parameters.end()) { return ::Steinberg::kInvalidArgument; }
	const auto model = it->model();
	model->restoreJournallingState();
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API Vst3PluginControls::restartComponent(::Steinberg::int32 flags)
{
	return ::Steinberg::kNotImplemented;
}

void Vst3PluginControls::saveSettings(QDomDocument& doc, QDomElement& element)
{
	if (auto state = Uncounted<BStream>{}; m_plugin->m_component->getState(&state) == ::Steinberg::kResultOk)
	{
		element.setAttribute("component_state", QString(state.toBase64()));
	}
	if (auto state = Uncounted<BStream>{}; m_controller->getState(&state) == ::Steinberg::kResultOk)
	{
		element.setAttribute("controller_state", QString(state.toBase64()));
	}
}

void Vst3PluginControls::loadSettings(const QDomElement& element)
{
	if (element.hasAttribute("component_state"))
	{
		auto stream = Uncounted<BStream>{};
		stream.fromBase64(element.attribute("component_state").toUtf8());
		m_plugin->m_component->setState(&stream);
		stream.seek(0, ::Steinberg::IBStream::kIBSeekSet);
		m_controller->setComponentState(&stream);
	}
	if (element.hasAttribute("controller_state"))
	{
		auto stream = Uncounted<BStream>{};
		stream.fromBase64(element.attribute("controller_state").toUtf8());
		m_controller->setState(&stream);
	}
}

QString Vst3PluginControls::nodeName() const
{
	return "vst3plugin";
}

std::vector<::Steinberg::Vst::UnitInfo> Vst3PluginControls::units()
{
	const auto unitInfo = interface_cast<::Steinberg::Vst::IUnitInfo>(m_controller);
	if (!unitInfo) { return {}; }
	auto result = std::vector<::Steinberg::Vst::UnitInfo>{};
	const auto unitCount = unitInfo->getUnitCount();
	result.reserve(unitCount);
	for(auto i = 0; i < unitCount; ++i)
	{
		if(auto info = ::Steinberg::Vst::UnitInfo{}; unitInfo->getUnitInfo(i, info) == ::Steinberg::kResultOk)
		{
			result.push_back(info);
		}
	}
	return result;
}

gui::Vst3PluginView* Vst3PluginControls::createPluginView(QWidget* parent)
{
	auto view = ::Steinberg::owned(m_controller->createView(::Steinberg::Vst::ViewType::kEditor));
	return view ? new Uncounted<gui::Vst3PluginView>(std::move(view), parent) : nullptr;
}

gui::Vst3PluginControlView* Vst3PluginControls::createControlView(QWidget* parent)
{
	return new gui::Vst3PluginControlView{this, parent};
}

void Vst3PluginControls::createParameters()
{
	const auto paramCount = m_controller->getParameterCount();
	m_parameters.reserve(paramCount);
	for (int i = 0; i < paramCount; ++i)
	{
		if (auto info = ::Steinberg::Vst::ParameterInfo{};
			m_controller->getParameterInfo(i, info) == ::Steinberg::kResultOk)
		{
			if (info.flags & ::Steinberg::Vst::ParameterInfo::kIsReadOnly) { continue; }
			const auto id = info.id;
			const auto unit = info.unitId;
			const auto title = toQString(info.title);
			if (info.stepCount == 0)
			{
				const auto min = m_controller->normalizedParamToPlain(id, 0);
				const auto max = m_controller->normalizedParamToPlain(id, 1);
				const auto def = m_controller->normalizedParamToPlain(id, info.defaultNormalizedValue);
				const auto step = (max - min) / 1000.f;
				m_parameters.emplace_back(id, unit, title,
					std::make_unique<FloatModel>(def, min, max, step, nullptr, title));
			}
			else if (info.flags & ::Steinberg::Vst::ParameterInfo::kIsList)
			{
				auto model = std::make_unique<ComboBoxModel>(nullptr, title);
				for(int i = 0; i <= info.stepCount; ++i)
				{
					const auto value = static_cast<double>(i) / info.stepCount;
					if (auto str = ::Steinberg::Vst::String128{};
						m_controller->getParamStringByValue(id, value, str) == ::Steinberg::kResultOk)
					{
						model->addItem(toQString(str));
					}
				}
				m_parameters.emplace_back(id, unit, title, std::move(model));
			}
			else
			{
				const auto steps = static_cast<int>(info.stepCount);
				const auto def = qBound(0, static_cast<int>(info.defaultNormalizedValue * (steps + 1)), steps);
				m_parameters.emplace_back(id, unit, title, std::make_unique<IntModel>(def, 0, steps, nullptr, title));
			}
		}
	}
}

} // namespace lmms
