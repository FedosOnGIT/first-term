//
// Created by fedos on 09.07.2020.
//
#include <vector>
#ifndef BIGINT_BUFFER_H
#define BIGINT_BUFFER_H

// the main vector, contains an array of elements, and counts how many elements lik on it
template<typename T>
struct buffer {
    std::vector<T> root;
    size_t counter;

    buffer() : root(), counter(1) {};

    explicit buffer(std::vector<T> const& change) : root(change), counter(1) {};

    buffer(T* begin, T* end) : root(begin, end), counter(1) {};
};

#endif //BIGINT_BUFFER_H
