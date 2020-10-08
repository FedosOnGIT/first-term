#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <memory>
#include <algorithm>

template<typename T>
struct vector {
    typedef T *iterator;
    typedef T const *const_iterator;

    // O(1) nothrow
    vector() : data_(nullptr), size_(0), capacity_(0) {}

    // O(N) strong
    vector(vector const &other) : data_(increase_buffer(other.data_, other.size_, other.size_)), size_(other.size_),
                                  capacity_(other.size_) {}

    // O(N) strong
    vector &operator=(vector const &other) {
        if (this != &other) {
            vector copy(other);
            swap(*this, copy);
        }
        return *this;
    }

    // O(N) nothrow
    ~vector() {
        real_clear(0, size_);
        operator delete(data_);
    }

    // O(1) nothrow
    T &operator[](size_t index) {
        return data_[index];
    }

    // O(1) nothrow
    T const &operator[](size_t index) const {
        return data_[index];
    }

    // O(1) nothrow
    T *data() {
        return data_;
    }

    // O(1) nothrow
    T const *data() const {
        return data_;
    }

    // O(1) nothrow
    size_t size() const {
        return size_;
    }

    // O(1) nothrow
    T &front() {
        return data_[0];
    }

    // O(1) nothrow
    T const &front() const {
        return data_[0];
    }

    // O(1) nothrow
    T &back() {
        return data_[size_ - 1];
    }

    // O(1) nothrow
    T const &back() const {
        return data_[size_ - 1];
    }

    // O(1)* strong
    void push_back(T const &input) {
        if (size_ == capacity_) {
            T link = input;
            make_buffer(increase_capacity());
            new(data_ + size_)T(link);
            size_++;
        } else {
            new(data_ + size_)T(input);
            size_++;
        }
    }

    // O(1) nothrow
    void pop_back() {
        size_--;
        data_[size_].~T();
    }

    // O(1) nothrow
    bool empty() const {
        return size_ == 0;
    }

    // O(1) nothrow
    size_t capacity() const {
        return capacity_;
    }

    // O(N) strong
    void reserve(size_t count) {
        if (count > capacity_) {
            make_buffer(count);
        }
    }

    // O(N) strong
    void shrink_to_fit() {
        if (size_ == 0) {
            make_buffer(size_);
        }
        capacity_ = size_;
    }

    // O(N) nothrow
    void clear() {
        real_clear(0, size_);
        size_ = 0;
    }


    // O(1) nothrow
    iterator begin() {
        return data_;
    }

    // O(1) nothrow
    iterator end() {
        return data_ + size_;
    }

    // O(1) nothrow
    const_iterator begin() const {
        return data_;
    }

    // O(1) nothrow
    const_iterator end() const {
        return data_ + size_;
    }

    // O(N) weak
    iterator insert(const_iterator pos, T const &input) {
        size_t position = pos - data_;
        push_back(input);
        for (size_t i = size_ - 1; i > position; i--) {
            std::swap(data_[i], data_[i - 1]);
        }
        return data_ + position;
    }

    // O(N) weak
    iterator erase(const_iterator pos) {
        size_t position = pos - data_;
        for (size_t i = position; i < size_ - 1; i++) {
            std::swap(data_[i], data_[i + 1]);
        }
        data_[size_ - 1].~T();
        size_--;
        return data_ + position;
    }


    // O(N) weak
    iterator erase(const_iterator first, const_iterator last) {
        size_t begin = first - data_;
        size_t end = last - data_;
        size_t new_size = size_ - (end - begin);
        T* new_data = static_cast<T *>(operator new(new_size * sizeof(T)));
        for (size_t i = 0; i < begin; i++) {
            new(new_data + i)T(data_[i]);
        }
        for (size_t i = end; i < size_; i++) {
            new(new_data + i - (end - begin))T(data_[i]);
        }
        clear();
        operator delete(data_);
        data_ = new_data;
        capacity_ = size_ = new_size;
        return data_ + begin;
    }

    template<typename K>
    friend void swap(vector<K> &first, vector<K> &second);

private:
    void real_clear(size_t begin, size_t end) {
        for (size_t i = end; i > begin; i--) {
            data_[i - 1].~T();
        }
    }

    size_t increase_capacity() const {
        return capacity_ * 2 + 1;
    }

    static T *increase_buffer(T *data, size_t new_size, size_t old_size) {
        T *new_data;
        if (new_size == 0) {
            new_data = nullptr;
        } else {
            new_data = static_cast<T *>(operator new(new_size * sizeof(T)));
            size_t position = 0;
            try {
                for (size_t i = 0; i < old_size; i++) {
                    new(new_data + i)T(data[i]);
                    position++;
                }
            } catch (...) {
                for (size_t i = position; i > 0; i--) {
                    new_data[i - 1].~T();
                }
                operator delete(new_data);
                throw;
            }
        }
        return new_data;
    }

    void make_buffer(size_t new_capacity) {
        T *new_data = increase_buffer(data_, new_capacity, size_);
        real_clear(0, size_);
        operator delete(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

private:
    T *data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
// O(1) nothrow
void swap(vector<T> &first, vector<T> &second) {
    std::swap(first.data_, second.data_);
    std::swap(first.size_, second.size_);
    std::swap(first.capacity_, second.capacity_);
}


#endif // VECTOR_H