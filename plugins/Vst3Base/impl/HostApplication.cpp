/*
 * HostApplication.cpp - Implements `Steinberg::Vst::IHostApplication` for LMMS
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

#include "impl/HostApplication.h"

#include <string_view>
#include <set>

#include "pluginterfaces/base/fstrdefs.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/base/smartpointer.h"
#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/vst/ivstattributes.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/vst/ivstunits.h"

#include "lmmsconfig.h"

#include "impl/AttributeList.h"
#include "impl/Message.h"

namespace lmms
{

namespace
{

template<typename... Interfaces>
const auto s_uuidSet = std::set{Interfaces::iid...};

} // namespace

::Steinberg::tresult PLUGIN_API HostApplication::getName(::Steinberg::Vst::String128 name)
{
	if (!name) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	constexpr auto lmmsName = std::basic_string_view{STR16("LMMS")};
	lmmsName.copy(name, 128);
	return ::Steinberg::kResultOk;
}

::Steinberg::tresult PLUGIN_API HostApplication::createInstance(::Steinberg::TUID cid, ::Steinberg::TUID iid, void** obj)
{
	if (!cid || !iid || !obj) [[unlikely]] { return ::Steinberg::kInvalidArgument; }

	::Steinberg::IPtr<Vst3Object> instance;
	if (isUuidOf<::Steinberg::Vst::IMessage>(cid))
	{
		instance = make_refcounted<Message>();
	}
	else if (isUuidOf<::Steinberg::Vst::IAttributeList>(cid))
	{
		instance = make_refcounted<AttributeList>();
	}

	if (instance && instance->queryInterface(iid, obj) == ::Steinberg::kResultOk)
	{
		return ::Steinberg::kResultOk;
	}
	*obj = nullptr;
	return ::Steinberg::kResultFalse;
}

::Steinberg::tresult PLUGIN_API HostApplication::isPlugInterfaceSupported(const ::Steinberg::TUID iid)
{
	if (!iid) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
	return s_uuidSet<
		::Steinberg::IPluginBase,
		::Steinberg::IPluginFactory,
		::Steinberg::IPluginFactory2,
		::Steinberg::IPluginFactory3,
		::Steinberg::IPlugView,
#ifdef LMMS_BUILD_LINUX
		::Steinberg::Linux::IEventHandler,
		::Steinberg::Linux::ITimerHandler,
#endif
		::Steinberg::Vst::IAudioProcessor,
		::Steinberg::Vst::IComponent,
		::Steinberg::Vst::IConnectionPoint,
		::Steinberg::Vst::IEditController,
		::Steinberg::Vst::IUnitInfo
	>.count(::Steinberg::FUID::fromTUID(iid)) != 0 ? ::Steinberg::kResultTrue : ::Steinberg::kResultFalse;
}

Uncounted<HostApplication> HostApplication::s_instance;

} // namespace lmms
