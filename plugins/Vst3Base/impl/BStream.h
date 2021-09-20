/*
 * BStream.h - Implementation of `Steinberg::Vst::IBStream`
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

#ifndef BSTREAM_H
#define BSTREAM_H

#include <QByteArray>
#include <QBuffer>

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstattributes.h"

#include "impl/AttributeList.h"
#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::IBStream`. Refer to the Steinberg documentation.
class BStream : public Implements<::Steinberg::IBStream, ::Steinberg::ISizeableStream, ::Steinberg::Vst::IStreamAttributes>
{
public:
	BStream();
	~BStream();

	// IBStream methods
	::Steinberg::tresult PLUGIN_API read(void* buffer, ::Steinberg::int32 numBytes,
		::Steinberg::int32* numBytesRead = nullptr) override;
	::Steinberg::tresult PLUGIN_API write(void* buffer, ::Steinberg::int32 numBytes,
		::Steinberg::int32* numBytesWritten = nullptr) override;
	::Steinberg::tresult PLUGIN_API seek(::Steinberg::int64 pos, ::Steinberg::int32 mode,
		::Steinberg::int64* result = nullptr) override;
	::Steinberg::tresult PLUGIN_API tell(::Steinberg::int64* pos) override;

	// ISizeableStream methods
	::Steinberg::tresult PLUGIN_API getStreamSize(::Steinberg::int64& size) override;
	::Steinberg::tresult PLUGIN_API setStreamSize(::Steinberg::int64 size) override;

	// IStreamAttributes methods
	::Steinberg::tresult PLUGIN_API getFileName(::Steinberg::Vst::String128 name) override;
	::Steinberg::Vst::IAttributeList* PLUGIN_API getAttributes() override;

	// LMMS-specific methods
	QByteArray toBase64() const { return m_stream.buffer().toBase64(); }
	void fromBase64(const QByteArray& base64)
	{
		m_stream.close();
		m_stream.setData(QByteArray::fromBase64(base64));
		m_stream.open(QIODevice::ReadWrite);
	}

private:
	QBuffer m_stream;
	DelegateRefCount<AttributeList> m_attributes;
};

} // namespace lmms

#endif // BSTREAM_H
