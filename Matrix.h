#pragma once

#include <algorithm>
#include <forward_list>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#define BRUTE(name, a, b, cnt) for (int32_t name##cnt = a; name##cnt != b; ++name##cnt)
#define BRUTE1(name, a, b) BRUTE(name,a,b,1)
#define BRUTE2(name, a, b) BRUTE1(name,a,b) BRUTE(name,a,b,2)
#define BRUTE3(name, a, b) BRUTE2(name,a,b) BRUTE(name,a,b,3)
#define BRUTE4(name, a, b) BRUTE3(name,a,b) BRUTE(name,a,b,4)
#define BRUTE5(name, a, b) BRUTE4(name,a,b) BRUTE(name,a,b,5)
#define BRUTE6(name, a, b) BRUTE5(name,a,b) BRUTE(name,a,b,6)
#define BRUTE7(name, a, b) BRUTE6(name,a,b) BRUTE(name,a,b,7)
#define BRUTE8(name, a, b) BRUTE7(name,a,b) BRUTE(name,a,b,8)
#define BRUTE9(name, a, b) BRUTE8(name,a,b) BRUTE(name,a,b,9)
#define BRUTE10(name, a, b) BRUTE9(name,a,b) BRUTE(name,a,b,10)
#define BRUTE11(name, a, b) BRUTE10(name,a,b) BRUTE(name,a,b,11)
#define BRUTE12(name, a, b) BRUTE11(name,a,b) BRUTE(name,a,b,12)

#define THROW(TYPE, TEXT) {                                 \
    std::cerr << "Exception at line " << __LINE__ << '\n'   \
              << TEXT << '\n';                              \
    throw std::TYPE("");                                    \
}

#define MULTITHREAD_VOID(func) {                                                           \
    size_t rowsForThread = std::max(contents.size() / std::thread::hardware_concurrency(), \
                                static_cast<size_t>(1));                                   \
    std::forward_list<std::future<void>> futures;                                          \
    size_t from = 0;                                                                       \
    while (from != contents.size()) {                                                      \
        size_t to = from + rowsForThread;                                                  \
        if (contents.size() - to < rowsForThread) {                                        \
            to = contents.size();                                                          \
        }                                                                                  \
        futures.push_front(std::async([&, from, to] {                                      \
            size_t i = from;                                                               \
            while (i != to) {                                                              \
                func                                                                       \
                ++i;                                                                       \
            }                                                                              \
        }));                                                                               \
                                                                                           \
        from = to;                                                                         \
    }                                                                                      \
    for (auto &f : futures) {                                                              \
        f.get();                                                                           \
    }                                                                                      \
}                                                                                          \

template<typename ValueType = double>
class Matrix {
 public:
    using container_type = std::vector<std::vector<ValueType>>;
    using value_type = ValueType;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    Matrix(size_t rows, size_t columns, ValueType value = ValueType()) :
        contents(container_type(rows, std::vector<ValueType>(columns, value))) {}

    Matrix(std::vector<std::vector<ValueType>> contents) : contents(std::move(contents)) {}

    static Matrix Unit(size_t size);

    [[nodiscard]] bool empty() const;

    [[nodiscard]] std::pair<size_t, size_t> size() const;

    [[nodiscard]] size_t Rows() const;

    [[nodiscard]] size_t Columns() const;

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    std::vector<ValueType>& operator[](size_t idx);

    const std::vector<ValueType>& operator[](size_t idx) const;

    bool operator==(const Matrix& other);

    bool operator!=(const Matrix& other);

    Matrix& operator-();

    Matrix& operator+=(const Matrix& other);

    Matrix& operator-=(const Matrix& other);

    Matrix& operator*=(const Matrix& other);

    Matrix& operator*=(ValueType other);

    Matrix& operator/=(ValueType other);

    Matrix operator+(const Matrix& other) const;

    Matrix operator-(const Matrix& other) const;

    Matrix operator*(const Matrix& other) const;

    Matrix operator*(const ValueType other) const;

    Matrix operator/(ValueType other) const;

    ValueType Tr() const;

    Matrix Transponed() const;

    Matrix& Transpone();

    Matrix& Pow(size_t pow) const;
 private:
    container_type contents;
};

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::Unit(size_t size) {
    Matrix<ValueType> ans(size, size);
    for (size_t i = 0; i != size; ++i) {
        ans[i][i] = 1;
    }
    return ans;
}

template<typename ValueType>
bool Matrix<ValueType>::empty() const {
    return contents.empty() || contents.front().empty();
}

template<typename ValueType>
std::pair<size_t, size_t> Matrix<ValueType>::size() const {
    return {Rows(), Columns()};
}

template<typename ValueType>
size_t Matrix<ValueType>::Rows() const {
    return contents.size();
}

