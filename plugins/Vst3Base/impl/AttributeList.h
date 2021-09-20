/*
 * AttributeList.h - Implementation of `Steinberg::Vst::IAttributeList`
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

#ifndef ATTRIBUTE_LIST_H
#define ATTRIBUTE_LIST_H

#include <functional>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "pluginterfaces/vst/ivstattributes.h"

#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::Vst::IAttributeList`. Refer to the Steinberg
//! documentation.
class AttributeList : public Implements<::Steinberg::Vst::IAttributeList>
{
public:
	::Steinberg::tresult PLUGIN_API setInt(AttrID id, ::Steinberg::int64 value) override;
	::Steinberg::tresult PLUGIN_API getInt(AttrID id, ::Steinberg::int64& value) override;
	::Steinberg::tresult PLUGIN_API setFloat(AttrID id, double value) override;
	::Steinberg::tresult PLUGIN_API getFloat(AttrID id, double& value) override;
	::Steinberg::tresult PLUGIN_API setString(AttrID id, const ::Steinberg::Vst::TChar* string) override;
	::Steinberg::tresult PLUGIN_API getString(AttrID id, ::Steinberg::Vst::TChar* string, ::Steinberg::uint32 size) override;
	::Steinberg::tresult PLUGIN_API setBinary(AttrID id, const void* data, ::Steinberg::uint32 size) override;
	::Steinberg::tresult PLUGIN_API getBinary(AttrID id, const void*& data, ::Steinberg::uint32& size) override;

private:
	template<typename T, typename Func>
	::Steinberg::tresult getValue(AttrID id, Func callback) const;

	using IntType = ::Steinberg::int64;
	using FloatType = double;
	using StringType = std::basic_string<::Steinberg::Vst::TChar>;
	using BinaryType = std::vector<char>;

	std::map<std::string, std::variant<IntType, FloatType, StringType, BinaryType>, std::less<>> m_attributes;
};

} // namespace lmms

#endif // ATTRIBUTE_LIST_H
