#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <gmp.h>
#include <iosfwd>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <functional>

struct big_integer {

    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    big_integer(std::string const &str);

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

    uint32_t return_value(size_t index) const {
        return size() > index ? (*this)[index] : 0;
    }
private:

    using function = std::function<unsigned int(uint32_t, uint32_t)>;

    friend big_integer bit_operation(big_integer const &a, big_integer const &b, const big_integer::function &function);

    friend big_integer short_div(big_integer const &first, uint32_t second);

    unsigned int &operator[](size_t index) {
        return bits[index];
    }

    unsigned int operator[](size_t index) const {
        if (index >= size()) {
            uint32_t null = 0;
            return null;
        }
        return bits[index];
    }

    void normalise() {
        while (size() > 0) {
            if (bits.back() == 0) {
                bits.pop_back();
            } else {
                break;
            }
        }
        if (size() == 0) {
            sign = false;
        }
    }

    void push_back(uint32_t element) {
        bits.push_back(element);
    }

    friend bool smaller(big_integer const &first, big_integer const &second, size_t index);

    friend void decrease(big_integer &first, big_integer const &second, size_t index);

    friend big_integer increase(big_integer const &first, uint32_t second);

    void reverse() {
        std::reverse(bits.begin(), bits.end());
    }

    friend big_integer additional(big_integer const &a) {
        if (a.sign) {
            big_integer change;
            for (size_t i = 0; i < a.size(); i++) {
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
        bits = std::vector<uint32_t>(new_size, 0);
        sign = false;
    }

    size_t size() const{
        return bits.size();
    }

private:
    std::vector<uint32_t> bits;

    bool sign;
};

#endif // BIG_INTEGER_H
