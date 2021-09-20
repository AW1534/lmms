/*
 * HostApplication.h - Implements `Steinberg::Vst::IHostApplication` for LMMS
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

#ifndef HOST_APPLICATION_H
#define HOST_APPLICATION_H

#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstpluginterfacesupport.h"

#include "Vst3Core.h"

namespace lmms
{

//! Implements `Steinberg::Vst::IHostApplication` for LMMS. Refer to the
//! Steinberg documentation.
class HostApplication : public Implements<::Steinberg::Vst::IHostApplication, ::Steinberg::Vst::IPlugInterfaceSupport>
{
public:
	// IHostApplication methods
	::Steinberg::tresult PLUGIN_API getName(::Steinberg::Vst::String128 name) override;
	::Steinberg::tresult PLUGIN_API createInstance(::Steinberg::TUID cid, ::Steinberg::TUID iid, void** obj) override;

	// IPlugInterfaceSupport methods
	::Steinberg::tresult PLUGIN_API isPlugInterfaceSupported(const ::Steinberg::TUID iid) override;

	//! Gets the singleton instance of `HostApplication`.
	static HostApplication* instance() { return &s_instance; }

private:
	static Uncounted<HostApplication> s_instance;
};

} // namespace lmms

#endif // HOST_APPLICATION_H
