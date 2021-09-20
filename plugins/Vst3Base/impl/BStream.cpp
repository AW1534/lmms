/*
 * BStream.cpp - Implementation of `Steinberg::Vst::IBStream`
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

#include "impl/BStream.h"

namespace lmms
{

BStream::BStream() :
	m_attributes(this)
{
	m_stream.open(QIODevice::ReadWrite);
}

BStream::~BStream()
{
	m_stream.close();
}

::Steinberg::tresult PLUGIN_API BStream::read(void* buffer,
	::Steinberg::int32 numBytes, ::Steinberg::int32* numBytesRead)
{
	if (!buffer || numBytes < 0) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	const auto read = static_cast<::Steinberg::int32>(m_stream.read(static_cast<char*>(buffer), numBytes));
	if (numBytesRead) { *numBytesRead = read == -1 ? 0 : read; }
	return read == -1 ? ::Steinberg::kResultFalse : ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API BStream::write(void* buffer,
	::Steinberg::int32 numBytes, ::Steinberg::int32* numBytesWritten)
{
	if (!buffer || numBytes < 0) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	const auto written = static_cast<::Steinberg::int32>(m_stream.write(static_cast<const char*>(buffer), numBytes));
	if (numBytesWritten) { *numBytesWritten = written == -1 ? 0 : written; }
	return written == -1 ? ::Steinberg::kResultFalse : ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API BStream::seek(::Steinberg::int64 pos,
	::Steinberg::int32 mode, ::Steinberg::int64* result)
{
	switch (mode)
	{
	case kIBSeekSet:
		break;
	case kIBSeekCur:
		pos += m_stream.pos();
		break;
	case kIBSeekEnd:
		pos += m_stream.size();
		break;
	[[unlikely]] default:
		return ::Steinberg::kInvalidArgument;
	}
	const auto success = m_stream.seek(pos);
	if (result) { *result = m_stream.pos(); }
	return success ? ::Steinberg::kResultOk : ::Steinberg::kResultFalse;
}

::Steinberg::tresult PLUGIN_API BStream::tell(::Steinberg::int64* pos)
{
	if (!pos) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	*pos = m_stream.pos();
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API BStream::getStreamSize(::Steinberg::int64& size)
{
	size = m_stream.size();
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API BStream::setStreamSize(::Steinberg::int64 size)
{
	m_stream.buffer().resize(size);
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API BStream::getFileName(::Steinberg::Vst::String128 name)
{
	if(!name) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	*name = 0;
	return ::Steinberg::kResultFalse;
}

::Steinberg::Vst::IAttributeList* PLUGIN_API BStream::getAttributes()
{
	return &m_attributes;
}

} // namespace lmms
