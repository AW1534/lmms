/*
 * Vst3EffectControlDialog.h
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

#ifndef VST3_EFFECT_CONTROL_DIALOG_H
#define VST3_EFFECT_CONTROL_DIALOG_H

#include "EffectControlDialog.h"

namespace lmms
{

class Vst3Effect;
class Vst3EffectControls;

namespace gui
{

class Vst3EffectControlDialog : public EffectControlDialog
{
	Q_OBJECT
public:
	Vst3EffectControlDialog(Vst3EffectControls* controls, Vst3Effect* effect);

protected:
	void showEvent(QShowEvent* se);
};

} // namespace gui

} // namespace lmms

#endif // VST3_EFFECT_CONTROL_DIALOG_H
