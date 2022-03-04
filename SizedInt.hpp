#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <type_traits>

template <size_t I> class __attribute__((__packed__)) sized_uint {
  static_assert(I <= 8);
  std::array<uint8_t, I> data = {0};

public:
  constexpr sized_uint(uint64_t e) { std::memcpy(data.data(), &e, I); }
  template <size_t J> constexpr sized_uint(const sized_uint<J> &e) {
    auto el = e.get();
    std::memcpy(data.data(), &el, I);
  }
  constexpr sized_uint() { data.fill(0); }
  constexpr auto get() const {
    if constexpr (I == 1) {
      uint8_t x = 0;
      std::memcpy(&x, data.data(), I);
      return x;
    } else if constexpr (I == 2) {
      uint16_t x = 0;
      std::memcpy(&x, data.data(), I);
      return x;
    } else if constexpr (I <= 4) {
      uint32_t x = 0;
      std::memcpy(&x, data.data(), I);
      return x;
    } else {
      uint64_t x = 0;
      std::memcpy(&x, data.data(), I);
      return x;
    }
  }
};
template <size_t I>
std::ostream &operator<<(std::ostream &os, const sized_uint<I> &e) {
  os << +e.get();
  return os;
}

template <size_t I, typename T>
constexpr auto &operator+=(T &lhs, const sized_uint<I> &rhs) {
  lhs += rhs.get();
  return lhs;
}

template <size_t I, typename T>
constexpr auto operator+(const T lhs, const sized_uint<I> &rhs) {
  return lhs + rhs.get();
}

template <size_t I, typename T>
constexpr auto operator+(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() + rhs;
}

template <size_t I, size_t J>
constexpr auto operator+(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() + rhs.get();
}

template <size_t I, size_t J>
constexpr auto operator==(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() == rhs.get();
}
template <size_t I, size_t J>
constexpr auto operator!=(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() != rhs.get();
}

template <size_t I, size_t J>
constexpr auto operator<(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() < rhs.get();
}

template <size_t I, size_t J>
constexpr auto operator>(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() > rhs.get();
}

template <size_t I, size_t J>
constexpr auto operator<=(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() <= rhs.get();
}

template <size_t I, size_t J>
constexpr auto operator>=(const sized_uint<I> &lhs, const sized_uint<J> &rhs) {
  return lhs.get() >= rhs.get();
}

template <class T>
concept Integral = std::is_integral<T>::value;

template <size_t I, Integral T>
constexpr auto operator==(const T lhs, const sized_uint<I> &rhs) {
  return (typename std::make_unsigned<T>::type)(lhs) == rhs.get();
}

template <size_t I, Integral T>
constexpr auto operator!=(const T lhs, const sized_uint<I> &rhs) {
  return (typename std::make_unsigned<T>::type)(lhs) != rhs.get();
}

template <size_t I, Integral T>
constexpr auto operator<(const T lhs, const sized_uint<I> &rhs) {
  return (typename std::make_unsigned<T>::type)(lhs) < rhs.get();
}
template <size_t I, Integral T>
constexpr auto operator>(const T lhs, const sized_uint<I> &rhs) {
  return (typename std::make_unsigned<T>::type)(lhs) > rhs.get();
}

template <size_t I, Integral T>
constexpr auto operator<=(const T lhs, const sized_uint<I> &rhs) {
  return (typename std::make_unsigned<T>::type)(lhs) <= rhs.get();
}
template <size_t I, Integral T>
constexpr auto operator>=(const T lhs, const sized_uint<I> &rhs) {
  return (typename std::make_unsigned<T>::type)(lhs) >= rhs.get();
}

template <size_t I, Integral T>
constexpr auto operator==(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() == (typename std::make_unsigned<T>::type)(rhs);
}

template <size_t I, Integral T>
constexpr auto operator!=(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() != (typename std::make_unsigned<T>::type)(rhs);
}

template <size_t I, Integral T>
constexpr auto operator<(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() < (typename std::make_unsigned<T>::type)(rhs);
}

template <size_t I, Integral T>
constexpr auto operator>(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() > (typename std::make_unsigned<T>::type)(rhs);
}

template <size_t I, Integral T>
constexpr auto operator<=(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() <= (typename std::make_unsigned<T>::type)(rhs);
}

template <size_t I, Integral T>
constexpr auto operator>=(const sized_uint<I> &lhs, const T rhs) {
  return lhs.get() >= (typename std::make_unsigned<T>::type)(rhs);
}