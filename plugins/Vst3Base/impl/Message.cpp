/*
 * Message.cpp - Implementation of `Steinberg::Vst::IMessage`
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

#include "impl/Message.h"

namespace lmms
{

Message::Message() : m_attributes(this) { }

::Steinberg::FIDString PLUGIN_API Message::getMessageID()
{
	return m_messageId.c_str();
}

void PLUGIN_API Message::setMessageID(::Steinberg::FIDString id)
{
	m_messageId = id;
}

::Steinberg::Vst::IAttributeList* PLUGIN_API Message::getAttributes()
{
	return &m_attributes;
}

} // namespace lmms
