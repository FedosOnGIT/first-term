//
// Created by fedos on 09.07.2020.
//
#include "buffer.h"
#include <algorithm>
#include <cassert>
#ifndef BIGINT_COPY_VECTOR_H
#define BIGINT_COPY_VECTOR_H

template<typename T>

struct copy_vector {
    copy_vector() {
        memory = new buffer<T>();
    }

    explicit copy_vector(std::vector<T> const& change) {
        memory = new buffer<T>(change);
    }

    copy_vector(T* begin, T* end) {
        memory = new buffer<T>(begin, end);
    }

    copy_vector(copy_vector<T> const& change) noexcept : memory(change.memory) {
        memory->counter++;
    }

    copy_vector(size_t length, const T& element) {
        memory = new buffer<T>(length, element);
    }

    ~copy_vector() {
        remove();
    }

    copy_vector<T>& operator=(copy_vector<T> const& assigned) {
        if (memory != assigned.memory) {
            remove();
            memory = assigned.memory;
            memory->counter++;
        }
        return *this;
    }

    size_t size() {
        return memory->root.size();
    }

    void push_back(T const& element) {
        unshare();
        memory->root.push_back(element);
    }

    void pop_back() {
        unshare();
        memory->root.pop_back();
    }

    T& operator[](size_t index) {
        unshare();
        return memory->root[index];
    }

    const T& operator[](size_t index) const {
        return memory->root[index];
    }

    void reverse() {
        unshare();
        std::reverse(std::begin(memory->root), std::end(memory->root));
    }

private:
    void unshare() {
        if (memory->counter > 1) {
            memory->counter--;
            memory = new buffer<T>(memory->root);
        }
    }

    void remove() {
        memory->counter--;
        if (!memory->counter) {
            delete(memory);
        }
    }
private:
    buffer<T>* memory;
};

#endif //BIGINT_COPY_VECTOR_H
