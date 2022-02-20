#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

template <typename... Ts> class SOA {
  using T = std::tuple<Ts...>;
  static constexpr std::size_t num_types = sizeof...(Ts);
  static constexpr std::array<std::size_t, num_types> alignments = {
      std::alignment_of_v<Ts>...};

  template <int I> using NthType = typename std::tuple_element<I, T>::type;

  static constexpr std::array<std::size_t, num_types> sizes = {sizeof(Ts)...};

  static constexpr size_t total_alignment = 64;

  size_t num_spots;
  void *base_array;

  template <size_t I> NthType<I> *get_starting_pointer_to_type() {
    static_assert(I < num_types);
    uintptr_t offset = 0;
    for (size_t i = 0; i < I; i++) {
      offset += num_spots * sizes[i];
      if (offset % alignments[i + 1] != 0) {
        offset += alignments[i + 1] - (offset % alignments[i + 1]);
      }
    }
    return (NthType<I> *)((char *)base_array + offset);
  }

  template <std::size_t... Is>
  SOA resize_impl(
      size_t n, [[maybe_unused]] std::integer_sequence<size_t, Is...> int_seq) {
    SOA soa(n);
    size_t end = std::min(num_spots, soa.num_spots);
    for (size_t i = 0; i < end; i++) {
      soa.get<Is...>(i) = get<Is...>(i);
    }
    const T zero;
    for (size_t i = end; i < soa.num_spots; i++) {
      soa.get<Is...>(i) = zero;
    }

    return soa;
  }

  template <size_t I> bool print_field() {
    for (size_t i = 0; i < num_spots; i++) {
      std::cout << std::get<0>(get<I>(i)) << ", ";
    }
    std::cout << "\n";
    return true;
  }
  template <size_t... Is>
  void print_soa_impl(
      [[maybe_unused]] std::integer_sequence<size_t, Is...> int_seq) {
    auto x = {print_field<Is>()...};
    (void)x;
  }

  template <size_t... Is>
  auto get_impl(size_t i,
                [[maybe_unused]] std::integer_sequence<size_t, Is...> int_seq) {
    return std::forward_as_tuple(get_starting_pointer_to_type<Is>()[i]...);
  }

public:
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
    if (length_to_allocate % total_alignment != 0) {
      length_to_allocate +=
          total_alignment - (length_to_allocate % total_alignment);
    }
    return length_to_allocate;
  }
  SOA(size_t n) : num_spots(n) {
    // set the total array to be 64 byte alignmed

    uintptr_t length_to_allocate = get_size();
    base_array = static_cast<void *>(
        std::aligned_alloc(total_alignment, length_to_allocate));
    std::cout << "allocated size " << length_to_allocate << "\n";
  }

  ~SOA() { free(base_array); }

  void zero() { std::memset(base_array, 0, get_size()); }

  template <size_t... Is> auto get(size_t i) {
    if constexpr (sizeof...(Is) > 0) {
      return get_impl<Is...>(i, {});
    } else {
      return get_impl(i, std::make_index_sequence<num_types>{});
    }
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
  template <size_t... Is> void print_soa() {
    if constexpr (sizeof...(Is) > 0) {
      print_soa_impl<Is...>({});
    } else {
      print_soa_impl(std::make_index_sequence<num_types>{});
    }
  }

  template <size_t... Is, class F>
  void map_range(F f, size_t start = 0,
                 size_t end = std::numeric_limits<size_t>::max()) {
    if (end == std::numeric_limits<size_t>::max()) {
      end = num_spots;
    }
    for (size_t i = start; i < end; i++) {
      std::apply(f, get<Is...>(i));
    }
  }
  template <size_t... Is, class F>
  void map_range_with_index(F f, size_t start = 0,
                            size_t end = std::numeric_limits<size_t>::max()) {
    if (end == std::numeric_limits<size_t>::max()) {
      end = num_spots;
    }
    for (size_t i = start; i < end; i++) {
      std::apply(f, std::tuple_cat(std::make_tuple(i), get<Is...>(i)));
    }
  }
  template <size_t... Is> void print_aos() {
    map_range<Is...>(
        [](auto... args) { ((std::cout << args << ","), ...) << "\n"; });
  }

  SOA resize(size_t n) {
    return resize_impl(n, std::make_index_sequence<num_types>{});
  }
  template <size_t... Is> SOA<NthType<Is>...> pull_types() {
    SOA<NthType<Is>...> soa(num_spots);
    for (size_t i = 0; i < num_spots; i++) {
      soa.get(i) = get<Is...>(i);
    }
    return soa;
  }
};