/*
 * Vst3Instrument.h
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

#ifndef VST3_INSTRUMENT_H
#define VST3_INSTRUMENT_H

#include <vector>

#include "Instrument.h"
#include "InstrumentView.h"
#include "lmms_basics.h"
#include "Vst3Plugin.h"
#include "Vst3PluginControls.h"

namespace lmms
{

namespace gui
{

class Vst3InstrumentView;

} // namespace gui

class Vst3Instrument : public Instrument
{
	Q_OBJECT
public:
	Vst3Instrument(InstrumentTrack* track, Descriptor::SubPluginFeatures::Key* key);
	~Vst3Instrument();

	Flags flags() const override;
	gui::PluginView* instantiateView(QWidget* parent) override;

	void deleteNotePluginData(NotePlayHandle* nph) override;
	void playNote(NotePlayHandle* nph, sampleFrame* buffer) override;
	void play(sampleFrame* workingBuffer) override;

	QString nodeName() const override;
	void saveSettings(QDomDocument& doc, QDomElement& element) override;
	void loadSettings(const QDomElement& element) override;

private:
	Vst3Plugin m_plugin;
	Vst3PluginControls m_controls;
	std::vector<sample_t> m_buffers[2];

	friend class gui::Vst3InstrumentView;
};

namespace gui
{

class Vst3InstrumentView : public InstrumentView
{
	Q_OBJECT
public:
	Vst3InstrumentView(Vst3Instrument* instrument, QWidget* parent);
};

} // namespace gui

} // namespace lmms

#endif // VST3_INSTRUMENT_H
