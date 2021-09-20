/*
 * EventList.h - Implementation of `Steinberg::Vst::IEventList`
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

#ifndef EVENT_LIST_H
#define EVENT_LIST_H

#include <cstddef>
#include <vector>

#include "pluginterfaces/vst/ivstevents.h"

#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::Vst::IEventList`. Refer to the Steinberg
//! documentation.
class EventList : public Implements<::Steinberg::Vst::IEventList>
{
public:
	EventList(std::size_t maxEvents);

	// IEventList methods
	::Steinberg::int32 PLUGIN_API getEventCount() override;
	::Steinberg::tresult PLUGIN_API getEvent(::Steinberg::int32 index, ::Steinberg::Vst::Event& e) override;
	::Steinberg::tresult PLUGIN_API addEvent(::Steinberg::Vst::Event& e) override;

	// LMMS-specific methods
	void reset();
	void sort();

private:
	std::vector<::Steinberg::Vst::Event> m_events;
	std::size_t m_maxEvents;
};

} // namespace lmms

#endif // EVENT_LIST_H
