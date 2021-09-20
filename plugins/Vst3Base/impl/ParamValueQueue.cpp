/*
 * ParamValueQueue.cpp - Implementation of `Steinberg::Vst::IParamValueQueue`
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

#include "ParamValueQueue.h"

namespace lmms
{

::Steinberg::Vst::ParamID PLUGIN_API ParamValueQueue::getParameterId()
{
	return m_id;
}

::Steinberg::int32 PLUGIN_API ParamValueQueue::getPointCount()
{
	return m_greatestOffset >= 0 ? 1 : 0;
}

::Steinberg::tresult PLUGIN_API ParamValueQueue::getPoint(::Steinberg::int32 index, ::Steinberg::int32 &sampleOffset, ::Steinberg::Vst::ParamValue &value)
{
	sampleOffset = 0;
	value = m_value;
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API ParamValueQueue::addPoint(::Steinberg::int32 sampleOffset, ::Steinberg::Vst::ParamValue value, ::Steinberg::int32 &index)
{
	// Store the latest sample we are given
	if (sampleOffset >= m_greatestOffset)
	{
		m_greatestOffset = sampleOffset;
		m_value = value;
	}
	index = 0;
	return ::Steinberg::kResultOk;
}

} // namespace lmms
