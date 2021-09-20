/*
 * Vst3PluginControlView.h
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

#ifndef VST3_PLUGIN_CONTROL_VIEW_H
#define VST3_PLUGIN_CONTROL_VIEW_H

#include <memory>
#include <vector>

#include <QTreeWidget>
#include <QWidget>

#include "Controls.h"

namespace lmms
{

class Vst3PluginControls;

namespace gui
{

class Vst3PluginControlView : public QWidget
{
public:
	explicit Vst3PluginControlView(Vst3PluginControls* controls, QWidget* parent = nullptr);

private:
	QTreeWidget* m_unitTree;
	std::vector<std::unique_ptr<Control>> m_controls;
};

} // namespace gui

} // namespace lmms

#endif // VST3_PLUGIN_CONTROL_VIEW_H
