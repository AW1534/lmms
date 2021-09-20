/*
 * ParamValueQueue.h - Implementation of `Steinberg::Vst::IParamValueQueue`
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

#ifndef PARAM_VALUE_QUEUE_H
#define PARAM_VALUE_QUEUE_H

#include "pluginterfaces/vst/ivstparameterchanges.h"

#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::Vst::IParamValueQueue`. Refer to the Steinberg
//! documentation. This "queue" currently only holds a single value.
class ParamValueQueue : public Implements<::Steinberg::Vst::IParamValueQueue>
{
public:
	ParamValueQueue(::Steinberg::Vst::ParamID id) : m_id{id} { }

	::Steinberg::Vst::ParamID PLUGIN_API getParameterId() override;
	::Steinberg::int32 PLUGIN_API getPointCount() override;
	::Steinberg::tresult PLUGIN_API getPoint(::Steinberg::int32 index,
		::Steinberg::int32& sampleOffset, ::Steinberg::Vst::ParamValue& value) override;
	::Steinberg::tresult PLUGIN_API addPoint(::Steinberg::int32 sampleOffset,
		::Steinberg::Vst::ParamValue value, ::Steinberg::int32& index) override;

private:
	::Steinberg::Vst::ParamID m_id;
	::Steinberg::int32 m_greatestOffset = -1;
	::Steinberg::Vst::ParamValue m_value;
};

} // namespace lmms

#endif // PARAM_VALUE_QUEUE_H
