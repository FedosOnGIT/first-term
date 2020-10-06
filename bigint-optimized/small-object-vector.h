//
// Created by fedos on 09.07.2020.
//
#include "copy-vector.h"

#ifndef BIGINT_SMALL_OBJECT_VECTOR_H
#define BIGINT_SMALL_OBJECT_VECTOR_H

template<typename T>
struct small_object_vector {

    small_object_vector() : will_grow(true), size(0) {}

    small_object_vector(small_object_vector<T> const& other) {
        size = other.size;
        will_grow = other.will_grow;
        if (will_grow) {
            take_small(other);
        } else {
            new (&big)copy_vector<T>(other.big);
        }
    }

    T& operator[](size_t index) {
        if (will_grow) {
            return small[index];
        } else {
            return big[index];
        }
    }

    const T& operator[](size_t index) const {
        if (will_grow) {
            return small[index];
        } else {
            return big[index];
        }
    }

    friend bool operator==(small_object_vector<T> const& first, small_object_vector<T> const& second) {
        if (first.size != second.size) {
            return false;
        }
        if (!first.will_grow && second.will_grow) {
            return first.big == second.big;
        }
        for (size_t i = 0; i < first.size; i++) {
            if (first[i] != second[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(small_object_vector<T> const& first, small_object_vector<T> const& second) {
        return !(first == second);
    }

    small_object_vector<T>& operator=(small_object_vector<T> const& other) {
        will_grow = other.will_grow;
        if (*this != &other) {
            size = other.size;
            if (other.will_grow) {
                take_small();
            } else {
                new(&big)copy_vector<T>(other.big);
            }
        }
        return *this;
    }

    ~small_object_vector<T>() {
        if (will_grow) {
            clear(size);
        } else {
            big.~copy_vector();
        }
    }

    void push_back(T const& element) {
        size++;
        convert_small_to_big();
        if (will_grow) {
            new (small + size - 1)T(element);
        } else {
            big.push_back(element);
        }
    }

    void pop_back() {
        size--;
        if (will_grow) {
            small[size].~T();
        } else {
            big.pop_back();
        }
    }

    size_t capacity() {
        return size;
    }

    bool empty() {
        return size == 0;
    }

    void allocate(size_t length, const T& element) {
        size = length;
        if (will_grow && length <= MAX) {
            for (size_t i = 0; i < size; i++) {
                new (small + i)T(element);
            }
        } else {
            will_grow = false;
            std::vector<T> main(length, element);
            new (&big)copy_vector<T>(main);
        }
    }

    void reverse() {
        if (will_grow) {
            T copy[MAX];
            for (size_t i = 0; i < size; i++) {
                new (copy + size - i - 1)T(small[i]);
            }
            clear(size);
            for (size_t i = 0; i < size; i++) {
                new (small + i)T(copy[i]);
            }
        } else {
            big.reverse();
        }
    }

private:
    void clear(size_t number) {
        for (size_t i = number; i > 0; i--) {
            small[i].~T();
        }
    }

    void convert_small_to_big() {
        if (will_grow && size == MAX) {
            std::vector<T> data(std::begin(small), std::end(small) - 1);
            clear(size);
            new (&big)copy_vector<T>(data);
            will_grow = false;
        }
    }

    void take_small(small_object_vector<T> const& other) {
        size_t position = 0;
        clear(size);
        try {
            for (size_t i = 0; i < other.size; i++) {
                new(small + i)T(other.small[i]);
                position++;
            }
        } catch (...) {
            clear(position);
            throw;
        }
    }

    void take_big_give_small(small_object_vector<T> &other) {
        copy_vector<T> current(other.big);
        other.big.~copy_vector();
        other.take_small(*this);
        clear(size);
        new (&big)copy_vector<T>(current);
    }


    friend void swap(small_object_vector<T> &first, small_object_vector<T> &second) {
        if (first.will_grow && second.will_grow) {
            std::swap(first.small, second.small);
        } else if (!first.will_grow && !second.will_grow) {
            std::swap(first.big, second.big);
        } else if (first.will_grow) {
            first.take_big_give_small(second);
        } else {
            second.take_big_give_small(first);
        }
        std::swap(first.size, second.size);
        std::swap(first.will_grow, second.will_grow);
    }
private:
    bool will_grow;
    size_t size;
    static const size_t MAX = 5;
    union {
        copy_vector<T> big;
        T small[MAX];
    };
};

#endif //BIGINT_SMALL_OBJECT_VECTOR_H
