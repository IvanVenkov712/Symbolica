#pragma once
#pragma once
#include <stdexcept>

template <typename T>
class poly_obj_handler {

	T* ptr = nullptr;

	void free();

	void copyFrom(const poly_obj_handler<T>& other);

	void moveFrom(poly_obj_handler<T>&& other);

public:
	poly_obj_handler() = default;

	poly_obj_handler(T* ptr);

	poly_obj_handler(const T& ref);

	poly_obj_handler(const poly_obj_handler<T>& other);

	poly_obj_handler<T>& operator=(const poly_obj_handler<T>& other);

	poly_obj_handler(poly_obj_handler<T>&& other) noexcept;

	poly_obj_handler<T>& operator=(poly_obj_handler<T>&& other) noexcept;

	~poly_obj_handler();

	operator bool() const;

	bool operator!() const;

	T& operator*();

	const T& operator*() const;

	T* operator->();

	const T* operator->() const;

	T* getPtr();

	const T* getPtr() const;

	void reset(T* newPtr);

	void reset(const T& newRef);
};

template<typename T>
bool operator==(const poly_obj_handler<T>& lhs, const poly_obj_handler<T>& rhs);

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