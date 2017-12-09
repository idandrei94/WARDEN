#pragma once

#include <stdint.h>
#include "Arduino.h"

template <class T > 
class List {
private:
	T *elements = nullptr;
	uint8_t count = 0;
	uint8_t array_capacity;
	uint8_t first = 0;
public:
	/** Constructor
	* @param capacity		Initial capacity for the array
	*/
	List(uint8_t capacity);

	~List();

	/** Check the size of the list
	* @return				The number of elements stored within the list
	*/
	uint8_t size();

	/** Check the current capacity of the array
	* @return				The current size of the array 
	*/
	uint8_t capacity();

	/** Insert an element at the end of the list
	* @param element		A pointer to an element to be added
	* @pre					The argument must not be nullptr
	* @post					The array size might be increased to accomodate the new element if it was full already
	* @return				Returns the new element count
	*/
	uint8_t push_back(T element);

	/** Insert an element at the beginning of the list
	* @param element		A pointer to an element to be added
	* @pre					The argument must not be nullptr
	* @post					The array size might be increased to accomodate the new element if it was full already
	* @return				Returns the new element count
	*/
	uint8_t push_front(T element);

	/** Remove and return the first element of the list
	* @post					The first element of the list will be removed
	* @return				Returns the removed element or nullptr if unable to remove
	*/
	T pop_front();

	/** Remove and return the last element of the list
	* @post					The last element of the list will be removed
	* @return				Returns the removed element or nullptr if unable to remove
	*/
	T pop_back();

	/** Checks whether an element already exists in the list
	* @return				Returns the index of the element in the list if found, -1 otherwise
	*/
	uint8_t contains(const T element);

	/** Get the element at the given index
	* @return				Returns the element on that given index or nullptr otherwise
	*/
	T get(uint8_t index);

	/* Double the size of the array
	* @pre					Array size must not be 0
	* @post					The new array will contain all the previous elements, starting with index 0 in the array
	*/
	void expand_capacity();
};

template <class T >
void List<T>::expand_capacity() {
		T *newArr = new T[capacity * 2];
		for (uint8_t i = 0; i < capacity; ++i) {
			newArr[i] = elements[(first + i) % capacity];
		}
		first = 0;
		delete[] elements;
		elements = newArr;
		capacity *= 2;
	}

template <class T >
List<T>::List(uint8_t cap) : array_capacity(cap) {
		elements = new T[array_capacity];
}

template <class T >
List<T>::~List() {
		delete[] elements;
	}

template <class T >
uint8_t List<T>::size() {
		return count;
	}

template <class T >
uint8_t List<T>::capacity() {
		return array_capacity;
	}

template <class T >
uint8_t List<T>::push_back(T element) {
	if (count == array_capacity) {
		return -1;
	}
	elements[(first + count) % array_capacity] = element;
	++count;
	return count;
}

template <class T >
uint8_t List<T>::push_front(T element) {
	if (count == array_capacity) {
		return -1;
	}
	first = first == 0 ? (array_capacity - 1) : (first - 1);
	elements[first] = element;
	++count;
	return count;
}

template <class T >
T List<T>::pop_front() {
		if (count == 0) {
			return nullptr;
		}
		uint8_t old_first = first;
		first = count == 1 ? 0 : (first + 1) % array_capacity;
		--count;
		return elements[old_first];
	}

template <class T >
T List<T>::pop_back() {
		if (size() == 0) {
			return nullptr;
		}
		--count;
		return elements[(first + count) % array_capacity];
	}

template <class T >
uint8_t List<T>::contains(const T element) {
		for (uint8_t i = 0; i < size; ++i) {
			if (*(elements[(first+i)%array_capacity]) == *element) {
				return i;
			}
		}
		return -1;
	}

template <class T >
T List<T>::get(uint8_t index) {
	if (index < 0 || index >= count) {
		return nullptr;
	}
	return elements[(first + index) % array_capacity];
}