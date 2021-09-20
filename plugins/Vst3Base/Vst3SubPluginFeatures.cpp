/*
 * Vst3SubPluginFeatures.cpp
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

#include "Vst3SubPluginFeatures.h"

#include <QLabel>

#include "Vst3Manager.h"

namespace lmms
{

Vst3SubPluginFeatures::Vst3SubPluginFeatures(Plugin::PluginTypes type) :
	SubPluginFeatures{type}
{ }

void Vst3SubPluginFeatures::fillDescriptionWidget(QWidget* desc, const Key* key) const
{
	const auto info = Vst3Manager::instance().pluginInfoForUuid(key->attributes["uuid"]);
	Q_ASSERT(info); // It existed earlier when we returned it from `listSubPluginKeys`...

	// Cast to void to silence nodiscard warnings - Qt manages the memory itself
	static_cast<void>(new QLabel(QWidget::tr("Name: %1").arg(info->name()), desc));

	if (const auto& version = info->version(); !version.isEmpty())
	{
		static_cast<void>(new QLabel(QWidget::tr("Version: %1").arg(version), desc));
	}

	if (const auto& vendor = info->vendor(); !vendor.isEmpty())
	{
		static_cast<void>(new QLabel(QWidget::tr("Vendor: %1").arg(vendor), desc));
	}

	if (const auto& url = info->url(); !url.isEmpty())
	{
		const auto hyperlink = QStringLiteral("<a href=\"%1\">%1</a>").arg(url);
		const auto label = new QLabel(QWidget::tr("URL: %1").arg(hyperlink), desc);
		label->setOpenExternalLinks(true);
	}

	if (auto email = info->email(); !email.isEmpty())
	{
		if (email.startsWith(QStringLiteral("mailto:"))) { email.remove(0, 7); }
		const auto hyperlink = QStringLiteral("<a href=\"mailto:%1\">%1</a>").arg(email);
		const auto label = new QLabel(QWidget::tr("Email: %1").arg(hyperlink), desc);
		label->setOpenExternalLinks(true);
	}
}

void Vst3SubPluginFeatures::listSubPluginKeys(const Plugin::Descriptor* desc, KeyList& kl) const
{
	for (const auto& [uuid, info] : Vst3Manager::instance())
	{
		if ((m_type == Plugin::Instrument && info.isInstrument())
		 || (m_type == Plugin::Effect && info.isEffect()))
		{
			kl << Key(desc, info.name(), {{"uuid", uuid}, {"name", info.name()}});
		}
	}
}

} // namespace lmms
