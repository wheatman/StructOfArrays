#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <limits>
#include <tuple>
#include <utility>

namespace {
template <typename T> concept TupleLike = requires(T a) {
  std::tuple_size<T>::value;
  std::get<0>(a);
};

template <size_t I, typename... Ts>
constexpr void get_alignment_impl(std::array<std::size_t, sizeof...(Ts)> &arr) {
  if constexpr (I == sizeof...(Ts)) {
    return;
  } else {
    arr[I] = std::alignment_of_v<std::tuple_element_t<I, std::tuple<Ts...>>>;
    get_alignment_impl<I + 1, Ts...>(arr);
  }
}

template <size_t I, typename... Ts>
constexpr void get_size_impl(std::array<std::size_t, sizeof...(Ts)> &arr) {
  if constexpr (I == sizeof...(Ts)) {
    return;
  } else {
    arr[I] = sizeof(std::tuple_element_t<I, std::tuple<Ts...>>);
    get_size_impl<I + 1, Ts...>(arr);
  }
}

template <typename... Ts> constexpr auto get_alignment() {
  std::array<std::size_t, sizeof...(Ts)> arr = {};
  get_alignment_impl<0, Ts...>(arr);
  return arr;
}
template <typename... Ts> constexpr auto get_size() {
  std::array<std::size_t, sizeof...(Ts)> arr = {};
  get_size_impl<0, Ts...>(arr);
  return arr;
}

// helper function to print a tuple of any size
template <class Tuple, std::size_t N> struct TuplePrinter {
  static void print(const Tuple &t) {
    TuplePrinter<Tuple, N - 1>::print(t);
    std::cout << ", " << std::get<N - 1>(t);
  }
};

template <class Tuple> struct TuplePrinter<Tuple, 1> {
  static void print(const Tuple &t) { std::cout << std::get<0>(t); }
};

template <typename... Args, std::enable_if_t<sizeof...(Args) == 0, int> = 0>
void printTuple(const std::tuple<Args...> &t) {
  std::cout << "()\n";
}

template <typename... Args, std::enable_if_t<sizeof...(Args) != 0, int> = 0>
void printTuple(const std::tuple<Args...> &t) {
  std::cout << "(";
  TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
  std::cout << ")\n";
}

} // namespace

template <typename... Ts> class SOA {
  using T = std::tuple<Ts...>;
  static constexpr std::size_t num_types = sizeof...(Ts);
  static constexpr std::array<std::size_t, num_types> alignments =
      get_alignment<Ts...>();
  static constexpr std::array<std::size_t, num_types> sizes = get_size<Ts...>();

  size_t num_spots;
  void *base_array;

  template <size_t I>
  std::tuple_element_t<I, T> *get_starting_pointer_to_type() {
    static_assert(I < num_types);
    uintptr_t offset = 0;
    for (size_t i = 0; i < I; i++) {
      offset += num_spots * sizes[i];
      if (offset % alignments[i + 1] != 0) {
        offset += alignments[i + 1] - (offset % alignments[i + 1]);
      }
    }
    return (std::tuple_element_t<I, T> *)((char *)base_array + offset);
  }

public:
  void zero() {
    uintptr_t length = 0;
    for (size_t i = 0; i < num_types; i++) {
      length += length * sizes[i];
      if (i < num_types - 1) {
        // get aligned for the next type
        if (length % alignments[i + 1] != 0) {
          length += alignments[i + 1] - (length % alignments[i + 1]);
        }
      }
    }
    std::memset(base_array, 0, length);
  }
  SOA(size_t n) : num_spots(n) {
    // set the total array to be 64 byte alignmed
    constexpr size_t total_alignment = 64;
    uintptr_t length_to_allocate = 0;
    for (size_t i = 0; i < num_types; i++) {
      length_to_allocate += num_spots * sizes[i];
      if (i < num_types - 1) {
        // get aligned for the next type
        if (length_to_allocate % alignments[i + 1] != 0) {
          length_to_allocate +=
              alignments[i + 1] - (length_to_allocate % alignments[i + 1]);
        }
      }
    }
    base_array = static_cast<void *>(
        std::aligned_alloc(total_alignment, length_to_allocate));
    std::cout << "allocated size " << length_to_allocate << "\n";
  }
  size_t get_size() {
    uintptr_t length_to_allocate = 0;
    for (size_t i = 0; i < num_types; i++) {
      length_to_allocate += num_spots * sizes[i];
      if (i < num_types - 1) {
        // get aligned for the next type
        if (length_to_allocate % alignments[i + 1] != 0) {
          length_to_allocate +=
              alignments[i + 1] - (length_to_allocate % alignments[i + 1]);
        }
      }
    }
    return length_to_allocate;
  }

  template <size_t... Is> auto get(size_t i) {
    return std::forward_as_tuple(get_starting_pointer_to_type<Is>()[i]...);
  }

  static void print_type_details() {
    std::cout << "num types are " << num_types << "\n";
    std::cout << "their alignments are ";
    for (const auto e : alignments) {
      std::cout << e << ", ";
    }
    std::cout << "\n";
    std::cout << "their sizes are ";
    for (const auto e : sizes) {
      std::cout << e << ", ";
    }
    std::cout << "\n";
  }
  template <size_t I, size_t... rest> void print_soa() {
    for (size_t i = 0; i < num_spots; i++) {
      std::cout << std::get<0>(get<I>(i)) << ", ";
    }
    std::cout << "\n";
    if constexpr (sizeof...(rest) > 0) {
      print_soa<rest...>();
    }
  }

  template <size_t... Is> void print_aos() {
    for (size_t i = 0; i < num_spots; i++) {
      printTuple(get<Is...>(i));
    }
  }

  template <size_t... Is, class F> void map_range(F f) {
    for (size_t i = 0; i < num_spots; i++) {
      std::apply(f, get<Is...>(i));
    }
  }
};