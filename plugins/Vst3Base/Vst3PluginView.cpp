/*
 * Vst3PluginView.cpp - Qt widget for embedding VST3 plugin views
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

#include "Vst3PluginView.h"

#include <utility>

#include <QDebug>
#include <QKeyEvent>
#include <QString>
#include <QWheelEvent>

#include "pluginterfaces/base/keycodes.h"

#include "lmmsconfig.h"

namespace lmms::gui
{

#if defined(LMMS_BUILD_LINUX)
	const ::Steinberg::FIDString platformType = ::Steinberg::kPlatformTypeX11EmbedWindowID;
#elif defined(LMMS_BUILD_WIN32)
	const ::Steinberg::FIDString platformType = ::Steinberg::kPlatformTypeHWND;
#elif defined(LMMS_BUILD_APPLE)
	const ::Steinberg::FIDString platformType = ::Steinberg::kPlatformTypeNSView;
#else
#	warning VST3 plugin UIs not supported on this platform
	const ::Steinberg::FIDString platformType = "";
#endif

struct Vst3KeyEvent
{
	::Steinberg::char16 key;
	::Steinberg::int16 keyCode;
	::Steinberg::int16 modifiers;
};

static Vst3KeyEvent translateKeyEvent(const QKeyEvent* event)
{
	auto vke = Vst3KeyEvent{};
	const auto text = event->text();
	vke.key = text.isEmpty() ? 0 : text.at(0).unicode();
	switch (event->key())
	{
		//case ?: vke.keyCode = ::Steinberg::KEY_BACK; break; // Not sure what this is
		case Qt::Key_Tab: vke.keyCode = ::Steinberg::KEY_TAB; break;
		case Qt::Key_Clear: vke.keyCode = ::Steinberg::KEY_CLEAR; break;
		case Qt::Key_Return: vke.keyCode = ::Steinberg::KEY_RETURN; break;
		case Qt::Key_Pause: vke.keyCode = ::Steinberg::KEY_PAUSE; break;
		case Qt::Key_Escape: vke.keyCode = ::Steinberg::KEY_ESCAPE; break;
		case Qt::Key_Space: vke.keyCode = ::Steinberg::KEY_SPACE; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NEXT; break; // Nor this
		case Qt::Key_End: vke.keyCode = ::Steinberg::KEY_END; break;
		case Qt::Key_Home: vke.keyCode = ::Steinberg::KEY_HOME; break;
		case Qt::Key_Left: vke.keyCode = ::Steinberg::KEY_LEFT; break;
		case Qt::Key_Up: vke.keyCode = ::Steinberg::KEY_UP; break;
		case Qt::Key_Right: vke.keyCode = ::Steinberg::KEY_RIGHT; break;
		case Qt::Key_Down: vke.keyCode = ::Steinberg::KEY_DOWN; break;
		case Qt::Key_PageUp: vke.keyCode = ::Steinberg::KEY_PAGEUP; break;
		case Qt::Key_PageDown: vke.keyCode = ::Steinberg::KEY_PAGEDOWN; break;
		case Qt::Key_Select: vke.keyCode = ::Steinberg::KEY_SELECT; break;
		case Qt::Key_Print: vke.keyCode = ::Steinberg::KEY_PRINT; break;
		case Qt::Key_Enter: vke.keyCode = ::Steinberg::KEY_ENTER; break;
		// It looks like this is supposed to be the print screen key, which Qt
		// doesn't expose
		//case ?: vke.keyCode = ::Steinberg::KEY_SNAPSHOT; break;
		case Qt::Key_Insert: vke.keyCode = ::Steinberg::KEY_INSERT; break;
		case Qt::Key_Delete: vke.keyCode = ::Steinberg::KEY_DELETE; break;
		case Qt::Key_Help: vke.keyCode = ::Steinberg::KEY_HELP; break;
		// Qt doesn't distinguish numpad keys in a platform-independent manner
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD0; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD1; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD2; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD3; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD4; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD5; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD6; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD7; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD8; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_NUMPAD9; break;
		case Qt::Key_Asterisk: vke.keyCode = ::Steinberg::KEY_MULTIPLY; break;
		case Qt::Key_Plus: vke.keyCode = ::Steinberg::KEY_ADD; break;
		//case ?: vke.keyCode = ::Steinberg::KEY_SEPARATOR; break; // Nor this
		case Qt::Key_Minus: vke.keyCode = ::Steinberg::KEY_SUBTRACT; break;
		case Qt::Key_Period: vke.keyCode = ::Steinberg::KEY_DECIMAL; break;
		case Qt::Key_Slash: vke.keyCode = ::Steinberg::KEY_DIVIDE; break;
		case Qt::Key_F1: vke.keyCode = ::Steinberg::KEY_F1; break;
		case Qt::Key_F2: vke.keyCode = ::Steinberg::KEY_F2; break;
		case Qt::Key_F3: vke.keyCode = ::Steinberg::KEY_F3; break;
		case Qt::Key_F4: vke.keyCode = ::Steinberg::KEY_F4; break;
		case Qt::Key_F5: vke.keyCode = ::Steinberg::KEY_F5; break;
		case Qt::Key_F6: vke.keyCode = ::Steinberg::KEY_F6; break;
		case Qt::Key_F7: vke.keyCode = ::Steinberg::KEY_F7; break;
		case Qt::Key_F8: vke.keyCode = ::Steinberg::KEY_F8; break;
		case Qt::Key_F9: vke.keyCode = ::Steinberg::KEY_F9; break;
		case Qt::Key_F10: vke.keyCode = ::Steinberg::KEY_F10; break;
		case Qt::Key_F11: vke.keyCode = ::Steinberg::KEY_F11; break;
		case Qt::Key_F12: vke.keyCode = ::Steinberg::KEY_F12; break;
		case Qt::Key_NumLock: vke.keyCode = ::Steinberg::KEY_NUMLOCK; break;
		case Qt::Key_ScrollLock: vke.keyCode = ::Steinberg::KEY_SCROLL; break;
		case Qt::Key_Shift: vke.keyCode = ::Steinberg::KEY_SHIFT; break;
		case Qt::Key_Control: vke.keyCode = ::Steinberg::KEY_CONTROL; break;
		case Qt::Key_Alt: vke.keyCode = ::Steinberg::KEY_ALT; break;
		case Qt::Key_Equal: vke.keyCode = ::Steinberg::KEY_EQUALS; break;
		case Qt::Key_Menu: vke.keyCode = ::Steinberg::KEY_CONTEXTMENU; break;
		case Qt::Key_MediaPlay: vke.keyCode = ::Steinberg::KEY_MEDIA_PLAY; break;
		case Qt::Key_MediaStop: vke.keyCode = ::Steinberg::KEY_MEDIA_STOP; break;
		case Qt::Key_MediaPrevious: vke.keyCode = ::Steinberg::KEY_MEDIA_PREV; break;
		case Qt::Key_MediaNext: vke.keyCode = ::Steinberg::KEY_MEDIA_NEXT; break;
		case Qt::Key_VolumeUp: vke.keyCode = ::Steinberg::KEY_VOLUME_UP; break;
		case Qt::Key_VolumeDown: vke.keyCode = ::Steinberg::KEY_VOLUME_DOWN; break;
		case Qt::Key_F13: vke.keyCode = ::Steinberg::KEY_F13; break;
		case Qt::Key_F14: vke.keyCode = ::Steinberg::KEY_F14; break;
		case Qt::Key_F15: vke.keyCode = ::Steinberg::KEY_F15; break;
		case Qt::Key_F16: vke.keyCode = ::Steinberg::KEY_F16; break;
		case Qt::Key_F17: vke.keyCode = ::Steinberg::KEY_F17; break;
		case Qt::Key_F18: vke.keyCode = ::Steinberg::KEY_F18; break;
		case Qt::Key_F19: vke.keyCode = ::Steinberg::KEY_F19; break;
		default:
			if (const auto keycode = event->key();
				(Qt::Key_0 <= keycode && keycode <= Qt::Key_9)
				|| (Qt::Key_A <= keycode && keycode <= Qt::Key_Z))
			{
				vke.keyCode = keycode + ::Steinberg::VKEY_FIRST_ASCII;
			}
			else
			{
				vke.keyCode = 0;
			}
			break;
	}
	const auto mods = event->modifiers();
	vke.modifiers = (mods.testFlag(Qt::ShiftModifier) ? ::Steinberg::kShiftKey : 0)
		| (mods.testFlag(Qt::AltModifier) ? ::Steinberg::kAlternateKey : 0)
		| (mods.testFlag(Qt::ControlModifier) ? ::Steinberg::kCommandKey : 0)
		| (mods.testFlag(Qt::MetaModifier) ? ::Steinberg::kControlKey : 0);
	return vke;
}

Vst3PluginView::Vst3PluginView(::Steinberg::IPtr<::Steinberg::IPlugView> view, QWidget* parent) :
	Base{parent},
	m_view{std::move(view)}
{
	if (m_view->isPlatformTypeSupported(platformType) != ::Steinberg::kResultTrue)
	{
		qWarning() << "Plugin does not support UI on this platform!";
		m_view.reset();
		return;
	}
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_NativeWindow);
	m_view->setFrame(this);
	m_view->attached(reinterpret_cast<void*>(winId()), platformType);
	if (::Steinberg::ViewRect rect; m_view->getSize(&rect) == ::Steinberg::kResultOk)
	{
		setFixedSize(rect.getWidth(), rect.getHeight());
	}
}

Vst3PluginView::~Vst3PluginView()
{
	if (m_view) { m_view->removed(); }
}

::Steinberg::tresult PLUGIN_API Vst3PluginView::resizeView(::Steinberg::IPlugView* view, ::Steinberg::ViewRect* newSize)
{
	const auto newWidth = newSize->getWidth(), newHeight = newSize->getHeight();
	if (newWidth != width() || newHeight != height())
	{
		setFixedSize(newWidth, newHeight);
		if (parentWidget()) { parentWidget()->adjustSize(); }
		view->onSize(newSize);
	}
	return ::Steinberg::kResultOk;
}

#ifdef LMMS_BUILD_LINUX

EventHandlerInfo::EventHandlerInfo(::Steinberg::Linux::IEventHandler* handler, ::Steinberg::Linux::FileDescriptor fd) :
	m_read{new QSocketNotifier{fd, QSocketNotifier::Read}},
	m_write{new QSocketNotifier{fd, QSocketNotifier::Write}},
	m_exception{new QSocketNotifier{fd, QSocketNotifier::Exception}}
{
	const auto callback = [handler, fd] { handler->onFDIsSet(fd); };
	QObject::connect(m_read.get(), &QSocketNotifier::activated, callback);
	QObject::connect(m_write.get(), &QSocketNotifier::activated, callback);
	QObject::connect(m_exception.get(), &QSocketNotifier::activated, callback);
	m_read->setEnabled(true);
	m_write->setEnabled(true);
	m_exception->setEnabled(true);
}

EventHandlerInfo::~EventHandlerInfo()
{
	if(m_read) { m_read->setEnabled(false); }
	if(m_write) { m_write->setEnabled(false); }
	if(m_exception) { m_exception->setEnabled(false); }
}

TimerInfo::TimerInfo(::Steinberg::Linux::ITimerHandler* handler, ::Steinberg::Linux::TimerInterval milliseconds) :
	m_timer{new QTimer{}}
{
	QObject::connect(m_timer.get(), &QTimer::timeout, [handler]() { handler->onTimer(); });
	m_timer->start(milliseconds);
}

TimerInfo::~TimerInfo()
{
	if(m_timer) { m_timer->stop(); }
}

::Steinberg::tresult PLUGIN_API Vst3PluginView::registerEventHandler(::Steinberg::Linux::IEventHandler* handler,
	::Steinberg::Linux::FileDescriptor fd)
{
	const auto [it, success] = m_eventHandlers.try_emplace(handler, handler, fd);
	return success ? ::Steinberg::kResultOk : ::Steinberg::kResultFalse;
}

::Steinberg::tresult PLUGIN_API Vst3PluginView::unregisterEventHandler(::Steinberg::Linux::IEventHandler* handler)
{
	if (const auto it = m_eventHandlers.find(handler); it != m_eventHandlers.end())
	{
		m_eventHandlers.erase(it);
		return ::Steinberg::kResultOk;
	}
	return ::Steinberg::kResultFalse;
}

::Steinberg::tresult PLUGIN_API Vst3PluginView::registerTimer(::Steinberg::Linux::ITimerHandler* handler,
	::Steinberg::Linux::TimerInterval milliseconds)
{
	const auto [it, success] = m_timers.try_emplace(handler, handler, milliseconds);
	return success ? ::Steinberg::kResultOk : ::Steinberg::kResultFalse;
}

::Steinberg::tresult PLUGIN_API Vst3PluginView::unregisterTimer(::Steinberg::Linux::ITimerHandler* handler)
{
	if (const auto it = m_timers.find(handler); it != m_timers.end())
	{
		m_timers.erase(it);
		return ::Steinberg::kResultOk;
	}
	return ::Steinberg::kResultFalse;
}

#endif // LMMS_BUILD_LINUX

void Vst3PluginView::focusInEvent(QFocusEvent*)
{
	if (m_view) { m_view->onFocus(true); }
}

void Vst3PluginView::focusOutEvent(QFocusEvent*)
{
	if (m_view) { m_view->onFocus(false); }
}

void Vst3PluginView::keyPressEvent(QKeyEvent* event)
{
	if (m_view)
	{
		const auto [key, keyCode, modifiers] = translateKeyEvent(event);
		if (m_view->onKeyDown(key, keyCode, modifiers) == ::Steinberg::kResultTrue)
		{
			event->accept();
			return;
		}
	}
	Base::keyPressEvent(event);
}

void Vst3PluginView::keyReleaseEvent(QKeyEvent* event)
{
	if (m_view)
	{
		const auto [key, keyCode, modifiers] = translateKeyEvent(event);
		if (m_view->onKeyUp(key, keyCode, modifiers) == ::Steinberg::kResultTrue)
		{
			event->accept();
			return;
		}
	}
	Base::keyReleaseEvent(event);
}

void Vst3PluginView::wheelEvent(QWheelEvent* event)
{
	if (m_view && m_view->onWheel(event->angleDelta().x() / 120.f) == ::Steinberg::kResultTrue)
	{
		event->accept();
		return;
	}
	Base::wheelEvent(event);
}

} // namespace lmms::gui
