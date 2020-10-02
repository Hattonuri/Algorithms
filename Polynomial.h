#pragma once

#include <forward_list>
#include <iostream>
#include <map>
#include <vector>

template<typename ValueType>
class Polynomial {
 public:
    using const_iterator = typename std::map<size_t, ValueType>::const_iterator;

    // We should be able to create double polynomial from vector<int>
    template<typename OtherValueType>
    explicit Polynomial(const std::vector<OtherValueType>& other) :
        Polynomial(other.begin(), other.end()) {}

    // Haven't made it explicit for automatic conversions in operations
    // Added enable_if for normal work of vector constructor and copy/move constructors
    Polynomial(ValueType other = ValueType(0));

    // Have't reserved memory because OtherIter can be not RandomAccess
    template<typename OtherIter>
    Polynomial(OtherIter first, OtherIter last);

    const std::map<size_t, ValueType>& GetSequence() const;

    bool operator==(const Polynomial& other) const;

    bool operator!=(const Polynomial& other) const;

    ValueType operator[](size_t i) const;

    int64_t Degree() const;

    const_iterator begin() const;

    const_iterator end() const;

    Polynomial& operator+=(const Polynomial& other);

    Polynomial& operator-=(const Polynomial& other);

    Polynomial& operator*=(const Polynomial& other);

    Polynomial operator+(const Polynomial& other) const;

    Polynomial operator-(const Polynomial& other) const;

    Polynomial operator*(const Polynomial& other) const;

    ValueType operator()(ValueType other) const;

    Polynomial operator&(const Polynomial& other) const;

    Polynomial operator/(const Polynomial& other) const;

    Polynomial operator%(const Polynomial& other) const;

    Polynomial operator,(const Polynomial& other) const;

    Polynomial Pow(const size_t power) const;

    Polynomial& RemoveZeros() {
        for (auto it = sequence.begin(); it != sequence.end();) {
            if (it->second == 0) {
                it = sequence.erase(it);
            } else {
                ++it;
            }
        }
        return *this;
    }

 private:
    std::map<size_t, ValueType> sequence;
};

template<typename ValueType>
template<typename OtherIter>
Polynomial<ValueType>::Polynomial(OtherIter first, OtherIter last) {
    size_t idx = 0;
    while (first != last) {
        if (*first != ValueType(0)) {
            sequence[idx] = *first;
        }
        ++first;
        ++idx;
    }
}

template<typename ValueType>
const std::map<size_t, ValueType>& Polynomial<ValueType>::GetSequence() const {
    return sequence;
}

template<typename ValueType>
bool Polynomial<ValueType>::operator==(const Polynomial<ValueType>& other) const {
    int64_t degree = Degree(), otherDegree = other.Degree();
    if (degree != otherDegree) {
        return false;
    }
    auto itOther = other.sequence.begin();
    for (auto it = sequence.begin(); it != sequence.end(); ++it) {
        while (itOther != other.sequence.end() && itOther->second == 0) {
            ++itOther;
        }
        if (it->second == 0) {
            continue;
        }
        if (itOther == other.sequence.end() || it->second != itOther->second) {
            return false;
        }
        ++itOther;
    }
    while (itOther != other.sequence.end() && itOther->second == 0) {
        ++itOther;
    }
    return itOther == other.sequence.end();
}

template<typename ValueType>
bool Polynomial<ValueType>::operator!=(const Polynomial<ValueType>& other) const {
    return !(*this == other);
}

template<typename ValueType>
ValueType Polynomial<ValueType>::operator[](size_t i) const {
    auto it = sequence.find(i);
    return it == sequence.end() ? 0 : it->second;
}

template<typename ValueType>
int64_t Polynomial<ValueType>::Degree() const {
    for (auto it = sequence.rbegin(); it != sequence.rend(); ++it) {
        if (it->second != 0) {
            return it->first;
        }
    }
    return -1;
}

template<typename ValueType>
typename Polynomial<ValueType>::const_iterator Polynomial<ValueType>::begin() const {
    return sequence.cbegin();
}

template<typename ValueType>
typename Polynomial<ValueType>::const_iterator Polynomial<ValueType>::end() const {
    int64_t degree = Degree();
    if (degree == -1) {
        return begin();
    }
    for (auto it = sequence.begin(); it != sequence.end(); ++it) {
        if (it->first == static_cast<size_t>(degree)) {
            return ++it;
        }
    }
    return sequence.end();
}

template<typename ValueType>
Polynomial<ValueType>& Polynomial<ValueType>::operator+=(const Polynomial<ValueType>& other) {
    for (const auto& i : other.sequence) {
        sequence[i.first] += i.second;
    }
    return RemoveZeros();
}

template<typename ValueType>
Polynomial<ValueType>& Polynomial<ValueType>::operator-=(const Polynomial<ValueType>& other) {
    for (const auto& i : other.sequence) {
        sequence[i.first] -= i.second;
    }
    return RemoveZeros();
}

