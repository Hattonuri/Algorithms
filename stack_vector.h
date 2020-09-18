#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class StackVector {
 public:
    explicit StackVector(size_t a_size = 0) {
        if (a_size > N) {
            throw std::invalid_argument("size > capacity");
        }
        saved = a_size;
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    T* begin() {
        return data.begin();
    }

    T* end() {
        return data.begin() + saved;
    }

    const T* begin() const {
        return data.cbegin();
    }

    const T* end() const {
        return data.cbegin() + saved;
    }

    size_t Size() const {
        return saved;
    }
    size_t Capacity() const {
        return N;
    }

    void PushBack(const T& value) {
        if (saved == N) {
            throw std::overflow_error("overflow");
        }
        data[saved++] = value;
    }

    T PopBack() {
        if (saved == 0) {
            throw std::underflow_error("underflow");
        }
        return data[--saved];
    }

 private:
    size_t saved;
    std::array<T, N> data;
};
