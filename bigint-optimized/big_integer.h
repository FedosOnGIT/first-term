#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <gmp.h>
#include <iosfwd>
#include "small-object-vector.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <functional>

struct big_integer {

    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend void swap(big_integer &first, big_integer &second) {
        std::swap(first.size, second.size);
        std::swap(first.sign, second.sign);
        first.bits.swap(second.bits);
    }

    friend big_integer abs(big_integer const &number);

    friend std::string to_string(big_integer const &a);

    friend big_integer operator+(big_integer const &a, big_integer const &b);

    friend big_integer operator-(big_integer const &a, big_integer const &b);

    friend big_integer operator*(big_integer const &a, big_integer const &b);

    friend big_integer operator/(big_integer const &a, big_integer const &b);

    friend big_integer operator%(big_integer const &a, big_integer const &b);

    friend big_integer operator&(big_integer const &a, big_integer const &b);

    friend big_integer operator|(big_integer const &a, big_integer const &b);

    friend big_integer operator^(big_integer const &a, big_integer const &b);

    friend big_integer operator<<(big_integer const &a, int b);

    friend big_integer operator>>(big_integer const &a, int b);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend std::ostream &operator<<(std::ostream &s, big_integer const &a);

private:
    using function = std::function<unsigned int(uint32_t , uint32_t)>;

    friend big_integer bit_operation(big_integer const &a, big_integer const &b, const big_integer::function &function);

    friend big_integer short_div(big_integer const &first, uint32_t second);

    uint32_t &operator[](size_t index) {
        return bits[index];
    }

    uint32_t back() {
        if (!size) {
            return 0;
        }
        return bits[size - 1];
    }

    unsigned int const &operator[](size_t index) const {
        return bits[index];
    }

    void normalise() {
        size = bits.size();
        while (!bits.empty()) {
            if (bits[size - 1] == 0) {
                bits.pop_back();
                size--;
            } else {
                break;
            }
        }
        if (bits.empty()) {
            sign = false;
            bits.push_back(0);
        }
    }

    void push_back(uint32_t element) {
        if (!size) {
            bits[0] = element;
        } else {
            bits.push_back(element);
        }
        size++;
    }

    friend bool smaller(big_integer const &first, big_integer const &second, size_t index);

    friend void decrease(big_integer &first, big_integer const &second, size_t index);

    friend big_integer increase(big_integer const &first, uint32_t second);

    void reverse() {
        bits.reverse();
    }

    friend big_integer additional(big_integer const &a) {
        if (a.sign) {
            big_integer change;
            for (size_t i = 0; i < a.size; i++) {
                change.push_back(~a[i]);
            }
            change++;
            change.sign = true;
            return change;
        } else {
            return a;
        }
    }

    void allocate(size_t new_size) {
        bits = small_object_vector<uint32_t>(new_size, 0);
        size = new_size;
        sign = false;
    }

private:
    size_t size;

    bool sign;

    small_object_vector<uint32_t> bits;
};

#endif // BIG_INTEGER_H