template<typename ValueType>
Polynomial<ValueType>& Polynomial<ValueType>::operator*=(const Polynomial<ValueType>& other) {
    *this = *this * other;
    return RemoveZeros();
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator+(const Polynomial<ValueType>& other) const {
    return Polynomial(*this) += other;
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator-(const Polynomial<ValueType>& other) const {
    return Polynomial(*this) -= other;
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator*(const Polynomial<ValueType>& other) const {
    std::map<size_t, ValueType> newPoly;
    for (const auto& i : sequence) {
        for (const auto& j : other.sequence) {
            newPoly[i.first + j.first] += i.second * j.second;
        }
    }
    Polynomial<ValueType> ans(0);
    ans.sequence = std::move(newPoly);

    return ans.RemoveZeros();
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator&(const Polynomial& other) const {
    Polynomial<ValueType> ans = ValueType(0);
    for (const auto& i : sequence) {
        ans += other.Pow(i.first) * i.second;
    }
    return ans;
}

template<typename ValueType>
ValueType Polynomial<ValueType>::operator()(const ValueType other) const {
    return (*this & other)[0];
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator/(const Polynomial& other) const {
    int64_t degree = Degree(), otherDegree = other.Degree();
    if (degree == -1 || otherDegree == -1) {
        return ValueType(0);
    }

    std::forward_list<std::pair<size_t, ValueType>> ans;
    Polynomial<ValueType> remainder(*this);
    remainder.RemoveZeros();
    // Cause otherDegree can't be -1, we don't need to compare with 0
    // i >= otherDegree -> i >= 0
    int64_t i = degree;
    while (i >= otherDegree) {
        auto iter = remainder.sequence.lower_bound(i);
        if (iter != remainder.sequence.end() && static_cast<int64_t>(iter->first) == i) {
            if (iter->second != 0) {
                ans.emplace_front(i - otherDegree, iter->second / other[otherDegree]);
                for (const auto& j : other.sequence) {
                    remainder.sequence[i + j.first - otherDegree] -=
                        j.second * ans.front().second;
                }
            }
            --i;
        } else {
            if (iter == remainder.GetSequence().begin()) {
                break;
            }
            i = prev(iter)->first;
        }
        remainder.RemoveZeros();
    }
    Polynomial<ValueType> ansPoly(0);
    for (const std::pair<size_t, ValueType>& j : ans) {
        ansPoly.sequence[j.first] = j.second;
    }
    return ansPoly;
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator%(const Polynomial& other) const {
    return *this - *this / other * other;
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::operator,(const Polynomial& other) const {
    if (other.Degree() < 0) {
        return *this / sequence.rbegin()->second;
    }
    return (other, *this % other);
}

template<typename ValueType>
Polynomial<ValueType>::Polynomial(ValueType other) {
    if (other != ValueType(0)) {
        sequence[0] = other;
    }
}

template<typename ValueType>
Polynomial<ValueType> Polynomial<ValueType>::Pow(size_t power) const {
    Polynomial<ValueType> ans(1), multiplier(*this);
    while (power != 0) {
        if (power & 1ULL) {
            ans *= multiplier;
        }
        multiplier *= multiplier;
        power >>= 1ULL;
    }
    return ans;
}

template<typename ValueType>
Polynomial<ValueType> operator==(ValueType first, const Polynomial<ValueType>& second) {
    return second == first;
}

template<typename ValueType>
Polynomial<ValueType> operator!=(ValueType first, const Polynomial<ValueType>& second) {
    return second != first;
}

template<typename ValueType>
Polynomial<ValueType> operator+(ValueType first, const Polynomial<ValueType>& second) {
    return second + first;
}

template<typename ValueType>
Polynomial<ValueType> operator-(ValueType first, const Polynomial<ValueType>& second) {
    return Polynomial<ValueType>(first) -= second;
}

template<typename ValueType>
Polynomial<ValueType> operator*(ValueType first, const Polynomial<ValueType>& second) {
    return second * first;
}

template<typename ValueType>
Polynomial<ValueType> operator&(ValueType first, const Polynomial<ValueType>& second) {
    return Polynomial<ValueType>(first) & second;
}

template<typename ValueType>
Polynomial<ValueType> operator/(ValueType first, const Polynomial<ValueType>& second) {
    return Polynomial<ValueType>(first) / second;
}

template<typename ValueType>
Polynomial<ValueType> operator%(ValueType first, const Polynomial<ValueType>& second) {
    return Polynomial<ValueType>(first) % second;
}

template<typename ValueType>
ValueType abs(ValueType number) {
    return (number < ValueType(0) ? ValueType(0) - number : number);
}

template<typename ValueType>
Polynomial<ValueType> operator,(ValueType first, const Polynomial<ValueType>& other) {
    return (Polynomial<ValueType>(first), other);
}

template<typename ValueType>
std::ostream& operator<<(std::ostream& out, const Polynomial<ValueType>& poly) {
    int64_t degree = poly.Degree();
    if (degree == -1) {
        out << '0';
    } else {
        auto it = poly.GetSequence().rbegin();
        while (it != poly.GetSequence().rend()) {
            if (it->second != ValueType(0)) {
                if (it->second < ValueType(0)) {
                    out << '-';
                } else if (static_cast<int64_t>(it->first) != degree) {
                    out << '+';
                }
                if (abs(it->second) != ValueType(1) || it->first == 0) {
                    out << abs(it->second);
                    if (it->first != 0) {
                        out << '*';
                    }
                }
                if (it->first != 0) {
                    out << 'x';
                }
                if (it->first > 1) {
                    out << "^" << it->first;
                }
            }
            ++it;
        }
    }
    return out;
}
