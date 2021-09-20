/*
 * AttributeList.cpp - Implementation of `Steinberg::Vst::IAttributeList`
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

#include "impl/AttributeList.h"

namespace lmms
{

::Steinberg::tresult PLUGIN_API AttributeList::setInt(AttrID id, ::Steinberg::int64 value)
{
	if (!id) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	m_attributes.insert_or_assign(id, value);
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API AttributeList::getInt(AttrID id, ::Steinberg::int64& value)
{
	if (!id) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	return getValue<IntType>(id,
		[&value](auto intValue)
		{
			value = intValue;
		}
	);
}

::Steinberg::tresult PLUGIN_API AttributeList::setFloat(AttrID id, double value)
{
	if (!id) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	m_attributes.insert_or_assign(id, value);
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API AttributeList::getFloat(AttrID id, double& value)
{
	if (!id) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	return getValue<FloatType>(id,
		[&value](auto floatValue)
		{
			value = floatValue;
		}
	);
}

::Steinberg::tresult PLUGIN_API AttributeList::setString(AttrID id, const ::Steinberg::Vst::TChar* string)
{
	if (!id || !string) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	m_attributes.insert_or_assign(id, string);
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API AttributeList::getString(AttrID id, ::Steinberg::Vst::TChar* string, ::Steinberg::uint32 size)
{
	if (!id || !string) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	return getValue<StringType>(id,
		[string, size](const auto& value)
		{
			value.copy(string, size / sizeof(::Steinberg::Vst::TChar));
		}
	);
}

::Steinberg::tresult PLUGIN_API AttributeList::setBinary(AttrID id, const void* data, ::Steinberg::uint32 size)
{
	if (!id || !data) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	const auto charData = static_cast<const char*>(data);
	m_attributes.insert_or_assign(id, BinaryType(charData, charData + size));
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API AttributeList::getBinary(AttrID id, const void*& data, ::Steinberg::uint32& size)
{
	if (!id) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	return getValue<BinaryType>(id,
		[&data, &size](const auto& value)
		{
			data = value.data();
			size = value.size();
		}
	);
}

template<typename T, typename Func>
::Steinberg::tresult AttributeList::getValue(AttrID id, Func callback) const
{
	if (const auto it = m_attributes.find(id); it != m_attributes.end())
	{
		if (const auto maybeValue = std::get_if<T>(&it->second))
		{
			callback(*maybeValue);
			return ::Steinberg::kResultOk;
		}
	}
	return ::Steinberg::kResultFalse;
}

} // namespace lmms
