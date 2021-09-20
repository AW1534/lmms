/*
 * Vst3Effect.h
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

#ifndef VST3_EFFECT_H
#define VST3_EFFECT_H

#include <vector>

#include "Effect.h"
#include "lmms_basics.h"

#include "Vst3EffectControls.h"
#include "Vst3Plugin.h"

namespace lmms
{

class Vst3EffectControls;

class Vst3Effect : public Effect
{
	Q_OBJECT
public:
	Vst3Effect(Model* parent, Descriptor::SubPluginFeatures::Key* key);

	bool processAudioBuffer(sampleFrame* buf, fpp_t frames) override;
	EffectControls* controls() override;

	Vst3Plugin& plugin() { return m_plugin; }

private:
	Vst3Plugin m_plugin;
	Vst3EffectControls m_controls;
	std::vector<sample_t> m_inputBuffer;
	std::vector<sample_t> m_outputBuffer;

	friend class Vst3EffectControls;
};

} // namespace lmms

#endif // VST3_EFFECT_H
