/*
 * Vst3Manager.cpp - discovery and loading of VST3 plugins
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

#include "Vst3Manager.h"

#include <array>
#include <cstring>
#include <type_traits>
#include <utility>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>

#include "lmmsconfig.h"

#ifdef LMMS_BUILD_LINUX
#	include <dlfcn.h>
#endif

#ifdef LMMS_BUILD_WIN32
#	include <windows.h>
#	include <atlbase.h>
#	include <knownfolders.h>
#	include <shlobj.h>
#endif

#ifdef LMMS_BUILD_APPLE
#	include <QByteArray>
#
#	include <CoreFoundation/CoreFoundation.h>
#endif

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"

#include "impl/HostApplication.h"
#include "Vst3Core.h"

namespace lmms
{

#ifdef LMMS_BUILD_APPLE
	template<typename T>
	struct CFReleaser
	{
		using pointer = T;
		void operator()(T ref) const { CFRelease(ref); }
	};

	template<typename T>
	using cf_ptr = std::unique_ptr<T, CFReleaser<T>>;
#endif

#if defined(LMMS_BUILD_LINUX)
	using EntryFunc = bool (PLUGIN_API *)(void *);
	using ExitFunc = bool (PLUGIN_API *)();
	constexpr auto entryFuncName = "ModuleEntry";
	constexpr auto exitFuncName = "ModuleExit";
#elif defined(LMMS_BUILD_WIN32)
	using EntryFunc = bool (PLUGIN_API *)();
	using ExitFunc = bool (PLUGIN_API *)();
	constexpr auto entryFuncName = "InitDll";
	constexpr auto exitFuncName = "ExitDll";
#elif defined(LMMS_BUILD_APPLE)
	using EntryFunc = bool (PLUGIN_API *)(CFBundleRef);
	using ExitFunc = bool (PLUGIN_API *)();
	constexpr auto entryFuncName = "bundleEntry";
	constexpr auto exitFuncName = "bundleExit";
#else
#	error LMMS does not currently support VST3 on this platform
#endif

#if defined(LMMS_BUILD_LINUX)
	constexpr auto binaryExtension = "so";
#	if defined(LMMS_HOST_X86_64)
		constexpr auto platformName = "x86_64-linux";
#	elif defined(LMMS_HOST_X86)
		constexpr auto platformName = "i686-linux";
#	else
#		error LMMS does not currently support VST3 on this architecture
#	endif
#elif defined(LMMS_BUILD_WIN32)
	constexpr auto binaryExtension = "vst3";
#	if defined(LMMS_HOST_X86_64)
		constexpr auto platformName = "x86_64-win";
#	elif defined(LMMS_HOST_X86)
		constexpr auto platformName = "x86-win";
#	else
#		error LMMS does not currently support VST3 on this architecture
#	endif
// No case for macOS - QLibrary can open the bundle directly
#endif

static QString fuidToString(const ::Steinberg::FUID &fuid)
{
	auto string = std::array<char, 33>{0};
	fuid.toString(string.data());
	Q_ASSERT_X(string.back() == 0, __func__, "buffer overflow");
	return QString::fromUtf8(string.data());
}

Vst3Module::Vst3Module(const QString &path) :
	m_library{path},
	m_path{path}
{
	qDebug() << "Attempting to load module" << path;
	if (!m_library.load()) { return; }

	const auto entryFunc = reinterpret_cast<EntryFunc>(m_library.resolve(entryFuncName));
#if defined(LMMS_BUILD_LINUX)
	// Open the library a second time manually because QLibrary doesn't expose
	// its internal handle
	const auto handle = dlopen(QFile::encodeName(path), RTLD_LAZY | RTLD_NOLOAD);
	if (!entryFunc || !handle || !entryFunc(handle)) { return; }
#elif defined(LMMS_BUILD_WIN32)
	if (entryFunc && !entryFunc()) { return; }
#elif defined(LMMS_BUILD_APPLE)
	// Create the bundle a second time manually because QLibrary doesn't expose
	// its internal handle
	const auto pathUtf8 = path.toUtf8();
	const auto url = cf_ptr<CFURLRef>{CFURLCreateFromFileSystemRepresentation(nullptr,
		reinterpret_cast<const UInt8 *>(pathUtf8.data()), pathUtf8.length(), true)};
	const auto bundle = cf_ptr<CFBundleRef>{CFBundleCreate(nullptr, url.get())};
	if(!entryFunc || !bundle || !entryFunc(bundle.get())) { return; }
#endif

	const auto getPluginFactory = reinterpret_cast<GetFactoryProc>(m_library.resolve("GetPluginFactory"));
	if (!getPluginFactory) { return; }

	m_factory = ::Steinberg::owned(getPluginFactory());
	if (!m_factory) { return; }

	if (auto info = ::Steinberg::PFactoryInfo{}; m_factory->getFactoryInfo(&info) == ::Steinberg::kResultOk)
	{
		m_vendor = info.vendor;
		m_url = info.url;
		m_email = info.email;
	}
}

Vst3Module::~Vst3Module()
{
	m_factory.reset();
	if (const auto exitFunc = reinterpret_cast<ExitFunc>(m_library.resolve(exitFuncName))) { exitFunc(); }
	m_library.unload();
}

std::vector<Vst3PluginInfo> Vst3Module::exportedPlugins() const
{
	auto plugins = std::vector<Vst3PluginInfo>{};
	const auto numClasses = m_factory->countClasses();
	plugins.reserve(numClasses);

	const auto addPlugin = [&plugins, this](const auto &classInfo)
	{
		using ClassType = std::remove_cv_t<std::remove_reference_t<decltype(classInfo)>>;
		if (strcmp(classInfo.category, kVstAudioEffectClass) != 0) { return; }
		const auto uuid = ::Steinberg::FUID::fromTUID(classInfo.cid);
		const auto name = toQString(classInfo.name);
		auto vendor = QString{};
		auto version = QString{};
		int flags;
		if constexpr (std::is_same_v<ClassType, ::Steinberg::PClassInfo2>
		           || std::is_same_v<ClassType, ::Steinberg::PClassInfoW>)
		{
			vendor = toQString(classInfo.vendor);
			version = toQString(classInfo.version);
			const auto categories = toQString(classInfo.subCategories).split("|");
			flags = (categories.contains("Instrument") ? Vst3PluginInfo::Instrument : 0)
				  | (categories.contains("Fx") ? Vst3PluginInfo::Effect : 0);
		}
		else
		{
			// The PClassInfo struct doesn't provide any information to allow
			// instruments and effects to be distinguished prior to
			// instantiating the plugin, so expose the plugin as both.
			flags = Vst3PluginInfo::Instrument | Vst3PluginInfo::Effect;
		}
		plugins.emplace_back(uuid, m_path, name, vendor.isEmpty() ? m_vendor : vendor, version, m_url, m_email, flags);
	};

	const auto pf2 = interface_cast<::Steinberg::IPluginFactory2>(m_factory);
	const auto pf3 = interface_cast<::Steinberg::IPluginFactory3>(m_factory);
	if (pf3) { pf3->setHostContext(HostApplication::instance()->asFUnknown()); }
	auto info1 = ::Steinberg::PClassInfo{};
	auto info2 = ::Steinberg::PClassInfo2{};
	auto info3 = ::Steinberg::PClassInfoW{};
	for (int i = 0; i < numClasses; ++i)
	{
		if (pf3 && pf3->getClassInfoUnicode(i, &info3) == ::Steinberg::kResultOk) { addPlugin(info3); }
		else if (pf2 && pf2->getClassInfo2(i, &info2) == ::Steinberg::kResultOk) { addPlugin(info2); }
		else if (m_factory->getClassInfo(i, &info1) == ::Steinberg::kResultOk) { addPlugin(info1); }
	}
	return plugins;
}

Vst3Manager::Vst3Manager()
{
	for (const auto &dir : pluginDirectories())
	{
		addPluginsFromDir(dir);
	}
}

const Vst3PluginInfo* Vst3Manager::pluginInfoForUuid(const QString& uuid) const
{
	const auto it = m_plugins.find(uuid);
	return it == m_plugins.end() ? nullptr : &it->second;
}

const Vst3Module* Vst3Manager::getModule(const QString& path)
{
	if (const auto it = m_modules.find(path); it != m_modules.end()) { return it->second.get(); }
	auto module = std::make_unique<Vst3Module>(path);
	if (!module->isValid()) { return nullptr; }
	const auto [result, success] = m_modules.try_emplace(path, std::move(module));
	Q_ASSERT(success); // the module wasn't already in the map, so insertion should succeed
	return result->second.get();
}

QStringList Vst3Manager::pluginDirectories()
{
	QStringList dirs;
#if defined(LMMS_BUILD_LINUX)
	dirs << (QDir::homePath() + "/.vst3")
		<< "/usr/lib/vst3"
		<< "/usr/local/lib/vst3";
#elif defined(LMMS_BUILD_WIN32)
	if (auto path = CComHeapPtr<WCHAR>{}; SHGetKnownFolderPath(FOLDERID_ProgramFilesCommon, 0, nullptr, &path) == S_OK)
	{
		dirs << (QDir::fromNativeSeparators(QString::fromWCharArray(path)) + "/VST3");
	}
#elif defined(LMMS_BUILD_APPLE)
	dirs << (QDir::homePath() + "/Library/Audio/Plug-ins/VST3")
		<< "/Library/Audio/Plug-ins/VST3"
		<< "/Network/Library/Audio/Plug-ins/VST3";
#endif
	return dirs;
}

void Vst3Manager::addPluginsFromDir(const QString& path)
{
	auto bundles = QDirIterator(path, QStringList("*.vst3"), QDir::Dirs,
		QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
	while (bundles.hasNext())
	{
		addPluginBundle(bundles.next());
	}
#ifdef LMMS_BUILD_WIN32
	// Previous versions of VST3 used single files instead of bundles on Windows
	auto libraries = QDirIterator(path, QStringList("*.vst3"), QDir::Files, QDirIterator::Subdirectories);
	while (libraries.hasNext())
	{
		const auto library = libraries.next();
		if (!library.contains(".vst3/", Qt::CaseInsensitive)) // Don't re-add libraries from bundles
		{
			addPluginLibrary(library);
		}
	}
#endif
}

void Vst3Manager::addPluginBundle(const QString& path)
{
#ifdef LMMS_BUILD_APPLE
	// On macOS, plugin bundles can be loaded directly as libraries
	addPluginLibrary(path);
#else
	auto pluginName = QDir{path}.dirName();
	pluginName.chop(5); // Remove .vst3 extension
	const auto binaryPath = QString{"%1/Contents/%2/%3.%4"}.arg(path, platformName, pluginName, binaryExtension);
	if (QFile::exists(binaryPath))
	{
		addPluginLibrary(binaryPath);
	}
	else
	{
		qWarning() << "Could not find appropriate binary in bundle:" << path;
	}
#endif
}

void Vst3Manager::addPluginLibrary(const QString& path)
{
	const auto module = getModule(path);
	if (!module) { return; }
	for (auto& info : module->exportedPlugins())
	{
		auto uuid = fuidToString(info.uuid());
		if (const auto [it, success] = m_plugins.try_emplace(std::move(uuid), std::move(info)); !success)
		{
			qWarning() << "Duplicate VST3 plugin UUID:" << uuid;
		}
	}
}

std::unique_ptr<Vst3Manager> Vst3Manager::s_instance;

} // namespace lmms
