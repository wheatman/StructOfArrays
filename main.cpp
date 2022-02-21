#include "aos.hpp"
#include "soa.hpp"

#include <cstdint>
#include <limits>
#include <random>
#include <sys/time.h>
#include <tuple>

static inline uint64_t get_time() {
  struct timeval st {};
  gettimeofday(&st, nullptr);
  return st.tv_sec * 1000000 + st.tv_usec;
}

using value_and_flag = struct value_and_flag {
  uint64_t value : 63;
  bool flag : 1;
};

std::ostream &operator<<(std::ostream &os, const value_and_flag &vf) {
  os << "{ " << vf.value << ", " << vf.flag << " } ";
  return os;
}

template <size_t I> class __attribute__((__packed__)) sized_int {
  static_assert(I <= 8);
  std::array<uint8_t, I> data;

public:
  sized_int(uint64_t e) { std::memcpy(data.data(), &e, I); }
  uint64_t get() const {
    uint64_t x = 0;
    std::memcpy(&x, data.data(), I);
    return x;
  }
};
template <size_t I>
std::ostream &operator<<(std::ostream &os, const sized_int<I> &e) {
  os << e.get();
  return os;
}

template <size_t I>
uint64_t &operator+=(uint64_t &lhs, const sized_int<I> &rhs) {
  lhs += rhs.get();
  return lhs;
}

template <size_t I>
uint64_t operator+(const uint64_t lhs, const sized_int<I> &rhs) {
  return lhs + rhs.get();
}
template <size_t I, size_t J>
uint64_t operator+(const sized_int<I> &lhs, const sized_int<J> &rhs) {
  return lhs.get() + rhs.get();
}

