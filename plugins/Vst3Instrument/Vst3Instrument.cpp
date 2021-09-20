/*
 * Vst3Instrument.cpp
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

#include "Vst3Instrument.h"

#include <array>

#include <QDebug>
#include <QVBoxLayout>

#include "AudioEngine.h"
#include "embed.h"
#include "Engine.h"
#include "InstrumentPlayHandle.h"
#include "InstrumentTrack.h"
#include "plugin_export.h"
#include "Vst3PluginView.h"
#include "Vst3SubPluginFeatures.h"

namespace lmms
{

extern "C"
{

const auto PLUGIN_EXPORT vst3instrument_plugin_descriptor = Plugin::Descriptor{
	LMMS_STRINGIFY(PLUGIN_NAME),
	"VST3",
	QT_TRANSLATE_NOOP("Vst3Instrument",
		"plugin for using VST3 instruments inside LMMS."),
	"Dominic Clark <mrdomclark$$$gmail.com, $$$=@>",
	0x0100,
	Plugin::Instrument,
	new PluginPixmapLoader{QStringLiteral("logo")},
	nullptr,
	new Vst3SubPluginFeatures{Plugin::Instrument}
};

} // extern "C"

Vst3Instrument::Vst3Instrument(InstrumentTrack* track, Descriptor::SubPluginFeatures::Key* key) :
	Instrument{track, &vst3instrument_plugin_descriptor, key},
	m_plugin{key->attributes["uuid"], false},
	m_controls{&m_plugin}
{
	const auto fpp = Engine::audioEngine()->framesPerPeriod();
	m_buffers[0].resize(fpp);
	m_buffers[1].resize(fpp);
	Engine::audioEngine()->addPlayHandle(new InstrumentPlayHandle{this, track});
	m_plugin.setActive(true);
	m_plugin.setProcessing(true);
}

Vst3Instrument::~Vst3Instrument()
{
	Engine::audioEngine()->removePlayHandlesOfTypes(instrumentTrack(),
		PlayHandle::TypeNotePlayHandle | PlayHandle::TypeInstrumentPlayHandle);
}

Instrument::Flags Vst3Instrument::flags() const
{
	return IsSingleStreamed;
}

gui::PluginView *Vst3Instrument::instantiateView(QWidget* parent)
{
	return new gui::Vst3InstrumentView{this, parent};
}

void Vst3Instrument::deleteNotePluginData(NotePlayHandle* nph)
{
	// Sometimes this will get called without us having done the note-off
	// yet, so pass the play handle to the plugin to do so if needed.
	m_plugin.processPlayHandle(nph);
}

void Vst3Instrument::playNote(NotePlayHandle* nph, sampleFrame* buffer)
{
	Q_UNUSED(buffer)
	m_plugin.processPlayHandle(nph);
}

void Vst3Instrument::play(sampleFrame* workingBuffer)
{
	auto buffers = std::array<sample_t*, 2>{&m_buffers[0][0], &m_buffers[1][0]};
	const fpp_t fpp = Engine::audioEngine()->framesPerPeriod();
	m_plugin.process(nullptr, buffers.data(), fpp);
	for (fpp_t i = 0; i < fpp; i++)
	{
		workingBuffer[i][0] = buffers[0][i];
		workingBuffer[i][1] = buffers[1][i];
	}
	instrumentTrack()->processAudioBuffer(workingBuffer, fpp, nullptr);
}

QString Vst3Instrument::nodeName() const
{
	return "vst3instrument";
}

void Vst3Instrument::saveSettings(QDomDocument& doc, QDomElement& element)
{
	m_controls.saveSettings(doc, element);
}

void Vst3Instrument::loadSettings(const QDomElement& element)
{
	m_controls.loadSettings(element);
}

namespace gui
{

Vst3InstrumentView::Vst3InstrumentView(Vst3Instrument* instrument, QWidget* parent) :
	InstrumentView(instrument, parent)
{
	const auto layout = new QVBoxLayout{this};
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(instrument->m_controls.createPluginView(nullptr));
}

} // namespace gui

extern "C"
{

PLUGIN_EXPORT Plugin* lmms_plugin_main(Model* parent, void* data)
{
	try
	{
		return new Vst3Instrument{static_cast<InstrumentTrack*>(parent),
			static_cast<Plugin::Descriptor::SubPluginFeatures::Key*>(data)};
	}
	catch (const PluginInstantiationError& e)
	{
		qWarning() << "Failed to instantiate VST3 instrument:" << e.what();
		return nullptr;
	}
}

} // extern "C"

} // namespace lmms
