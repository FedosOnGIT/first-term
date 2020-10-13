//
// Created by fedos on 09.07.2020.
//
#include "copy-vector.h"

#ifndef BIGINT_SMALL_OBJECT_VECTOR_H
#define BIGINT_SMALL_OBJECT_VECTOR_H

template<typename T>
struct small_object_vector {

    small_object_vector() : is_small(true), size_(0) {}

    small_object_vector(small_object_vector<T> const& other) {
        size_ = other.size_;
        is_small = other.is_small;
        if (is_small) {
            take_small(other);
        } else {
            new (&big)copy_vector<T>(other.big);
        }
    }

    small_object_vector(size_t length, const T& element) {
        if (length < MAX) {
            for (size_t i = 0; i < length; i++) {
                new (small + i)T(element);
            }
        } else {
            // noexcept
            new (&big)copy_vector<T>(length, element);
        }
        size_ = length;
        is_small = size_ < MAX;
    }

    T& operator[](size_t index) {
        if (is_small) {
            return small[index];
        } else {
            return big[index];
        }
    }

    const T& operator[](size_t index) const {
        if (is_small) {
            return small[index];
        } else {
            return big[index];
        }
    }

    small_object_vector<T>& operator=(small_object_vector<T> const& other) {
        if (this != &other) {
            if (other.is_small) {
                take_small(other);
            } else {
                new(&big)copy_vector<T>(other.big);
            }
            is_small = other.is_small;
            size_ = other.size_;
        }
        return *this;
    }

    ~small_object_vector<T>() {
        if (is_small) {
            clear_small(size_);
        } else {
            big.~copy_vector();
        }
    }

    void push_back(T const& element) {
        convert_small_to_big(size_ + 1);
        if (is_small) {
            new (small + size_)T(element);
        } else {
            big.push_back(element);
        }
        size_++;
    }

    void pop_back() {
        size_--;
        if (is_small) {
            small[size_].~T();
        } else {
            big.pop_back();
        }
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void reverse() {
        if (is_small) {
            std::reverse(small, small + size_);
        } else {
            big.reverse();
        }
    }

private:
    void clear_small(size_t number) {
        for (size_t i = number; i > 0; i--) {
            small[i].~T();
        }
    }

    void convert_small_to_big(int new_size) {
        if (is_small && new_size == MAX) {
            copy_vector<T> temp(std::begin(small), std::end(small) - 1);
            clear_small(size_);
            // noexcept
            new (&big)copy_vector<T>(temp);
            is_small = false;
        }
    }

    // takes empty vector small!
    void take_small(small_object_vector<T> const& other) {
        size_t position = 0;
        try {
            for (; position<other.size(); position++) {
                new(small + position)T(other.small[position]);
            }
        } catch (...) {
            clear_small(position);
            throw;
        }
    }

    void take_big_give_small(small_object_vector<T> &other) {
        copy_vector<T> current(other.big);
        other.big.~copy_vector();
        try {
            other.take_small(*this);
        } catch (...) {
            new(&other.big)copy_vector<T>(current);
            throw;
        }
        new(&big)copy_vector<T>(current);
    }


    friend void swap(small_object_vector<T> &first, small_object_vector<T> &second) {
        using std::swap;
        if (first.is_small && second.is_small) {
            swap(first.small, second.small);
        } else if (!first.is_small && !second.is_small) {
            swap(first.big, second.big);
        } else if (first.is_small) {
            first.take_big_give_small(second);
        } else {
            second.take_big_give_small(first);
        }
        swap(first.size_, second.size_);
        swap(first.is_small, second.is_small);
    }
private:
    // vector WILL GROW from small to big
    bool is_small;
    size_t size_;
    static constexpr size_t MAX = 5;
    union {
        copy_vector<T> big;
        T small[MAX];
    };
};

template<typename T>
bool operator==(small_object_vector<T> const& first, small_object_vector<T> const& second) {
    if (first.size() != second.size()) {
        return false;
    }
    for (size_t i = 0; i < first.size_; i++) {
        if (first[i] != second[i]) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool operator!=(small_object_vector<T> const& first, small_object_vector<T> const& second) {
    return !(first == second);
}
#endif //BIGINT_SMALL_OBJECT_VECTOR_H
