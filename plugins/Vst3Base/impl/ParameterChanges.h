/*
 * ParameterChanges.h - Implementation of `Steinberg::Vst::IParameterChanges`
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

#ifndef PARAMETER_CHANGES_H
#define PARAMETER_CHANGES_H

#include <vector>

#include "pluginterfaces/vst/ivstparameterchanges.h"

#include "impl/ParamValueQueue.h"
#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::Vst::IParameterChanges`. Refer to the Steinberg
//! documentation.
class ParameterChanges : public Implements<::Steinberg::Vst::IParameterChanges>
{
public:
	ParameterChanges(std::size_t maxParameters);

	// IParameterChanges methods
	::Steinberg::int32 PLUGIN_API getParameterCount() override;
	::Steinberg::Vst::IParamValueQueue* PLUGIN_API getParameterData(::Steinberg::int32 index) override;
	::Steinberg::Vst::IParamValueQueue* PLUGIN_API addParameterData(
		const ::Steinberg::Vst::ParamID& id, ::Steinberg::int32& index) override;

	// LMMS-specific methods
	void reset();

private:
	// `Uncounted` rather than `DelegateRefCount` since these don't meaningfully
	// survive between buffers, and `DelegateRefCount` doesn't work well in
	// containers.
	std::vector<Uncounted<ParamValueQueue>> m_queues;
	std::size_t m_maxParameters;
};

} // namespace lmms

#endif // PARAMETER_CHANGES_H
