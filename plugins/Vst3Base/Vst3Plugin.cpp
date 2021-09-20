/*
 * Vst3Plugin.cpp
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

#include "Vst3Plugin.h"

#include <utility>

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "pluginterfaces/vst/vstspeaker.h"

#include "AudioEngine.h"
#include "Engine.h"
#include "MeterModel.h"
#include "NotePlayHandle.h"
#include "Song.h"
#include "TimeLineWidget.h"
#include "TimePos.h"

#include "impl/HostApplication.h"

namespace lmms
{

constexpr unsigned int NOTE_ON_MASK = 0x80000000;
constexpr unsigned int NOTE_ID_MASK = 0x7fffffff;

Vst3Plugin::Vst3Plugin(const QString& uuid, bool isEffect) :
	m_changesIn{256},
	m_changesOut{256},
	m_eventsIn{isEffect ? 0 : 256},
	m_eventsOut{0},
	m_noteIdCounter{0},
	m_framesPlayed{0},
	m_active{false},
	m_processing{false},
	m_effect{isEffect}
{
	const auto info = Vst3Manager::instance().pluginInfoForUuid(uuid);
	if (!info) { throw PluginInstantiationError{"Could not find plugin in database."}; }

	m_module = Vst3Manager::instance().getModule(info->module());
	if (!m_module) { throw PluginInstantiationError{"Could not load plugin module."}; }

	m_component = m_module->createInstance<::Steinberg::Vst::IComponent>(info->uuid());
	if (!m_component) { throw PluginInstantiationError{"Could not instantiate plugin."}; }

	m_processor = interface_cast<::Steinberg::Vst::IAudioProcessor>(m_component);
	if (!m_processor) { throw PluginInstantiationError{"Plugin is not an audio processor."}; }

	if (m_component->initialize(HostApplication::instance()->asFUnknown()) != ::Steinberg::kResultOk)
	{ throw PluginInstantiationError{"Plugin initialization failed."}; }

	setupProcessing();
}

Vst3Plugin::~Vst3Plugin()
{
	setProcessing(false);
	setActive(false);
	m_component->terminate();
}

class Vst3PluginDeactivator
{
public:
	Vst3PluginDeactivator(Vst3Plugin* plugin) :
		m_plugin(plugin),
		m_active(plugin->isActive()),
		m_processing(plugin->isProcessing())
	{
		m_plugin->setProcessing(false);
		m_plugin->setActive(false);
	}

	~Vst3PluginDeactivator()
	{
		m_plugin->setActive(m_active);
		m_plugin->setProcessing(m_processing);
	}

private:
	Vst3Plugin* m_plugin;
	bool m_active;
	bool m_processing;
};

void Vst3Plugin::setupProcessing()
{
	const auto deactivator = Vst3PluginDeactivator{this};
	auto setup = ::Steinberg::Vst::ProcessSetup{
		::Steinberg::Vst::kRealtime, // TODO: Use kOffline for export?
		::Steinberg::Vst::kSample32,
		Engine::audioEngine()->framesPerPeriod(),
		static_cast<::Steinberg::Vst::SampleRate>(Engine::audioEngine()->processingSampleRate())
	};
	m_processor->setupProcessing(setup);
	::Steinberg::Vst::SpeakerArrangement arrangement[1] = {::Steinberg::Vst::SpeakerArr::kStereo};
	if (m_effect)
	{
		m_processor->setBusArrangements(arrangement, 1, arrangement, 1);
		m_component->activateBus(::Steinberg::Vst::kAudio, ::Steinberg::Vst::kInput, 0, true);
		m_component->activateBus(::Steinberg::Vst::kAudio, ::Steinberg::Vst::kOutput, 0, true);
	}
	else
	{
		m_processor->setBusArrangements(nullptr, 0, arrangement, 1);
		m_component->activateBus(::Steinberg::Vst::kEvent, ::Steinberg::Vst::kInput, 0, true);
		m_component->activateBus(::Steinberg::Vst::kAudio, ::Steinberg::Vst::kOutput, 0, true);
	}
}

void Vst3Plugin::setActive(bool active)
{
	if (active != m_active)
	{
		m_component->setActive(active);
		m_active = active;
	}
}

void Vst3Plugin::processPlayHandle(NotePlayHandle* nph)
{
	// We use the NotePlayHandle's plugin data pointer as an integer instead
	// Lower bits are the note id, highest bit is a note-on flag
	static_assert(sizeof(unsigned int) <= sizeof(void*), "insufficient room for note data");
	const auto data = reinterpret_cast<unsigned int*>(&nph->m_pluginData);
	if ((*data & NOTE_ON_MASK) == 0 && !nph->isReleased())
	{
		*data |= NOTE_ON_MASK;
		*data |= (m_noteIdCounter++ & NOTE_ID_MASK);
		::Steinberg::Vst::Event event;
		event.busIndex = 0;
		event.sampleOffset = nph->offset();
		event.ppqPosition = nph->pos().getTicks() * 4.0 / DefaultTicksPerBar;
		event.flags = 0;
		event.type = ::Steinberg::Vst::Event::kNoteOnEvent;
		event.noteOn.channel = 0;
		event.noteOn.pitch = nph->midiKey();
		event.noteOn.tuning = 0.0f;
		event.noteOn.velocity = nph->getVolume() / 200.0f;
		event.noteOn.length = nph->frames();
		event.noteOn.noteId = *data & NOTE_ID_MASK;
		m_eventsIn.addEvent(event);
	}
	else if ((*data & NOTE_ON_MASK) != 0 && nph->isReleased())
	{
		*data &= ~NOTE_ON_MASK;
		::Steinberg::Vst::Event event;
		event.busIndex = 0;
		event.sampleOffset = nph->framesBeforeRelease();
		event.ppqPosition = nph->endPos().getTicks() * 4.0 / DefaultTicksPerBar;
		event.flags = 0;
		event.type = ::Steinberg::Vst::Event::kNoteOffEvent;
		event.noteOff.channel = 0;
		event.noteOff.pitch = nph->midiKey();
		event.noteOff.velocity = 0.0f;
		event.noteOff.noteId = *data & NOTE_ID_MASK;
		event.noteOff.tuning = 0.0f;
		m_eventsIn.addEvent(event);
	}
}

void Vst3Plugin::process(sample_t** in, sample_t** out, f_cnt_t frames)
{
	if (!m_processing) { return; }

	// Set up audio input
	::Steinberg::Vst::AudioBusBuffers inBuses[1];
	inBuses[0].numChannels = 2;
	inBuses[0].silenceFlags = 0;
	inBuses[0].channelBuffers32 = in;

	// Set up audio output
	::Steinberg::Vst::AudioBusBuffers outBuses[1];
	outBuses[0].numChannels = 2;
	outBuses[0].silenceFlags = 0;
	outBuses[0].channelBuffers32 = out;

	// Set up process context
	::Steinberg::Vst::ProcessContext context;
	const auto song = Engine::getSong();
	const auto& pos = song->getPlayPos();
	const gui::TimeLineWidget *const timeline = pos.m_timeLine;
	const auto& timeSig = song->getTimeSigModel();
	context.state = ::Steinberg::Vst::ProcessContext::kProjectTimeMusicValid
		| ::Steinberg::Vst::ProcessContext::kBarPositionValid
		| ::Steinberg::Vst::ProcessContext::kTempoValid
		| ::Steinberg::Vst::ProcessContext::kTimeSigValid;
	if (song->isPlaying())
	{
		context.state |= ::Steinberg::Vst::ProcessContext::kPlaying;
	}
	if (timeline && timeline->loopPointsEnabled())
	{
		context.state |= ::Steinberg::Vst::ProcessContext::kCycleActive
			| ::Steinberg::Vst::ProcessContext::kCycleValid;
		context.cycleStartMusic = timeline->loopBegin().getTicks() * 4.0 / DefaultTicksPerBar;
		context.cycleEndMusic = timeline->loopEnd().getTicks() * 4.0 / DefaultTicksPerBar;
	}
	context.sampleRate = static_cast<::Steinberg::Vst::SampleRate>(Engine::audioEngine()->processingSampleRate());
	context.projectTimeSamples = m_framesPlayed;
	context.projectTimeMusic = pos.getTicks() * 4.0 / DefaultTicksPerBar;
	context.barPositionMusic = pos.toAbsoluteBar().getTicks() * 4.0 / DefaultTicksPerBar;
	context.tempo = song->getTempo();
	context.timeSigNumerator = timeSig.getNumerator();
	context.timeSigDenominator = timeSig.getDenominator();

	// Set up process data
	::Steinberg::Vst::ProcessData data;
	data.processMode = ::Steinberg::Vst::kRealtime; // TODO: Use kOffline for export?
	data.symbolicSampleSize = ::Steinberg::Vst::kSample32;
	data.numSamples = frames;
	data.numInputs = m_effect ? 1 : 0;
	data.numOutputs = 1;
	data.inputs = inBuses;
	data.outputs = outBuses;
	data.inputParameterChanges = &m_changesIn;
	data.outputParameterChanges = &m_changesOut;
	data.inputEvents = &m_eventsIn;
	data.outputEvents = &m_eventsOut;
	data.processContext = &context;

	// Do processing
	m_eventsIn.sort();
	m_changesOut.reset();
	m_eventsOut.reset();
	m_processor->process(data);
	m_changesIn.reset();
	m_eventsIn.reset();

	// Update state
	/*for(int i = 0, count = m_changesOut.getParameterCount(); i < count; ++i)
	{
		::Steinberg::Vst::IParamValueQueue* const queue = m_changesOut.getParameterData(i);
		const ::Steinberg::Vst::ParamID id = queue->getParameterId();
		const int points = queue->getPointCount();
		if (points > 0)
		{
			::Steinberg::int32 sampleOffset;
			::Steinberg::Vst::ParamValue value;
			queue->getPoint(points - 1, sampleOffset, value);
			m_controller->setParamNormalized(id, value);
		}
	}*/
	m_framesPlayed += frames;
}

void Vst3Plugin::setProcessing(bool processing)
{
	if (processing != m_processing)
	{
		m_processor->setProcessing(processing);
		m_processing = processing;
	}
}

} // namespace lmms
