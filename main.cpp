#include "aos.hpp"
#include "soa.hpp"

#include <limits>
#include <random>
#include <sys/time.h>
#include <tuple>

static inline uint64_t get_time() {
  struct timeval st {};
  gettimeofday(&st, nullptr);
  return st.tv_sec * 1000000 + st.tv_usec;
}

int main(int32_t argc, char *argv[]) {
  {
    SOA<int>::print_type_details();
    SOA<int, bool>::print_type_details();
    SOA<int, bool, short>::print_type_details();
    SOA<int, short, bool, long>::print_type_details();
    SOA<int, short, bool, char[3]>::print_type_details();
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
    tup.get<0, 1, 2, 3>(9) = std::make_tuple(5, 4, false, 100);
    tup.print_soa<0, 1, 2, 3>();
    std::cout << "\n";

    size_t sum_all = 0;
    tup.map_range<0, 3>([&sum_all](auto x, auto y) { sum_all += x + y; });
    std::cout << sum_all << "\n";

    tup.map_range<0>([](auto &x) { x += 1; });
    tup.print_soa<0, 1, 2, 3>();
  }

  if (argc > 1) {
    std::cout << "\n";
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
    tup.map_range<0, 1, 2, 3>([&sum_all](auto w, auto x, auto y, auto z) {
      sum_all += w + x + y + z;
    });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_all << "\n";
  }

  if (argc > 1) {
    std::cout << "\n";
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
    tup.map_range<0, 1, 2, 3>([&sum_all](auto w, auto x, auto y, auto z) {
      sum_all += w + x + y + z;
    });
    end = get_time();
    std::cout << "time was " << end - start << "  sum was ";
    std::cout << sum_all << "\n";
  }

  return 0;
}