/*
 * Vst3SubPluginFeatures.h
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

#ifndef VST3_SUB_PLUGIN_FEATURES_H
#define VST3_SUB_PLUGIN_FEATURES_H

#include "Plugin.h"

#include "vst3base_export.h"

namespace lmms
{

class VST3BASE_EXPORT Vst3SubPluginFeatures : public Plugin::Descriptor::SubPluginFeatures
{
public:
	Vst3SubPluginFeatures(Plugin::PluginTypes type);

	void fillDescriptionWidget(QWidget* desc, const Key* key) const override;
	void listSubPluginKeys(const Plugin::Descriptor* desc, KeyList& kl) const override;
};

} // namespace lmms

#endif // VST3_SUB_PLUGIN_FEATURES_H
