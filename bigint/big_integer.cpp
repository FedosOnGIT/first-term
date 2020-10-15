#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>


// ======================================================== initialisation ======================================================

const uint32_t SMALL_BITS = 0xFFFFFFFF;

uint32_t remainder(uint64_t result) {
    return result & SMALL_BITS;
}


static big_integer const &ten() {
    static const big_integer RESULT(10);
    return RESULT;
}

static const uint64_t BASE = (1ull << 32u);

big_integer::big_integer() : bits(), sign(false) {}

big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int a) : bits(1, std::abs(static_cast<int64_t>(a))) {
    sign = a < 0;
}

big_integer::big_integer(std::string const &str) : big_integer() {
    bool minus = str[0] == '-';
    for (size_t i = minus; i < str.size(); i++) {
        if (!std::isdigit(str[i])) {
            throw std::runtime_error("incorrect number");
        }
        *this *= ten();
        *this += (str[i] - '0');
    }
    sign = minus;
    this->normalise();
}

big_integer &big_integer::operator=(big_integer const &other) {
    big_integer copy(other);
    swap(*this, copy);
    return *this;
}

big_integer::~big_integer() = default;

// =====================================================================

// ========================== Operator: operation= =================================

big_integer &big_integer::operator+=(big_integer const &rhs) {
    *this = *this + rhs;
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    *this = *this - rhs;
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    *this = *this * rhs;
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    *this = *this / rhs;
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    *this = *this % rhs;
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    *this = *this & rhs;
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    *this = *this | rhs;
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    *this = *this ^ rhs;
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    *this = *this << rhs;
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    *this = *this >> rhs;
    return *this;
}

// ===================================================================

// ========================== Unary Operations ==============================

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    if (size() <= 1 && !(*this)[0]) {
        return *this;
    }
    big_integer copy = *this;
    copy.sign = !copy.sign;
    return copy;
}

big_integer big_integer::operator~() const {
    return -(*this + 1);
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}
// ================================================================================
// ======================================= add =========================================

big_integer operator+(big_integer const &a, big_integer const &b) {
    bool minus = a.sign ^b.sign;
    int first = 1, second = 1;
    bool more = false;
    if (minus) {
        more = abs(a) >= abs(b);
        first = more ? 1 : -1;
        second = more ? -1 : 1;
    }
    int carry = 0;
    big_integer answer;
    size_t firstSize = a.size(), secondSize = b.size();
    for (size_t i = 0; i < std::max(firstSize, secondSize) || carry == 1; i++) {
        int64_t one = a.return_value(i);
        int64_t two = b.return_value(i);
        uint64_t result = one * first + two * second + BASE + carry;
        answer.push_back(remainder(result));
        carry = static_cast<int>(result >> 32u) - 1;
    }
    if (minus) {
        answer.sign = more? a.sign :b.sign;
    } else {
        answer.sign = a.sign;
    }
    answer.normalise();
    return answer;
}

big_integer operator-(big_integer const &a, big_integer const &b) {
    return a + (-b);
}
// ============================================================================================


// ================================= multiply =================================================

// only multiplies on second, but doesn't normalise. Ex.: 1 0 0 * 0 = 0 0 0
big_integer increase(big_integer const &first, uint32_t second) {
    uint32_t carry = 0;
    big_integer multiply;
    for (size_t i = 0; i < first.size() || carry; i++) {
        uint64_t result = static_cast<uint64_t>(first[i]) * second + carry;
        multiply.push_back(remainder(result));
        carry = result >> 32u;
    }
    return multiply;
}

big_integer operator*(big_integer const &a, big_integer const &b) {
    big_integer answer;
    answer.allocate(a.size() + b.size() + 1);
    uint64_t carry;
    for (size_t i = 0; i < a.size(); i++) {
        carry = 0;
        for (size_t j = 0; j < b.size() || carry; j++) {
            uint64_t result =
                    static_cast<uint64_t>(a[i]) * (b.return_value(j)) + carry + answer[i + j];
            answer[i + j] = (remainder(result));
            carry = result >> 32u;
        }
    }
    answer.sign = a.sign ^ b.sign;
    answer.normalise();
    return answer;
}
// ==========================================================================================

// =============================== Division starts here =====================================
bool smaller(big_integer const &first, big_integer const &second, size_t index) {
    if (first.size() > second.size() + index && first[second.size() + index]) {
        return false;
    }
    for (size_t i = second.size(); i > 0; i--) {
        size_t j = i + index;
        if (first[j - 1] != second[i - 1]) {
            return first[j - 1] < second[i - 1];
        }
    }
    return false;
}

void decrease(big_integer &first, big_integer const &second, size_t index) {
    int borrow = 0;
    for (size_t i = 0; i < second.size() || borrow; i++) {
        uint64_t difference = BASE + first[index + i] - (second[i])
                                        + borrow;
        first[index + i] = remainder(difference);
        borrow = static_cast<int32_t>(difference >> 32u) - 1;
    }
}

