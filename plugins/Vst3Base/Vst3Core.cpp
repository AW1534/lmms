/*
 * Vst3Core.cpp - Define IIDs for all used interfaces
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

#include "Vst3Core.h"

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/gui/iplugview.h"
#include "pluginterfaces/vst/ivstattributes.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstpluginterfacesupport.h"
#include "pluginterfaces/vst/ivstunits.h"

#include "lmmsconfig.h"

DEF_CLASS_IID(Steinberg::IPlugFrame)
DEF_CLASS_IID(Steinberg::IPlugView)
#ifdef LMMS_BUILD_LINUX
DEF_CLASS_IID(Steinberg::Linux::IRunLoop)
#endif
DEF_CLASS_IID(Steinberg::Vst::IAttributeList)
DEF_CLASS_IID(Steinberg::Vst::IAudioProcessor)
DEF_CLASS_IID(Steinberg::Vst::IComponent)
DEF_CLASS_IID(Steinberg::Vst::IComponentHandler)
DEF_CLASS_IID(Steinberg::Vst::IConnectionPoint)
DEF_CLASS_IID(Steinberg::Vst::IEditController)
DEF_CLASS_IID(Steinberg::Vst::IEventList)
DEF_CLASS_IID(Steinberg::Vst::IHostApplication)
DEF_CLASS_IID(Steinberg::Vst::IMessage)
DEF_CLASS_IID(Steinberg::Vst::IParameterChanges)
DEF_CLASS_IID(Steinberg::Vst::IParamValueQueue)
DEF_CLASS_IID(Steinberg::Vst::IStreamAttributes)
DEF_CLASS_IID(Steinberg::Vst::IPlugInterfaceSupport)
DEF_CLASS_IID(Steinberg::Vst::IUnitInfo)
