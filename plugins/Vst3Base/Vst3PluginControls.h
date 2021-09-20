/*
 * Vst3PluginControls.h
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

#ifndef VST3_PLUGIN_CONTROLS_H
#define VST3_PLUGIN_CONTROLS_H

#include <memory>
#include <utility>
#include <vector>

#include <QDomDocument>
#include <QDomElement>
#include <QString>

#include "pluginterfaces/base/smartpointer.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstunits.h"

#include "AutomatableModel.h"
#include "ComboBoxModel.h"
#include "Vst3Core.h"

#include "vst3base_export.h"

class QWidget;

namespace lmms
{

class Vst3Plugin;

namespace gui
{

class Vst3PluginControlView;
class Vst3PluginView;

} // namespace gui

class Parameter
{
public:
	Parameter(::Steinberg::Vst::ParamID id, ::Steinberg::Vst::UnitID unit,
			const QString& name, std::unique_ptr<AutomatableModel> model) :
		m_id{id},
		m_unit{unit},
		m_name{name},
		m_model{std::move(model)} { }

	::Steinberg::Vst::ParamID id() const { return m_id; }
	::Steinberg::Vst::UnitID unit() const { return m_unit; }
	const QString& name() const { return m_name; }
	AutomatableModel* model() { return m_model.get(); }
	const AutomatableModel* model() const { return m_model.get(); }

private:
	::Steinberg::Vst::ParamID m_id;
	::Steinberg::Vst::UnitID m_unit;
	QString m_name;
	std::unique_ptr<AutomatableModel> m_model;
};

class VST3BASE_EXPORT Vst3PluginControls : public Uncounted<Implements<::Steinberg::Vst::IComponentHandler>>
{
public:
	Vst3PluginControls(Vst3Plugin* plugin);
	Vst3PluginControls(const Vst3PluginControls&) = delete;
	~Vst3PluginControls();

	Vst3PluginControls& operator=(const Vst3PluginControls&) = delete;

	// IComponentHandler methods
	::Steinberg::tresult PLUGIN_API beginEdit(::Steinberg::Vst::ParamID id) override;
	::Steinberg::tresult PLUGIN_API performEdit(::Steinberg::Vst::ParamID id,
		::Steinberg::Vst::ParamValue valueNormalized) override;
	::Steinberg::tresult PLUGIN_API endEdit(::Steinberg::Vst::ParamID id) override;
	::Steinberg::tresult PLUGIN_API restartComponent(::Steinberg::int32 flags) override;

	void saveSettings(QDomDocument& doc, QDomElement& element);
	void loadSettings(const QDomElement& element);
	QString nodeName() const;

	std::vector<Parameter>& parameters() { return m_parameters; }
	std::vector<::Steinberg::Vst::UnitInfo> units();

	gui::Vst3PluginView* createPluginView(QWidget* parent);
	gui::Vst3PluginControlView* createControlView(QWidget* parent);

private:
	void createParameters();

	Vst3Plugin* m_plugin;
	::Steinberg::IPtr<::Steinberg::Vst::IEditController> m_controller;
	std::vector<Parameter> m_parameters;
};

} // namespace lmms

#endif // VST3_PLUGIN_CONTROLS_H
