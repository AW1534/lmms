/*
 * Vst3Effect.cpp
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

#include "Vst3Effect.h"

#include <array>
#include <memory>

#include <QDebug>

#include "AudioEngine.h"
#include "embed.h"
#include "Engine.h"

#include "plugin_export.h"
#include "Vst3SubPluginFeatures.h"

namespace lmms
{

extern "C" {

const auto PLUGIN_EXPORT vst3effect_plugin_descriptor = Plugin::Descriptor{
	LMMS_STRINGIFY(PLUGIN_NAME),
	"VST3",
	QT_TRANSLATE_NOOP("Vst3Effect",
		"plugin for using VST3 instruments inside LMMS."),
	"Dominic Clark <mrdomclark$$$gmail.com, $$$=@>",
	0x0100,
	Plugin::Effect,
	new PluginPixmapLoader{QStringLiteral("logo")},
	nullptr,
	new Vst3SubPluginFeatures{Plugin::Effect}
};

} // extern "C"

Vst3Effect::Vst3Effect(Model* parent, Descriptor::SubPluginFeatures::Key* key) :
	Effect{&vst3effect_plugin_descriptor, parent, key},
	m_plugin{key->attributes["uuid"], true},
	m_controls{this},
	m_inputBuffer(Engine::audioEngine()->framesPerPeriod() * DEFAULT_CHANNELS),
	m_outputBuffer(Engine::audioEngine()->framesPerPeriod() * DEFAULT_CHANNELS)
{
	m_plugin.setActive(true);
	m_plugin.setProcessing(true);
}

bool Vst3Effect::processAudioBuffer(sampleFrame* buf, fpp_t frames)
{
	if (!isEnabled() || !isRunning()) { return false; }
	Q_ASSERT_X(frames * 2 <= m_inputBuffer.size(), __func__, "too many frames in buffer");
	auto inputBuffers = std::array<sample_t*, 2>{m_inputBuffer.data(), m_inputBuffer.data() + frames};
	auto outputBuffers = std::array<sample_t*, 2>{m_outputBuffer.data(), m_outputBuffer.data() + frames};
	for (fpp_t i = 0; i < frames; ++i)
	{
		inputBuffers[0][i] = buf[i][0];
		inputBuffers[1][i] = buf[i][1];
	}
	m_plugin.process(inputBuffers.data(), outputBuffers.data(), frames);
	const auto dry = dryLevel(), wet = wetLevel();
	auto outSum = 0.;
	for (fpp_t i = 0; i < frames; i++)
	{
		buf[i][0] = buf[i][0] * dry + outputBuffers[0][i] * wet;
		buf[i][1] = buf[i][1] * dry + outputBuffers[1][i] * wet;
		outSum += buf[i][0] * buf[i][0] + buf[i][1] * buf[i][1];
	}
	checkGate(outSum / frames);
	return isRunning();
}

EffectControls* Vst3Effect::controls()
{
	return &m_controls;
}

extern "C"
{

PLUGIN_EXPORT Plugin* lmms_plugin_main(Model* parent, void* data)
{
	try
	{
		return new Vst3Effect{parent, static_cast<Plugin::Descriptor::SubPluginFeatures::Key*>(data)};
	}
	catch (const PluginInstantiationError& e)
	{
		qWarning() << "Failed to instantiate VST3 effect:" << e.what();
		return nullptr;
	}
}

} // extern "C"

} // namespace lmms
