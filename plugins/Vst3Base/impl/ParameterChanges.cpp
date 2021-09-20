/*
 * ParameterChanges.cpp - Implementation of `Steinberg::Vst::IParameterChanges`
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

#include "impl/ParameterChanges.h"

#include <algorithm>

namespace lmms
{

ParameterChanges::ParameterChanges(std::size_t maxParameters) :
	m_maxParameters{maxParameters}
{
	m_queues.reserve(maxParameters);
}

::Steinberg::int32 PLUGIN_API ParameterChanges::getParameterCount()
{
	return static_cast<::Steinberg::int32>(m_queues.size());
}

::Steinberg::Vst::IParamValueQueue* PLUGIN_API ParameterChanges::getParameterData(::Steinberg::int32 index)
{
	if (index < 0 || index >= m_queues.size()) { return nullptr; }
	return &m_queues[index];
}

::Steinberg::Vst::IParamValueQueue* PLUGIN_API ParameterChanges::addParameterData(const ::Steinberg::Vst::ParamID& id, ::Steinberg::int32& index)
{
	auto it = std::find_if(m_queues.begin(), m_queues.end(),
		[id](auto& queue) { return queue.getParameterId() == id; });
	if (it != m_queues.end())
	{
		index = static_cast<::Steinberg::int32>(it - m_queues.begin());
		return &*it;
	}
	if (m_queues.size() >= m_maxParameters) { return nullptr; }
	index = static_cast<::Steinberg::int32>(m_queues.size());
	return &m_queues.emplace_back(id);
}

void ParameterChanges::reset()
{
	m_queues.clear();
}

} // namespace lmms