template<typename ValueType>
size_t Matrix<ValueType>::Columns() const {
    return Rows() == 0 ? 0 : contents.front().size();
}

template<typename ValueType>
std::vector<ValueType>& Matrix<ValueType>::operator[](size_t idx) {
    return contents[idx];
}

template<typename ValueType>
const std::vector<ValueType>& Matrix<ValueType>::operator[](size_t idx) const {
    return contents[idx];
}

template<typename ValueType>
bool Matrix<ValueType>::operator==(const Matrix& other) {
    return contents == other.contents;
}

template<typename ValueType>
bool Matrix<ValueType>::operator!=(const Matrix& other) {
    return !(*this == other);
}

template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::operator-() {
    for (auto& i : contents) {
        for (auto& j : i) {
            j = -j;
        }
    }
    return *this;
}
template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::operator+=(const Matrix& other) {
    if (empty() || size() != other.size()) {
        THROW(out_of_range, "Matrices differ in size")
    }
    MULTITHREAD_VOID(
        for (size_t j = 0; j != Columns(); ++j) {
            contents[i][j] += other.contents[i][j];
        }
    )
    return *this;
}

template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::operator-=(const Matrix& other) {
    if (size() != other.size()) {
        THROW(out_of_range, "Matrices differ in size")
    }
    MULTITHREAD_VOID(
        for (size_t j = 0; j != Columns(); ++j) {
            contents[i][j] -= other[i][j];
        }
    )
    return *this;
}

template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::operator*=(const Matrix& other) {
    *this = std::move(*this * other);
    return *this;
}

template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::operator*=(const ValueType other) {
    MULTITHREAD_VOID(
        for (size_t j = 0; j != Columns(); ++j) {
            contents[i][j] *= other;
        }
    )
    return *this;
}

template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::operator/=(ValueType other) {
    MULTITHREAD_VOID(
        for (size_t j = 0; j != Columns(); ++j) {
            contents[i][j] /= other;
        }
    )
    return *this;
}

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::operator+(const Matrix& other) const {
    return Matrix(*this) += other;
}

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::operator-(const Matrix& other) const {
    return Matrix(*this) -= other;
}

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::operator*(const Matrix& other) const {
    if (empty() || other.empty() || contents.front().size() != other.contents.size()) {
        THROW(out_of_range, "Can't multiply")
    }
    Matrix<ValueType> ans(Rows(), other.Columns());
    MULTITHREAD_VOID(
        for (size_t j = 0; j != Columns(); ++j) {
            for (size_t k = 0; k != other.Columns(); ++k) {
                ans[i][k] += contents[i][j] * other[j][k];
            }
        }
    )
    return ans;
}

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::operator*(const ValueType other) const {
    return Matrix(*this) *= other;
}

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::operator/(ValueType other) const {
    return Matrix(*this) /= other;
}

template<typename ValueType>
ValueType Matrix<ValueType>::Tr() const {
    ValueType ans{};
    for (size_t i = 0; i != std::min(Rows(), Columns()); ++i) {
        ans += contents[i][i];
    }
    return ans;
}

template<typename ValueType>
Matrix<ValueType> Matrix<ValueType>::Transponed() const {
    Matrix<ValueType> ans(Columns(), Rows());
    MULTITHREAD_VOID(
        for (size_t j = 0; j != Columns(); ++j) {
            ans[j][i] = contents[i][j];
        }
    )
    return ans;
}

template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::Transpone() {
    *this = Transponed();
    return *this;
}

template<typename ValueType>
typename Matrix<ValueType>::iterator Matrix<ValueType>::begin() {
    return contents.begin();
}

template<typename ValueType>
typename Matrix<ValueType>::const_iterator Matrix<ValueType>::begin() const {
    return contents.cbegin();
}

template<typename ValueType>
typename Matrix<ValueType>::iterator Matrix<ValueType>::end() {
    return contents.end();
}

template<typename ValueType>
typename Matrix<ValueType>::const_iterator Matrix<ValueType>::end() const {
    return contents.cend();
}
template<typename ValueType>
Matrix<ValueType>& Matrix<ValueType>::Pow(size_t pow) const {
    Matrix<ValueType> ans = Unit(Columns()), mult(*this);
    while (pow != 0LL) {
        if (pow & 1ULL) {
            ans *= mult;
        }
        mult *= mult;
        pow >>= 1ULL;
    }
    return ans;
}

template<typename ValueType>
std::ostream& operator<<(std::ostream& out, const Matrix<ValueType>& mrx) {
    for (const auto& i : mrx) {
        for (const ValueType& j : i) {
            out << j << ' ';
        }
        out << '\n';
    }
    return out;
}

template<typename ValueType>
Matrix<ValueType> operator *(ValueType first, const Matrix<ValueType> &second) {
    return second * first;
}
