/*
 * Vst3PluginView.h - Qt widget for embedding VST3 plugin views
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

#ifndef VST3_PLUGIN_VIEW_H
#define VST3_PLUGIN_VIEW_H

#ifdef LMMS_BUILD_LINUX
#	include <map>
#	include <memory>
#
#	include <QSocketNotifier>
#	include <QTimer>
#
#	include "X11EmbedContainer.h"
#endif

#include <QWidget>

#include "pluginterfaces/base/smartpointer.h"
#include "pluginterfaces/gui/iplugview.h"

#include "vst3base_export.h"
#include "Vst3Core.h"

namespace lmms::gui
{

#ifdef LMMS_BUILD_LINUX

struct DeleteLater
{
	operator()(QObject* object) { object->deleteLater(); }
};

class EventHandlerInfo
{
public:
	EventHandlerInfo(::Steinberg::Linux::IEventHandler* handler, ::Steinberg::Linux::FileDescriptor fd);
	~EventHandlerInfo();

	EventHandlerInfo(EventHandlerInfo&&) = default;
	EventHandlerInfo& operator=(EventHandlerInfo&&) = default;

private:
	std::unique_ptr<QSocketNotifier, DeleteLater> m_read;
	std::unique_ptr<QSocketNotifier, DeleteLater> m_write;
	std::unique_ptr<QSocketNotifier, DeleteLater> m_exception;
};

class TimerInfo
{
public:
	TimerInfo(::Steinberg::Linux::ITimerHandler* handler, ::Steinberg::Linux::TimerInterval milliseconds);
	~TimerInfo();

	TimerInfo(TimerInfo&&) = default;
	TimerInfo& operator=(TimerInfo&&) = default;

private:
	std::unique_ptr<QTimer, DeleteLater> m_timer;
};

#endif // LMMS_BUILD_LINUX

class VST3BASE_EXPORT Vst3PluginView :
#ifdef LMMS_BUILD_LINUX
	public QX11EmbedContainer,
	public Implements<::Steinberg::IPlugFrame, ::Steinberg::Linux::IRunLoop>
#else
	public QWidget,
	public Implements<::Steinberg::IPlugFrame>
#endif
{
	Q_OBJECT
public:
	explicit Vst3PluginView(::Steinberg::IPtr<::Steinberg::IPlugView> view, QWidget* parent = nullptr);
	~Vst3PluginView() override;

	// IPlugFrame methods
	::Steinberg::tresult PLUGIN_API resizeView(::Steinberg::IPlugView* view, ::Steinberg::ViewRect* newSize) override;

#ifdef LMMS_BUILD_LINUX
	// IRunLoop methods
	::Steinberg::tresult PLUGIN_API registerEventHandler(
		::Steinberg::Linux::IEventHandler* handler, ::Steinberg::Linux::FileDescriptor fd) override;
	::Steinberg::tresult PLUGIN_API unregisterEventHandler(::Steinberg::Linux::IEventHandler* handler) override;
	::Steinberg::tresult PLUGIN_API registerTimer(
		::Steinberg::Linux::ITimerHandler* handler, ::Steinberg::Linux::TimerInterval milliseconds) override;
	::Steinberg::tresult PLUGIN_API unregisterTimer(::Steinberg::Linux::ITimerHandler* handler) override;
#endif // LMMS_BUILD_LINUX

protected:
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
#ifdef LMMS_BUILD_LINUX
	using Base = QX11EmbedContainer;
#else
	using Base = QWidget;
#endif

	::Steinberg::IPtr<::Steinberg::IPlugView> m_view;
#ifdef LMMS_BUILD_LINUX
	std::map<::Steinberg::Linux::IEventHandler*, EventHandlerInfo> m_eventHandlers;
	std::map<::Steinberg::Linux::ITimerHandler*, TimerInfo> m_timers;
#endif // LMMS_BUILD_LINUX
};

} // namespace lmms::gui

#endif // VST3_PLUGIN_VIEW_H
