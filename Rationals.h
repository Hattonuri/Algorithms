#pragma once

#include <algorithm>
#include <cstdlib>

// Most operations don't use references because that's
// the solution to references invalidations
class Rational {
 private:
    int64_t multiplier = 0;
    int64_t divisor = 1;

 public:
    // Haven't made it explicit to interact with right-side int without redundant code
    Rational(int64_t numerator = 0, int64_t denumerator = 1);

    static Rational& MakeIrreducible(Rational* value);

    // For user not to ignore return value :(
    [[nodiscard]] int64_t numerator() const;

    [[nodiscard]] int64_t denominator() const;

    Rational operator+() const;

    Rational operator-() const;

    bool operator==(Rational that) const;

    bool operator!=(Rational that) const;

    Rational& operator+=(Rational that);

    Rational& operator-=(Rational that);

    Rational& operator*=(Rational that);

    Rational& operator/=(Rational that);

    Rational operator+(Rational that) const;

    Rational operator-(Rational that) const;

    Rational operator*(Rational that) const;

    Rational operator/(Rational that) const;

    Rational& operator++();

    const Rational operator++(int);

    Rational& operator--();

    const Rational operator--(int);
};

Rational& Rational::MakeIrreducible(Rational* value) {
    if (value->divisor < 0) {
        value->multiplier = -value->multiplier;
        value->divisor = -value->divisor;
    }

    int64_t commonMultiplier = std::__gcd(std::abs(value->multiplier), value->divisor);
    value->multiplier /= commonMultiplier;
    value->divisor /= commonMultiplier;
    return *value;
}

Rational::Rational(int64_t numerator, int64_t denumerator) : multiplier(numerator), divisor(denumerator) {
    MakeIrreducible(this);
}

int64_t Rational::numerator() const {
    Rational answer = *this;
    return MakeIrreducible(&answer).multiplier;
}

int64_t Rational::denominator() const {
    Rational answer = *this;
    return MakeIrreducible(&answer).divisor;
}

Rational Rational::operator+() const {
    return Rational(multiplier, divisor);
}

Rational Rational::operator-() const {
    return Rational(-multiplier, divisor);
}

bool Rational::operator==(Rational that) const {
    return multiplier * that.divisor == that.multiplier * divisor;
}

bool Rational::operator!=(Rational that) const {
    return multiplier * that.divisor != that.multiplier * divisor;
}

Rational& Rational::operator+=(Rational that) {
    MakeIrreducible(this);
    MakeIrreducible(&that);
    *this = Rational(multiplier * that.divisor + divisor * that.multiplier,
                     divisor * that.divisor);

    MakeIrreducible(this);
    return *this;
}

Rational& Rational::operator-=(Rational that) {
    *this += -that;
    return *this;
}

Rational& Rational::operator*=(Rational that) {
    MakeIrreducible(this);
    MakeIrreducible(&that);
    this->multiplier *= that.multiplier;
    this->divisor *= that.divisor;
    MakeIrreducible(this);
    return *this;
}

Rational& Rational::operator/=(Rational that) {
    *this *= Rational(that.divisor, that.multiplier);
    return *this;
}

Rational Rational::operator+(Rational that) const {
    return Rational(multiplier, divisor) += that;
}

Rational Rational::operator-(Rational that) const {
    return Rational(multiplier, divisor) -= that;
}

Rational Rational::operator*(Rational that) const {
    return Rational(multiplier, divisor) *= that;
}

Rational Rational::operator/(Rational that) const {
    return Rational(multiplier, divisor) /= that;
}

Rational& Rational::operator++() {
    return *this += 1;
}

const Rational Rational::operator++(int) {
    Rational answer = *this;
    ++*this;
    return answer;
}

Rational& Rational::operator--() {
    return *this -= 1;
}

const Rational Rational::operator--(int) {
    Rational answer = *this;
    --*this;
    return answer;
}

Rational operator+(int64_t first, Rational second) {
    return second += first;
}

Rational operator-(int64_t first, Rational second) {
    return -(second - first);
}

Rational operator*(int64_t first, Rational second) {
    return second * first;
}

Rational operator/(int64_t first, Rational second) {
    return first * Rational(second.denominator(), second.numerator());
}
