/*
 * Vst3EffectControls.h
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

#ifndef VST3_EFFECT_CONTROLS_H
#define VST3_EFFECT_CONTROLS_H

#include "EffectControls.h"

#include "Vst3PluginControls.h"

namespace lmms
{

class Vst3Effect;

namespace gui
{

class EffectControlDialog;

} // namespace gui

class Vst3EffectControls : public EffectControls
{
	Q_OBJECT
public:
	Vst3EffectControls(Vst3Effect* effect);

	int controlCount() override;
	gui::EffectControlDialog* createView() override;

	void saveSettings(QDomDocument& doc, QDomElement& element) override;
	void loadSettings(const QDomElement& element) override;
	QString nodeName() const override;

	Vst3PluginControls& pluginControls() { return m_controls; }

private:
	Vst3PluginControls m_controls;
	Vst3Effect* m_effect;
};

} // namespace lmms

#endif // VST3_EFFECT_CONTROLS_H
