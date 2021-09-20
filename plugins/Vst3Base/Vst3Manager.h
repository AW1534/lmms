/*
 * Vst3Manager.h - discovery and loading of VST3 plugins
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

#ifndef VST3_MANAGER_H
#define VST3_MANAGER_H

#include <map>
#include <memory>
#include <vector>

#include <QLibrary>
#include <QString>
#include <QStringList>

#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/base/smartpointer.h"

namespace lmms
{

//! Represents a single VST3 plugin. A VST3 plugin is identified by its `uuid`,
//! and has a number of properties which may or may not be empty, some of which
//! are not present in older versions of the API. Each VST3 plugin may be either
//! an instrument, an effect, or both (the latter is the case for older versions
//! of the API, which do not expose the plugin type, or for later versions where
//! the plugin is explicitly marked as such).
class Vst3PluginInfo
{
public:
	enum Flags
	{
		Instrument = 1 << 0,
		Effect = 1 << 1,
	};

	Vst3PluginInfo() = default;
	Vst3PluginInfo(const ::Steinberg::FUID& uuid, const QString& module,
			const QString& name, const QString& vendor, const QString& version,
			const QString& url, const QString& email, int flags) :
		m_uuid{uuid}, m_module{module}, m_name{name}, m_vendor{vendor},
		m_version{version}, m_url{url}, m_email{email}, m_flags{flags}
	{ }

	const ::Steinberg::FUID& uuid() const { return m_uuid; }
	const QString& module() const { return m_module; }
	const QString& name() const { return m_name; }
	const QString& vendor() const { return m_vendor; }
	const QString& version() const { return m_version; }
	const QString& url() const { return m_url; }
	const QString& email() const { return m_email; }
	bool isInstrument() const { return m_flags & Instrument; }
	bool isEffect() const { return m_flags & Effect; }

private:
	::Steinberg::FUID m_uuid;
	QString m_module;
	QString m_name;
	QString m_vendor;
	QString m_version;
	QString m_url;
	QString m_email;
	int m_flags = 0;
};

//! Holds a VST3 plugin library, manages initialization and deinitialization of
//! the library, and handles exposure and instantiation of the plugins within.
class Vst3Module
{
public:
	explicit Vst3Module(const QString& path);
	~Vst3Module();

	Vst3Module(const Vst3Module&) = delete;
	Vst3Module& operator=(const Vst3Module&) = delete;

	//! Tests if this module is a valid VST3 library. This entails that it was
	//! successfully initialized, and exports a plugin factory.
	bool isValid() const { return m_factory.get() != nullptr; }

	//! Lists the plugins in this module that are compatible with LMMS (as far
	//! as can be determined without instantiating them).
	std::vector<Vst3PluginInfo> exportedPlugins() const;

	//! Creates an instance of the class with UUID `fuid`, returned via the
	//! interface `T`. Returns a null `Steinberg::IPtr<T>` upon failure.
	template<typename T>
	::Steinberg::IPtr<T> createInstance(const ::Steinberg::FUID& fuid) const
	{
		if (void* instance = nullptr; m_factory->createInstance(fuid, T::iid, &instance) == ::Steinberg::kResultOk)
		{
			return ::Steinberg::owned(static_cast<T*>(instance));
		}
		return nullptr;
	}

private:
	QLibrary m_library;
	::Steinberg::IPtr<::Steinberg::IPluginFactory> m_factory;
	// We have to store the path ourselves, and not rely on QLibrary::fileName,
	// since the latter can differ from the path given to the constructor.
	// For example, on macOS we provide the path to the plugin bundle, but the
	// library's `fileName` is that of the binary within the bundle.
	QString m_path;
	QString m_vendor;
	QString m_url;
	QString m_email;
};

//! Discovers and loads VST3 plugin libraries on the system, and maintains a
//! list of all LMMS-compatible plugins within them.
class Vst3Manager
{
public:
	Vst3Manager();

	//! Returns an iterator to the first UUID-plugin pair of the manager. 
	auto begin() const { return m_plugins.begin(); }
	//! Returns a past-the-end iterator to the manager's plugins.
	auto end() const { return m_plugins.end(); }

	//! Finds the plugin info for the plugin with UUID `uuid`, or returns
	//! `nullptr` if no such plugin can be found.
	const Vst3PluginInfo* pluginInfoForUuid(const QString& uuid) const;

	//! Returns the VST3 module at the given location, loading it if necessary,
	//! or returns `nullptr` if there is no such valid module.
	const Vst3Module* getModule(const QString& path);

	//! Creates the `Vst3Manager` singleton, scanning for plugins in the
	//! process. The singleton must not yet be initialized.
	static void initialize()
	{
		Q_ASSERT_X(!s_instance, __func__, "instance is already initialized");
		s_instance = std::make_unique<Vst3Manager>();
	}

	//! Frees the `Vst3Manager` singleton, releasing all plugin libraries in the
	//! process.
	static void deinitialize() { s_instance.reset(); }

	//! Gets the `Vst3Manager` singleton instance. Must not be called before
	//! `initialize` is called, or after `deinitialize` is called.
	static Vst3Manager& instance()
	{
		Q_ASSERT_X(s_instance.get() != nullptr, __func__, "instance is not initialized");
		return *s_instance;
	}

private:
	//! Returns a list of directories where VST3 plugins can be found on the
	//! current system.
	static QStringList pluginDirectories();

	//! Scans the given directory recursively for VST3 plugins and adds them all
	//! to the list of discovered VST3 plugins.
	void addPluginsFromDir(const QString& path);

	//! Adds all the VST3 plugins contained within the given bundle to the list
	//! of discovered VST3 plugins.
	void addPluginBundle(const QString& path);

	//! Adds all the VST3 plugins contained within the given library to the list
	//! of discovered VST3 plugins.
	void addPluginLibrary(const QString& path);

	std::map<QString, Vst3PluginInfo> m_plugins;
	std::map<QString, std::unique_ptr<Vst3Module>> m_modules;

	static std::unique_ptr<Vst3Manager> s_instance;
};

} // namespace lmms

#endif // VST3_MANAGER_H
