/*
 * Vst3EffectControls.cpp
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

#include "Vst3EffectControls.h"

#include "Vst3Effect.h"
#include "Vst3EffectControlDialog.h"

namespace lmms
{

Vst3EffectControls::Vst3EffectControls(Vst3Effect* effect) :
	EffectControls{effect},
	m_controls{&effect->m_plugin},
	m_effect{effect}
{ }

int Vst3EffectControls::controlCount()
{
	return 1;
}

gui::EffectControlDialog* Vst3EffectControls::createView()
{
	return new gui::Vst3EffectControlDialog{this, m_effect};
}

void Vst3EffectControls::saveSettings(QDomDocument& doc, QDomElement& element)
{
	m_controls.saveSettings(doc, element);
}

void Vst3EffectControls::loadSettings(const QDomElement& element)
{
	m_controls.loadSettings(element);
}

QString Vst3EffectControls::nodeName() const
{
	return m_controls.nodeName();
}

} // namespace lmms
