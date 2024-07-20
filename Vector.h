#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <cmath>
#include <functional> // Include this for std::hash
#include <type_traits>

// Enable if the number of arguments matches N and all are convertible to float
template<std::size_t N, typename... T>
using vector_t = std::enable_if_t<(sizeof...(T) == N) && std::conjunction_v<std::is_convertible<T, float>...>>;

template<std::size_t N>
class Vector {
    std::array<float, N> elems;
public:
    Vector() = default;

    template<typename... Args, vector_t<N, Args...>* = nullptr>
    explicit Vector(Args... args) : elems{static_cast<float>(args)...} {}

    // Addition
    Vector<N> operator+(const Vector<N>& other) const {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i) {
            result.elems[i] = this->elems[i] + other.elems[i];
        }
        return result;
    }

    // Subtraction
    Vector<N> operator-(const Vector<N>& other) const {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i) {
            result.elems[i] = this->elems[i] - other.elems[i];
        }
        return result;
    }

    // Dot Product
    float operator*(const Vector<N>& other) const {
        float result = 0;
        for (std::size_t i = 0; i < N; ++i) {
            result += this->elems[i] * other.elems[i];
        }
        return result;
    }

    // Scalar multiplication
    Vector<N> operator*(float scalar) const {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i) {
            result.elems[i] = this->elems[i] * scalar;
        }
        return result;
    }

    bool operator<(const Vector<N>& other) const {
        float a = 0;
        float b = 0;
        for (std::size_t i = 0; i < N; ++i) {
            a += (this->elems[i] * this->elems[i]);
            b += (other.elems[i] * other.elems[i]);
        }
        return sqrt(a) < sqrt(b);
    }

    bool operator>(const Vector<N>& other) const {
        float a = 0;
        float b = 0;
        for (std::size_t i = 0; i < N; ++i) {
            a += (this->elems[i] * this->elems[i]);
            b += (other.elems[i] * other.elems[i]);
        }
        return sqrt(a) > sqrt(b);
    }

    bool operator==(const Vector<N>& other) const {
        for (std::size_t i = 0; i < N; ++i) {
            if (this->elems[i] != other.elems[i]) return false;
        }
        return true;
    }

    // Indexing
    float& operator[](std::size_t index) {
        if (index >= N) {
            throw std::out_of_range("Index out of bounds");
        }
        return elems[index];
    }

    const float& operator[](std::size_t index) const {
        if (index >= N) {
            throw std::out_of_range("Index out of bounds");
        }
        return elems[index];
    }

    // Access elements for hashing
    const std::array<float, N>& get() const {
        return elems;
    }
};

// Specialize std::hash for Vector<N>
namespace std {
    template<std::size_t N>
    struct hash<Vector<N>> {
        size_t operator()(const Vector<N>& vec) const {
            size_t seed = 0;
            for (const auto& elem : vec.get()) {
                seed ^= std::hash<float>{}(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

#endif // VECTOR_H