big_integer short_div(big_integer const &first, uint32_t second) {
    uint64_t rest = 0;
    big_integer quotient;
    quotient.allocate(first.size());
    for (size_t i = first.size(); i > 0; i--) {
        uint64_t result = BASE * rest + first[i - 1];
        quotient[i - 1] = result / second;
        rest = result % second;
    }
    quotient.normalise();
    if (first.sign && rest) {
        quotient++;
    }
    return quotient;
}

big_integer operator/(big_integer const &a, big_integer const &b) {
    big_integer quotient;
    size_t position = b.size();
    if (position == 1) {
        quotient = short_div(abs(a), b[0]);
    } else {
        uint32_t f = BASE / (1ull + b[position - 1]);
        big_integer remainder = increase(a, f);
        big_integer divisor = increase(b, f);
        remainder.push_back(0);
        size_t length = remainder.size();
        position = divisor.size();
        for (int32_t i = length - position - 1; i >= 0; i--) {
            uint64_t r = BASE * (remainder[i + position]) +
                                   remainder[i + position - 1];
            uint64_t q = divisor[position - 1];
            auto qt = static_cast<uint32_t>(std::min(r / q, BASE - 1));
            big_integer result = increase(divisor, qt);
            while (smaller(remainder, result, i)) {
                qt--;
                result -= divisor;
            }
            decrease(remainder, result, i);
            quotient.push_back(qt);
        }
        quotient.reverse();
    }
    quotient.sign = a.sign ^ b.sign;
    quotient.normalise();
    return quotient;
}

// ==========================================================================================
big_integer operator%(big_integer const &a, big_integer const &b) {
    return a - (a / b * b);
}

big_integer bit_operation(big_integer const &a, big_integer const &b, const big_integer::function &function) {
    big_integer real_a = additional(a.size() < b.size() ? b : a);
    big_integer real_b = additional(a.size() < b.size() ? a : b);
    if (real_a.size() < real_b.size()) {
        swap(real_a, real_b);
    }
    big_integer answer;
    for (size_t i = 0; i < real_b.size(); i++) {
        answer.push_back(function(real_a[i], real_b[i]));
    }
    uint32_t help = real_b.sign ? UINT32_MAX : 0;
    for (size_t i = real_b.size(); i < real_a.size(); i++) {
        answer.push_back(function(real_a[i], help));
    }
    answer.sign = function(a.sign, b.sign);
    answer.normalise();
    return additional(answer);
}

big_integer operator&(big_integer const &a, big_integer const &b) {
    return bit_operation(a, b, [](uint32_t first, uint32_t second) { return first & second; });
}

big_integer operator|(big_integer const &a, big_integer const &b) {
    return bit_operation(a, b, [](uint32_t first, uint32_t second) { return first | second; });
}

big_integer operator^(big_integer const &a, big_integer const &b) {
    return bit_operation(a, b, [](uint32_t first, uint32_t second) { return first ^ second; });
}

big_integer operator<<(big_integer const &a, int b) {
    if (b > 0) {
        big_integer answer = a;
        uint32_t small = b % 32, big = b / 32, short_multiply = (1u << small);
        answer = increase(answer, short_multiply);
        answer.reverse();
        for (size_t i = 0; i < big; i++) {
            answer.push_back(0);
        }
        answer.reverse();
        answer.sign = a.sign;
        return answer;
    } else {
        return a >> -b;
    }
}

big_integer operator>>(big_integer const &a, int b) {
    if (b > 0) {
        big_integer answer = a;
        uint32_t small = b % 32, big = b / 32, short_divide = (1u << small), length = answer.size();
        answer.reverse();
        for (size_t i = 0; i < std::min(big, length); i++) {
            answer.bits.pop_back();
        }
        answer.reverse();
        answer = short_div(answer, short_divide);
        answer.sign = a.sign;
        answer.normalise();
        return answer;
    } else {
        return a << -b;
    }
}


bool operator==(big_integer const &a, big_integer const &b) {
    if (a.check_zero(b)) {
        return true;
    }
    if (a.size() != b.size() || a.sign != b.sign) {
        return false;
    }
    for (size_t i = a.size(); i > 0; i--) {
        if (a[i - 1] != b[i - 1]) {
            return false;
        }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.check_zero(b)) {
        return false;
    }
    if (a.sign != b.sign) {
        return a.sign > b.sign;
    }
    if (a.size() != b.size()) {
        return a.sign? a.size() > b.size() : a.size() < b.size();
    }
    for (int i = a.size(); i > 0; i--) {
        if (a[i - 1] != b[i - 1]) {
            return a.sign? a[i - 1] > b[i - 1]: a[i - 1] < b[i - 1];
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return (a < b) || (a == b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return (a > b) || (a == b);
}

std::string to_string(big_integer const &a) {
    std::string answer;
    big_integer decreased = a;

    while (a.size() && decreased.bits.back() != 0) {
        big_integer result = decreased % ten();
        decreased /= ten();
        answer.push_back(result[0] + '0');
    }
    if (answer.empty()) {
        return "0";
    }
    if (a.sign) {
        answer.push_back('-');
    }
    reverse(answer.begin(), answer.end());
    return answer;
}

big_integer abs(big_integer const &number) {
    return number.sign ? -number : number;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