int main(int32_t argc, char *argv[]) {
  {
    SOA<int>::print_type_details();
    SOA<int, bool>::print_type_details();
    SOA<int, bool, short>::print_type_details();
    SOA<int, short, bool, long>::print_type_details();
    SOA<int, short, bool, char[3]>::print_type_details();
    SOA<int, short, bool, value_and_flag>::print_type_details();
    SOA<sized_int<3>, sized_int<5>, sized_int<6>,
        sized_int<7>>::print_type_details();
  }
  {
    size_t length = 10;
    auto tup = SOA<int, short, bool, long>(length);
    tup.zero();
    tup.get<0>(0) = std::make_tuple(1);
    tup.get<1>(1) = std::make_tuple(2);
    tup.get<2>(2) = std::make_tuple(true);
    tup.get<3>(3) = std::make_tuple(1000);

    tup.get<0>(7) = std::make_tuple(99);
    tup.get<0, 3>(8) = std::make_tuple(101, 202);
    tup.get(9) = std::make_tuple(5, 4, false, 100);
    tup.print_soa();
    tup.print_aos<0, 1, 2, 3>();
    tup.print_aos_with_index<0, 1, 2, 3>();
    std::cout << "\n";

    size_t sum_all = 0;
    tup.map_range<0, 3>(
        [&sum_all](auto... args) { sum_all += (0 + ... + args); });
    std::cout << sum_all << "\n";

    tup.map_range<0, 1>([](auto &x, auto &y) {
      x += 1;
      y -= 1;
    });
    tup.print_soa();

    auto tup2 = tup.resize(9);
    tup2.print_soa();

    auto tup3 = tup2.resize(20);
    tup3.print_soa();
    std::cout << "\n";
    tup3.print_soa<0, 3>();
    std::cout << "\n";

    auto tup4 = tup2.pull_types<0, 2>();
    tup4.print_type_details();
    tup4.print_soa();

    auto tup5 = SOA<value_and_flag, value_and_flag>(5);
    tup5.print_type_details();
    value_and_flag vf1 = {100, false};
    value_and_flag vf2 = {10, true};
    tup5.get(0) = std::make_tuple(vf1, vf2);
    tup5.get(1) = std::make_tuple(vf2, vf1);
    tup5.get(2) = std::make_tuple(vf1, vf2);
    tup5.get(3) = std::make_tuple(vf2, vf1);
    tup5.get(4) = std::make_tuple(vf1, vf2);
    tup5.print_soa();
    size_t sum_true = 0;
    tup5.map_range<0>([&sum_true](auto x) {
      if (x.flag)
        sum_true += x.value;
    });
    std::cout << "sum_true = " << sum_true << "\n";

    auto tup6 = SOA<sized_int<3>, sized_int<5>, sized_int<6>, sized_int<7>>(3);
    tup6.print_type_details();
    tup6.get(0) = std::make_tuple(1, 2, 3, 4);
    tup6.get(1) = std::make_tuple(5, 6, 7, 8);
    tup6.get(2) = std::make_tuple(9, 10, 11, 12);
    tup6.print_soa();
    sum_all = 0;
    tup6.map_range([&sum_all](auto... args) { sum_all += (0 + ... + args); });
    std::cout << "sum_all = " << sum_all << "\n";
  }
  uint64_t flag = 0xFFFFFFFF;
  if (argc > 2) {
    flag = std::strtol(argv[2], nullptr, 10);
  }

  if (argc > 1 && (flag & 1)) {
    std::cout << "\n SOA<uint8_t, uint16_t, uint32_t, uint64_t>\n ";
    uint64_t number_of_elements = std::strtol(argv[1], nullptr, 10);
    auto tup = SOA<uint8_t, uint16_t, uint32_t, uint64_t>(number_of_elements);
    std::cout << "size = " << tup.get_size() << "\n";
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::uniform_int_distribution<uint64_t> dis_int(
    //     0, std::numeric_limits<uint32_t>::max());
    // for (uint64_t i = 0; i < number_of_elements; i++) {
    //   tup.set(i, {dis_int(g), dis_int(g), dis_int(g), dis_int(g)});
    // }
    for (uint64_t i = 0; i < number_of_elements; i++) {
      tup.get(i) = std::make_tuple(i, 2 * i, 3 * i, 4 * i);
    }
    uint64_t start = 0;
    uint64_t end = 0;

    start = get_time();
    size_t sum_first = 0;
    tup.map_range<0>([&sum_first](auto x) { sum_first += x; });
    end = get_time();
    std::cout << "First time was " << end - start << "  sum was ";
    std::cout << sum_first << "\n";

    start = get_time();
    size_t sum_second = 0;
    tup.map_range<1>([&sum_second](auto x) { sum_second += x; });
    end = get_time();
    std::cout << "Second time was " << end - start << "  sum was ";
    std::cout << sum_second << "\n";

    start = get_time();
    size_t sum_third = 0;
    tup.map_range<2>([&sum_third](auto x) { sum_third += x; });
    end = get_time();
    std::cout << "Third time was " << end - start << "  sum was ";
    std::cout << sum_third << "\n";

    start = get_time();
    size_t sum_forth = 0;
    tup.map_range<3>([&sum_forth](auto x) { sum_forth += x; });
    end = get_time();
    std::cout << "Forth time was " << end - start << "  sum was ";
    std::cout << sum_forth << "\n";

    start = get_time();
    size_t sum_first_2 = 0;
    tup.map_range<0, 1>(
        [&sum_first_2](auto x, auto y) { sum_first_2 += x + y; });
    end = get_time();
    std::cout << "First 2 time was " << end - start << "  sum was ";
    std::cout << sum_first_2 << "\n";

    start = get_time();
    size_t sum_second_2 = 0;
    tup.map_range<2, 3>(
        [&sum_second_2](auto x, auto y) { sum_second_2 += x + y; });
    end = get_time();
    std::cout << "Second 2 time was " << end - start << "  sum was ";
    std::cout << sum_second_2 << "\n";

    start = get_time();
    size_t sum_all = 0;
    tup.map_range([&sum_all](auto... args) { sum_all += (0 + ... + args); });
    end = get_time();
    std::cout << "All time was " << end - start << "  sum was ";
    std::cout << sum_all << "\n";
  }
  if (argc > 1 && (flag & 2)) {
    std::cout
        << "\nSOA<sized_int<1>, sized_int<2>, sized_int<4>, sized_int<8>>\n";
    uint64_t number_of_elements = std::strtol(argv[1], nullptr, 10);
    auto tup = SOA<sized_int<1>, sized_int<2>, sized_int<4>, sized_int<8>>(
        number_of_elements);
    std::cout << "size = " << tup.get_size() << "\n";
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::uniform_int_distribution<uint64_t> dis_int(
    //     0, std::numeric_limits<uint32_t>::max());
    // for (uint64_t i = 0; i < number_of_elements; i++) {
    //   tup.set(i, {dis_int(g), dis_int(g), dis_int(g), dis_int(g)});
    // }
    for (uint64_t i = 0; i < number_of_elements; i++) {
      tup.get(i) = std::make_tuple(i, 2 * i, 3 * i, 4 * i);
    }
    uint64_t start = 0;
    uint64_t end = 0;

    start = get_time();
    size_t sum_first = 0;
    tup.map_range<0>([&sum_first](auto x) { sum_first += x; });
    end = get_time();
    std::cout << "First time was " << end - start << "  sum was ";
    std::cout << sum_first << "\n";

    start = get_time();
    size_t sum_second = 0;
    tup.map_range<1>([&sum_second](auto x) { sum_second += x; });
    end = get_time();
    std::cout << "Second time was " << end - start << "  sum was ";
    std::cout << sum_second << "\n";

    start = get_time();
    size_t sum_third = 0;
    tup.map_range<2>([&sum_third](auto x) { sum_third += x; });
    end = get_time();
    std::cout << "Third time was " << end - start << "  sum was ";
    std::cout << sum_third << "\n";

    start = get_time();
    size_t sum_forth = 0;
    tup.map_range<3>([&sum_forth](auto x) { sum_forth += x; });
    end = get_time();
    std::cout << "Forth time was " << end - start << "  sum was ";
    std::cout << sum_forth << "\n";

    start = get_time();
    size_t sum_first_2 = 0;
    tup.map_range<0, 1>(
        [&sum_first_2](auto x, auto y) { sum_first_2 += x + y; });
    end = get_time();
    std::cout << "First 2 time was " << end - start << "  sum was ";
    std::cout << sum_first_2 << "\n";

    start = get_time();
    size_t sum_second_2 = 0;
    tup.map_range<2, 3>(
        [&sum_second_2](auto x, auto y) { sum_second_2 += x + y; });
    end = get_time();
    std::cout << "Second 2 time was " << end - start << "  sum was ";
    std::cout << sum_second_2 << "\n";

    start = get_time();
    size_t sum_all = 0;
    tup.map_range([&sum_all](auto... args) { sum_all += (0 + ... + args); });
    end = get_time();
    std::cout << "All time was " << end - start << "  sum was ";
    std::cout << sum_all << "\n";
  }

  if (argc > 1 && (flag & 4)) {
    std::cout << "\nAOS<uint8_t, uint16_t, uint32_t, uint64_t>\n";
    uint64_t number_of_elements = std::strtol(argv[1], nullptr, 10);
    auto tup = AOS<uint8_t, uint16_t, uint32_t, uint64_t>(number_of_elements);
    std::cout << "size = " << tup.get_size() << "\n";
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::uniform_int_distribution<uint64_t> dis_int(
    //     0, std::numeric_limits<uint32_t>::max());
    // for (uint64_t i = 0; i < number_of_elements; i++) {
    //   tup.set(i, {dis_int(g), dis_int(g), dis_int(g), dis_int(g)});
    // }
    for (uint64_t i = 0; i < number_of_elements; i++) {
      tup.get<0, 1, 2, 3>(i) = std::make_tuple(i, 2 * i, 3 * i, 4 * i);
    }
    uint64_t start = 0;
    uint64_t end = 0;

    start = get_time();
    size_t sum_first = 0;
    tup.map_range<0>([&sum_first](auto x) { sum_first += x; });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_first << "\n";

    start = get_time();
    size_t sum_second = 0;
    tup.map_range<1>([&sum_second](auto x) { sum_second += x; });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_second << "\n";

    start = get_time();
    size_t sum_third = 0;
    tup.map_range<2>([&sum_third](auto x) { sum_third += x; });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_third << "\n";

    start = get_time();
    size_t sum_forth = 0;
    tup.map_range<3>([&sum_forth](auto x) { sum_forth += x; });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_forth << "\n";

    start = get_time();
    size_t sum_first_2 = 0;
    tup.map_range<0, 1>(
        [&sum_first_2](auto x, auto y) { sum_first_2 += x + y; });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_first_2 << "\n";

    start = get_time();
    size_t sum_second_2 = 0;
    tup.map_range<2, 3>(
        [&sum_second_2](auto x, auto y) { sum_second_2 += x + y; });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_second_2 << "\n";

    start = get_time();
    size_t sum_all = 0;
    tup.map_range([&sum_all](auto... args) { sum_all += (0 + ... + args); });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_all << "\n";
  }

  if (argc > 1 && (flag & 8)) {
    std::cout
        << "\nSOA<sized_int<3>, sized_int<5>, sized_int<6>, sized_int<7>>\n";
    uint64_t number_of_elements = std::strtol(argv[1], nullptr, 10);
    auto tup = SOA<sized_int<3>, sized_int<5>, sized_int<6>, sized_int<7>>(
        number_of_elements);
    std::cout << "size = " << tup.get_size() << "\n";
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::uniform_int_distribution<uint64_t> dis_int(
    //     0, std::numeric_limits<uint32_t>::max());
    // for (uint64_t i = 0; i < number_of_elements; i++) {
    //   tup.set(i, {dis_int(g), dis_int(g), dis_int(g), dis_int(g)});
    // }
    for (uint64_t i = 0; i < number_of_elements; i++) {
      tup.get(i) = std::make_tuple(i, 2 * i, 3 * i, 4 * i);
    }
    uint64_t start = 0;
    uint64_t end = 0;

    start = get_time();
    size_t sum_first = 0;
    tup.map_range<0>([&sum_first](auto x) { sum_first += x; });
    end = get_time();
    std::cout << "First time was " << end - start << "  sum was ";
    std::cout << sum_first << "\n";

    start = get_time();
    size_t sum_second = 0;
    tup.map_range<1>([&sum_second](auto x) { sum_second += x; });
    end = get_time();
    std::cout << "Second time was " << end - start << "  sum was ";
    std::cout << sum_second << "\n";

    start = get_time();
    size_t sum_third = 0;
    tup.map_range<2>([&sum_third](auto x) { sum_third += x; });
    end = get_time();
    std::cout << "Third time was " << end - start << "  sum was ";
    std::cout << sum_third << "\n";

    start = get_time();
    size_t sum_forth = 0;
    tup.map_range<3>([&sum_forth](auto x) { sum_forth += x; });
    end = get_time();
    std::cout << "Forth time was " << end - start << "  sum was ";
    std::cout << sum_forth << "\n";

    start = get_time();
    size_t sum_first_2 = 0;
    tup.map_range<0, 1>(
        [&sum_first_2](auto x, auto y) { sum_first_2 += x + y; });
    end = get_time();
    std::cout << "First 2 time was " << end - start << "  sum was ";
    std::cout << sum_first_2 << "\n";

    start = get_time();
    size_t sum_second_2 = 0;
    tup.map_range<2, 3>(
        [&sum_second_2](auto x, auto y) { sum_second_2 += x + y; });
    end = get_time();
    std::cout << "Second 2 time was " << end - start << "  sum was ";
    std::cout << sum_second_2 << "\n";

    start = get_time();
    size_t sum_all = 0;
    tup.map_range([&sum_all](auto... args) { sum_all += (0 + ... + args); });
    end = get_time();
    std::cout << "All time was " << end - start << "  sum was ";
    std::cout << sum_all << "\n";
  }

  return 0;
}