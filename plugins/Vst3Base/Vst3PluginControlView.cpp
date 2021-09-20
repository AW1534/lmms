/*
 * Vst3PluginControlView.cpp
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

#include "Vst3PluginControlView.h"

#include <map>
#include <stdexcept>

#include <QVBoxLayout>

#include "ControlLayout.h"
#include "Vst3Core.h"
#include "Vst3PluginControls.h"

namespace lmms::gui
{

Vst3PluginControlView::Vst3PluginControlView(Vst3PluginControls* controls, QWidget* parent) :
	QWidget{parent}
{
	const auto layout = new QVBoxLayout{this};
	const auto filter = new ControlFilterWidget{};
	layout->addWidget(filter);
	m_unitTree = new QTreeWidget{};
	m_unitTree->setHeaderHidden(true);
	m_unitTree->setRootIsDecorated(false);
	m_unitTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_unitTree->setSelectionMode(QAbstractItemView::NoSelection);
	layout->addWidget(m_unitTree);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	// This will break if the plugin's units don't form a valid tree structure,
	// but that's complex to verify, so we just trust it...
	auto unitItems = std::map<::Steinberg::Vst::UnitID, QTreeWidgetItem*>{};
	// Unit 0 is the root unit
	unitItems.try_emplace(0, m_unitTree->invisibleRootItem());
	const auto units = controls->units();
	// Create `QTreeWidgetItem`s for each unit
	for (const auto &unit : units)
	{
		if (unit.id != 0) { unitItems.try_emplace(unit.id, new QTreeWidgetItem{{toQString(unit.name)}}); }
	}
	// Link the tree items to their parents to form the unit tree
	for (const auto &unit : units)
	{
		if (unit.id != 0) { unitItems.at(unit.parentUnitId)->addChild(unitItems.at(unit.id)); }
	}
	auto unitContainers = std::map<::Steinberg::Vst::UnitID, ControlLayout*>{};
	for (auto& parameter : controls->parameters())
	{
		const auto container = [&, this] {
			if (const auto it = unitContainers.find(parameter.unit()); it != unitContainers.end())
			{
				return it->second;
			}
			const auto widget = new QWidget{};
			const auto layout = new ControlLayout{};
			connect(filter, &ControlFilterWidget::filterChanged, layout, &ControlLayout::setFilterString);
			widget->setLayout(layout);
			widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
			const auto unitItem = unitItems.at(parameter.unit());
			const auto controlsItem = new QTreeWidgetItem{unitItem};
			m_unitTree->setItemWidget(controlsItem, 0, widget);
			unitContainers.try_emplace(parameter.unit(), layout);
			return layout;
		}();
		const auto control = m_controls.emplace_back([&]() -> std::unique_ptr<Control> {
			const auto model = parameter.model();
			if (dynamic_cast<FloatModel*>(model)) { return std::make_unique<KnobControl>(); }
			else if (dynamic_cast<ComboBoxModel*>(model)) { return std::make_unique<ComboControl>(); }
			else if (dynamic_cast<IntModel*>(model)) { return std::make_unique<LcdControl>(3); }
			Q_ASSERT_X(false, __func__, "unhandled model type");
			throw std::logic_error{"should not get here"};
		}()).get();
		control->setModel(parameter.model());
		control->setText(parameter.name());
		control->topWidget()->setObjectName(parameter.name());
		container->addWidget(control->topWidget());
	}
}

} // namespace lmms::gui
