/*
 * Vst3Base.h - Export definitions for the VST3 base library
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

#include "Plugin.h"

#include "vst3base_export.h"
#include "Vst3Manager.h"

namespace lmms
{

extern "C" {

Plugin::Descriptor VST3BASE_EXPORT vst3base_plugin_descriptor =
{
	LMMS_STRINGIFY(PLUGIN_NAME),
	"VST3 Base",
	QT_TRANSLATE_NOOP("Vst3Base",
		"Common base library for VST3 effects and instruments"),
	"Dominic Clark <mrdomclark$$$gmail.com, $$$=@>",
	0x0100,
	Plugin::Library,
	nullptr,
	nullptr,
	nullptr
};

VST3BASE_EXPORT void lmms_plugin_init()
{
	Vst3Manager::initialize();
}

VST3BASE_EXPORT void lmms_plugin_deinit()
{
	Vst3Manager::deinitialize();
}

}

} // namespace lmms
