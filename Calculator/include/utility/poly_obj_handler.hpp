#pragma once
#pragma once
/** @file poly_obj_handler.hpp
 * @brief Exclusive polymorphic ownership with clone-based value semantics.
 */
#include <stdexcept>

/** @brief Nullable owner that deep-copies T through its virtual clone() method.
 * @tparam T A polymorphic type with clone() returning a newly allocated T* and a
 * virtual destructor when derived objects are stored through a base pointer.
 *
 * Raw pointers are adopted, never borrowed. Copies allocate independent objects;
 * moves leave the source empty. Dereference/member access on empty handlers throws
 * std::logic_error. Pointer comparison tests identity, not structural equality.
 * @warning Copy assignment and reset lack exception safety if clone() throws.
 * reset arguments must not alias the currently owned object. See known-issues.md.
 */
template <typename T>
class poly_obj_handler {

	T* ptr = nullptr;

	void free();

	void copyFrom(const poly_obj_handler<T>& other);

	void moveFrom(poly_obj_handler<T>&& other);

public:
	/// Construct an empty owner.
	poly_obj_handler() = default;

	/// Adopt ptr, which must be null or uniquely owned and deletable through T*.
	poly_obj_handler(T* ptr);

	/// Clone ref; ownership of ref itself remains with its caller.
	poly_obj_handler(const T& ref);

	/// Deep-copy the owned object, preserving an empty source as empty.
	poly_obj_handler(const poly_obj_handler<T>& other);

	/// Delete the old object, then clone other; clone failure can leave a dangling pointer.
	poly_obj_handler<T>& operator=(const poly_obj_handler<T>& other);

	/// Transfer ownership without cloning; leave other empty.
	poly_obj_handler(poly_obj_handler<T>&& other) noexcept;

	/// Delete the old object and transfer ownership; self-move leaves it unchanged.
	poly_obj_handler<T>& operator=(poly_obj_handler<T>&& other) noexcept;

	/// Delete the owned object, if any.
	~poly_obj_handler();

	/// Whether the handler owns a non-null pointer.
	operator bool() const;

	/// Whether the handler is empty.
	bool operator!() const;

	/// Access the object; throw std::logic_error when empty.
	T& operator*();

	/// Read the object; throw std::logic_error when empty.
	const T& operator*() const;

	/// Access members; throw std::logic_error when empty.
	T* operator->();

	/// Read members; throw std::logic_error when empty.
	const T* operator->() const;

	/// Borrow the pointer, possibly null; ownership stays with this handler.
	T* getPtr();

	/// Borrow a const pointer, possibly null; ownership stays with this handler.
	const T* getPtr() const;

	/// Delete the old object and adopt newPtr; newPtr must not equal getPtr().
	void reset(T* newPtr);

	/// Delete the old object then clone newRef; newRef must not refer to the old object.
	void reset(const T& newRef);
};

/// Compare owned pointer identity, including equality of two empty handlers.
template<typename T>
bool operator==(const poly_obj_handler<T>& lhs, const poly_obj_handler<T>& rhs);

/// Compare pointer inequality; this does not compare expression content.
template<typename T>
bool operator!=(const poly_obj_handler<T>& lhs, const poly_obj_handler<T>& rhs);

template<typename T>
void poly_obj_handler<T>::free()
{
	delete ptr;
	ptr = nullptr;
}

template<typename T>
void poly_obj_handler<T>::copyFrom(const poly_obj_handler<T>& other)
{
	if (other.ptr != nullptr) {
		ptr = other.ptr->clone();
	}
	else {
		ptr = nullptr;
	}
}

template<typename T>
void poly_obj_handler<T>::moveFrom(poly_obj_handler<T>&& other)
{
	ptr = other.ptr;
	other.ptr = nullptr;
}

template<typename T>
poly_obj_handler<T>::poly_obj_handler(T* ptr) : ptr(ptr) {}

template<typename T>
poly_obj_handler<T>::poly_obj_handler(const T& ref) : ptr(ref.clone()) {}

template<typename T>
poly_obj_handler<T>::poly_obj_handler(const poly_obj_handler<T>& other)
{
	copyFrom(other);
}

template<typename T>
poly_obj_handler<T>& poly_obj_handler<T>::operator=(const poly_obj_handler<T>& other)
{
	if (this != &other) {
		free();
		copyFrom(other);
	}
	return *this;
}

template<typename T>
poly_obj_handler<T>::poly_obj_handler(poly_obj_handler<T>&& other) noexcept
{
	moveFrom(std::move(other));
}

template<typename T>
poly_obj_handler<T>& poly_obj_handler<T>::operator=(poly_obj_handler<T>&& other) noexcept
{
	if (this != &other) {
		free();
		moveFrom(std::move(other));
	}
	return *this;
}

template<typename T>
poly_obj_handler<T>::~poly_obj_handler()
{
	free();
}

template<typename T>
poly_obj_handler<T>::operator bool() const
{
	return ptr != nullptr;
}

template<typename T>
bool poly_obj_handler<T>::operator!() const
{
	return ptr == nullptr;
}

template<typename T>
T& poly_obj_handler<T>::operator*()
{
	if (ptr == nullptr) {
		throw std::logic_error("trying to dereference nullptr!");
	}
	return *ptr;
}

template<typename T>
const T& poly_obj_handler<T>::operator*() const
{
	if (ptr == nullptr) {
		throw std::logic_error("trying to dereference nullptr!");
	}
	return *ptr;
}

template<typename T>
T* poly_obj_handler<T>::operator->()
{
	if (ptr == nullptr) {
		throw std::logic_error("trying to dereference nullptr!");
	}
	return ptr;
}

template<typename T>
const T* poly_obj_handler<T>::operator->() const
{
	if (ptr == nullptr) {
		throw std::logic_error("trying to dereference nullptr!");
	}
	return ptr;
}

template<typename T>
T* poly_obj_handler<T>::getPtr()
{
	return ptr;
}

template<typename T>
const T* poly_obj_handler<T>::getPtr() const
{
	return ptr;
}

template<typename T>
void poly_obj_handler<T>::reset(T* newPtr)
{
	free();
	ptr = newPtr;
}

template<typename T>
void poly_obj_handler<T>::reset(const T& newRef)
{
	free();
	ptr = newRef.clone();
}

template<typename T>
bool operator==(const poly_obj_handler<T>& lhs, const poly_obj_handler<T>& rhs)
{
	return lhs.getPtr() == rhs.getPtr();
}

template<typename T>
bool operator!=(const poly_obj_handler<T>& lhs, const poly_obj_handler<T>& rhs)
{
	return lhs.getPtr() != rhs.getPtr();
}
