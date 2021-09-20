/*
 * Vst3EffectControlDialog.cpp
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

#include "Vst3EffectControlDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>

#include "Vst3Effect.h"
#include "Vst3EffectControls.h"
#include "Vst3PluginControlView.h"
#include "Vst3PluginView.h"

namespace lmms::gui
{

Vst3EffectControlDialog::Vst3EffectControlDialog(Vst3EffectControls* controls, Vst3Effect* effect) :
	EffectControlDialog(controls)
{
	setWindowTitle(effect->key().name);
	auto layout = new QVBoxLayout{this};
	auto tabs = new QTabWidget{};
	tabs->addTab(controls->pluginControls().createPluginView(nullptr), tr("Plugin"));
	tabs->addTab(controls->pluginControls().createControlView(nullptr), tr("Controls"));
	layout->addWidget(tabs);
}

void Vst3EffectControlDialog::showEvent(QShowEvent* se)
{
	EffectControlDialog::showEvent(se);
	// Workaround for a (unexplained) bug where on project-load the effect
	// control window has size 0 and would only restore to the proper size upon
	// moving the window or interacting with it.
	if (parentWidget())
	{
		parentWidget()->adjustSize();
	}
}

} // namespace lmms::gui
