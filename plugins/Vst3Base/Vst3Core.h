/*
 * Vst3Core.h - VST3 base classes and helper functions
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

#ifndef VST3_BASE_H
#define VST3_BASE_H

#include <atomic>
#include <type_traits>
#include <utility>

#include <QString>

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/smartpointer.h"

namespace lmms
{

inline QString toQString(const ::Steinberg::char8* str)
{
	return QString::fromUtf8(str);
}

inline QString toQString(const ::Steinberg::char16* str)
{
	return QString::fromUtf16(reinterpret_cast<const unsigned short*>(str));
}

//! Tests whether `iid` is the UUID of interface `T`.
template<typename T>
bool isUuidOf(const ::Steinberg::TUID iid)
{
	return ::Steinberg::FUnknownPrivate::iidEqual(T::iid, iid);
}

//! Like `dynamic_cast` but for `queryInterface`. If `obj` is not `nullptr` and
//! implements the interface `T`, returns a pointer to that interface of `obj`
//! wrapped in a `Steinberg::IPtr`, otherwise returns a null `Steinberg::IPtr`.
template<typename T>
::Steinberg::IPtr<T> interface_cast(::Steinberg::FUnknown* obj)
{
	if (void* result = nullptr; obj && obj->queryInterface(T::iid, &result) == ::Steinberg::kResultOk)
	{
		return ::Steinberg::owned(static_cast<T*>(result));
	}
	return nullptr;
}

namespace detail
{

//! Lock-free reference count implemented using atomics.
class AtomicRefCount
{
public:
	AtomicRefCount(::Steinberg::uint32 init) : m_value{init} { }

	// Copying or moving reference-counted objects should not copy the reference
	// count; new reference counts should always start at one, and existing ones
	// should not be modified.
	AtomicRefCount(const AtomicRefCount&) noexcept { }
	AtomicRefCount(AtomicRefCount&&) noexcept { }

	AtomicRefCount& operator=(const AtomicRefCount&) noexcept { return *this; }
	AtomicRefCount& operator=(AtomicRefCount&&) noexcept { return *this; }

	::Steinberg::uint32 operator++()
	{
		return m_value.fetch_add(1, std::memory_order_relaxed) + 1;
	}

	::Steinberg::uint32 operator--()
	{
		const auto refcount = m_value.fetch_sub(1, std::memory_order_release) - 1;
		if (refcount == 0)
		{
			// Reference count reached zero, so whoever is using us probably
			// wants to delete themselves now. Ensure all modifications on other
			// threads happen-before the destructor call.
			std::atomic_thread_fence(std::memory_order_acquire);
		}
		return refcount;
	}

	operator ::Steinberg::uint32() const
	{
		return m_value.load(std::memory_order_relaxed);
	}

private:
	std::atomic<::Steinberg::uint32> m_value;
	static_assert(decltype(m_value)::is_always_lock_free, "atomic refcount is not always lock free");
};

class FixedRefCount
{
	static constexpr ::Steinberg::uint32 fakeRefCount = 100u;

public:
	FixedRefCount(::Steinberg::uint32) { }

	::Steinberg::uint32 operator++() { return fakeRefCount; }
	::Steinberg::uint32 operator--() { return fakeRefCount; }
	operator ::Steinberg::uint32() const { return fakeRefCount; }
};

template<typename T, typename... Rest>
using First = T;

} // namespace detail

class Vst3Object : public ::Steinberg::FUnknown { };

//! Inherits from `Interfaces...` and provides an appropriate implementation of
//! `queryInterface`. The resulting class is still abstract as it lacks any
//! reference counting methods. See `RefCounted`, `DelegateRefCount`, and
//! `Uncounted` for implementations of these.
template<typename... Interfaces>
class Implements : public Vst3Object, public Interfaces...
{
	static_assert((std::is_base_of_v<::Steinberg::FUnknown, Interfaces> || ...),
		"All interfaces must derive from Steinberg::FUnknown");

public:
	// Redeclared here to avoid ambiguity when calling without a refcount
	// implementation yet provided.
	::Steinberg::uint32 PLUGIN_API addRef() override = 0;
	::Steinberg::uint32 PLUGIN_API release() override = 0;

	//! Cast this object to `Steinberg::FUnknown`.
	//! Useful since this cast is often ambiguous.
	::Steinberg::FUnknown* asFUnknown() { return static_cast<detail::First<Interfaces...>*>(this); }

	::Steinberg::tresult PLUGIN_API queryInterface(const ::Steinberg::TUID iid, void** obj) override
	{
		if (!iid || !obj) [[unlikely]] { return ::Steinberg::kInvalidArgument; }
		if ((querySingleInterface<Interfaces>(iid, obj) || ...
			|| querySingleInterface<::Steinberg::FUnknown>(iid, obj)))
		{
			return ::Steinberg::kResultOk;
		}
		*obj = nullptr;
		return ::Steinberg::kNoInterface;
	}

private:
	template<typename Interface>
	bool querySingleInterface(const ::Steinberg::TUID iid, void** obj)
	{
		if (isUuidOf<Interface>(iid))
		{
			addRef();
			if constexpr (std::is_same_v<Interface, ::Steinberg::FUnknown>) { *obj = asFUnknown(); }
			else { *obj = static_cast<Interface*>(this); }
			return true;
		}
		return false;
	}
};

template<typename Object, typename Counter>
class CountedObject : public Object
{
public:
	using Object::Object;

	::Steinberg::uint32 PLUGIN_API addRef() override { return ++m_refCount; }

	::Steinberg::uint32 PLUGIN_API release() override
	{
		if (const auto refCount = --m_refCount; refCount != 0) { return refCount; }
		delete this;
		return 0;
	}

	::Steinberg::uint32 refCount() const { return m_refCount; }

private:
	[[no_unique_address]] Counter m_refCount = 1u;
};

template<typename T>
using RefCounted = CountedObject<T, detail::AtomicRefCount>;

template<typename T>
using Uncounted = CountedObject<T, detail::FixedRefCount>;

//! Creates a reference-counted instance of `T` and wraps it in a
//! `Steinberg::IPtr`.
template<typename T, typename Interface = T, typename... Args>
::Steinberg::IPtr<Interface> make_refcounted(Args&&... args)
{
	return ::Steinberg::owned<Interface>(new RefCounted<T>(std::forward<Args>(args)...));
}

namespace detail
{

//! Reference count that delegates to the reference count of another object.
class DelegatedRefCount
{
public:
	DelegatedRefCount(::Steinberg::FUnknown* delegate) noexcept : m_delegate{delegate} { }
	// Can't copy or move construct as this could leave `m_delegate` pointing
	// to the wrong object. Instead the delegate must be explicitly specified
	// for each new instance.
	DelegatedRefCount(const DelegatedRefCount&) = delete;
	DelegatedRefCount(DelegatedRefCount&&) = delete;

	// Copying or moving shouldn't change who provides our reference count.
	DelegatedRefCount& operator=(const DelegatedRefCount&) noexcept { return *this; }
	DelegatedRefCount& operator=(DelegatedRefCount&&) noexcept { return *this; }

	::Steinberg::uint32 operator++() { return m_delegate->addRef(); }
	::Steinberg::uint32 operator--() { return m_delegate->release(); }
	operator ::Steinberg::uint32() const { return 3; }

private:
	::Steinberg::FUnknown *m_delegate;
};

} // namespace detail

//! Defines reference-counting methods on `T` that forward to a delegate object
//! provided to the constructor. Use, for example, for member objects to which
//! pointers are handed out. This enables any code using these member objects
//! to keep the containing class alive until it is no longer needed.
//! The containing class must not hold objects using this mixin in a
//! `Steinberg::IPtr`: the object is not capable of freeing itself, and such a
//! construct may lead to the containing class holding a reference to itself.
//! The object must be deleted along with the containing class, so instead hold
//! it directly as a member, or in a `std::unique_ptr`.
//! See `make_delegaterefcount`.
template<typename T>
class DelegateRefCount : public T
{
public:
	template<typename... Args>
	explicit DelegateRefCount(Args&&... args, Vst3Object* delegate)
		noexcept(std::is_nothrow_constructible_v<T, Args...>) :
		T(std::forward<Args>(args)...),
		m_refCount{delegate}
	{ }
	DelegateRefCount(const DelegateRefCount&) = delete;

	::Steinberg::uint32 PLUGIN_API addRef() override { return ++m_refCount; }
	::Steinberg::uint32 PLUGIN_API release() override { return --m_refCount; }
	::Steinberg::uint32 refCount() const { return m_refCount; }

private:
	detail::DelegatedRefCount m_refCount;
};

} // namespace lmms

#endif // VST3_BASE_H
