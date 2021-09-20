/*
 * EventList.cpp - Implementation of `Steinberg::Vst::IEventList`
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

#include <algorithm>

#include "impl/EventList.h"

namespace lmms
{

EventList::EventList(std::size_t maxEvents) :
	m_maxEvents{maxEvents}
{
	m_events.reserve(maxEvents);
}

::Steinberg::int32 PLUGIN_API EventList::getEventCount()
{
	return m_events.size();
}

::Steinberg::tresult PLUGIN_API EventList::getEvent(::Steinberg::int32 index, ::Steinberg::Vst::Event& e)
{
	if (index < 0 || index >= m_events.size()) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	e = m_events[index];
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API EventList::addEvent(::Steinberg::Vst::Event& e)
{
	if (m_events.size() >= m_maxEvents) { return ::Steinberg::kResultFalse; }
	m_events.push_back(e);
	return ::Steinberg::kResultOk;
}

void EventList::reset()
{
	m_events.clear();
}

void EventList::sort()
{
	std::stable_sort(m_events.begin(), m_events.begin() + getEventCount(),
		[](const ::Steinberg::Vst::Event& a, const ::Steinberg::Vst::Event& b)
		{
			return a.sampleOffset < b.sampleOffset;
		}
	);
}

} // namespace lmms
