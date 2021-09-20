/*
 * Vst3Plugin.h
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

#ifndef VST3_PLUGIN_H
#define VST3_PLUGIN_H

#include <atomic>
#include <stdexcept>

#include <QString>

#include "pluginterfaces/base/smartpointer.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"

#include "lmms_basics.h"
#include "impl/EventList.h"
#include "impl/ParameterChanges.h"
#include "Vst3Manager.h"
#include "vst3base_export.h"

namespace lmms
{

class NotePlayHandle;

class PluginInstantiationError : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class VST3BASE_EXPORT Vst3Plugin
{
public:
	Vst3Plugin(const QString& uuid, bool isEffect);
	~Vst3Plugin();

	void setupProcessing();
	void setActive(bool active);
	bool isActive() const { return m_active; }

	void processPlayHandle(NotePlayHandle* nph);
	void process(sample_t** in, sample_t** out, f_cnt_t frames);
	void setProcessing(bool processing);
	bool isProcessing() const { return m_processing; }

private:
	const Vst3Module* m_module;
	::Steinberg::IPtr<::Steinberg::Vst::IComponent> m_component;
	::Steinberg::IPtr<::Steinberg::Vst::IAudioProcessor> m_processor;
	Uncounted<ParameterChanges> m_changesIn;
	Uncounted<ParameterChanges> m_changesOut;
	Uncounted<EventList> m_eventsIn;
	Uncounted<EventList> m_eventsOut;
	std::atomic_int m_noteIdCounter;
	f_cnt_t m_framesPlayed;
	bool m_active;
	bool m_processing;
	bool m_effect;

	friend class Vst3PluginControls;
};

} // namespace lmms

#endif // VST3_PLUGIN_H
