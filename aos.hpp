#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <tuple>

template <typename... Ts> class AOS {
  using T = std::tuple<Ts...>;
  static constexpr size_t element_size = sizeof(T);
  size_t num_spots;
  T *base_array;

public:
  size_t get_size() { return num_spots * element_size; }

  AOS(size_t n) : num_spots(n) {
    // set the total array to be 64 byte alignmed

    uintptr_t length_to_allocate = get_size();
    base_array = static_cast<T *>(std::malloc(length_to_allocate));
    std::cout << "allocated size " << length_to_allocate << "\n";
  }
  ~AOS() { free(base_array); }
  void zero() { std::memset(base_array, 0, get_size()); }

  template <size_t... Is> auto get(size_t i) {
    return std::forward_as_tuple(std::get<Is>(base_array[i])...);
  }

  template <size_t... Is, class F> void map_range(F f) {
    for (size_t i = 0; i < num_spots; i++) {
      std::apply(f, get<Is...>(i));
    }
  }
};