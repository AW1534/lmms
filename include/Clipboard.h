/*
 * Clipboard.h - the clipboard for clips, notes etc.
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef LMMS_CLIPBOARD_H
#define LMMS_CLIPBOARD_H

#include <QDomElement>
#include <QDropEvent>
#include <QPixmap>

#include "FileTypes.h"
#include "lmms_export.h"


class QMimeData;


namespace lmms
{
	enum class MimeType
	{
		StringPair, //!< string like key:value, only visible to LMMS instances
		DataFile, //!< DataFile XML, only visible to LMMS instances
		PlainText,
	};
}


namespace lmms::Clipboard
{
	// Convenience Methods
	void copyMimeData(QMimeData* m);
	const QMimeData * getMimeData();
	bool hasFormat( MimeType mT );

	// Helper methods for String data
	void LMMS_EXPORT copyString(const QString& str, MimeType mT = MimeType::PlainText);
	QString getString(MimeType mT = MimeType::PlainText);

} // namespace lmms::Clipboard


namespace lmms::MimeData
{
	// Helper methods for String Pair data
	QMimeData* fromStringPair(const QString& key, const QString& value);
	std::pair<QString, QString> toStringPair(const QMimeData* md);
} // namespace lmms::MimeData


namespace lmms::DragAndDrop
{
	void exec(QWidget* widget, QMimeData* md, const QPixmap& icon = {});
	void execStringPairDrag(const QString& key, const QString& value, const QPixmap& icon, QWidget* widget);

	//! Accept drag enter event if it contains a file of allowed type
	bool acceptFile(QDragEnterEvent* dee, const std::initializer_list<FileType> allowedTypes);
	//! Accept drag enter event if it contains a string pair of allowed type
	bool acceptStringPair(QDragEnterEvent* dee, const std::initializer_list<QString> allowedKeys);

	inline std::pair<QString, QString> getStringPair(const QDropEvent* de)
	{
		return MimeData::toStringPair(de->mimeData());
	}

	//! Get file path from drop event (empty if it doesn't match allowedType)
	LMMS_EXPORT QString getFile(const QDropEvent* de, FileType allowedType);
	//! Get file path and type from drop event
	LMMS_EXPORT std::pair<QString, FileType> getFileAndType(const QDropEvent* de);
	//! Get file path and suffix (not including dot) from drop event
	LMMS_EXPORT std::pair<QString, QString> getFileAndExt(const QDropEvent* de);


} // namespace lmms::DragAndDrop


namespace lmms
{
	// TODO move to another namespace, maybe an anonymous one in Clipboard.cpp
	inline const char * mimeType( MimeType type )
	{
		switch( type )
		{
			case MimeType::PlainText:
				return "text/plain";
			case MimeType::StringPair:
				return "application/x-lmms-stringpair";
			break;
			case MimeType::DataFile:
			default:
				return "application/x-lmms-clipboard";
				break;
		}
	}
} // namespace lmms

#endif // LMMS_CLIPBOARD_H
