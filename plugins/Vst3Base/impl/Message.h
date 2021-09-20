/*
 * Message.h - Implementation of `Steinberg::Vst::IMessage`
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include "pluginterfaces/vst/ivstmessage.h"

#include "impl/AttributeList.h"
#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::Vst::IMessage`. Refer to the Steinberg documentation.
class Message : public Implements<::Steinberg::Vst::IMessage>
{
public:
	Message();

	::Steinberg::FIDString PLUGIN_API getMessageID() override;
	void PLUGIN_API setMessageID(::Steinberg::FIDString id) override;
	::Steinberg::Vst::IAttributeList* PLUGIN_API getAttributes() override;

private:
	std::string m_messageId;
	DelegateRefCount<AttributeList> m_attributes;
};

} // namespace lmms

#endif // MESSAGE_H
