#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <list>
#include <optional>
#include <set>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <whl.hpp>

TEST_CASE("literals") {
  using namespace whl::literals;
  REQUIRE(sizeof(0_sz) == sizeof(std::size_t));
}

TEST_CASE("with_index") {
  auto vec = std::vector{1, 2, 3};
  for (auto [i, v] : whl::with_index(vec)) {
    REQUIRE(i + 1 == v);
  }
}

TEST_CASE("foreach_indexed") {
  auto vec = std::vector{1, 2, 3};
  whl::foreach_indexed(vec, [](auto &&i, auto &&v) {
    REQUIRE(i + 1 == v);
  });
  whl::foreach_indexed(vec.begin(), vec.end(), [](auto &&i, auto &&v) {
    REQUIRE(i + 1 == v);
  });
}

TEST_CASE("string upper & lower") {
  auto str = std::string{"lowercase_and_UPPERCASE"};
  whl::str::toupper_inplace(str);
  REQUIRE(str == "LOWERCASE_AND_UPPERCASE");
  REQUIRE(whl::str::tolower(str) == "lowercase_and_uppercase");
  REQUIRE(whl::str::tolower(str.begin() + 1, str.end() - 1) == "owercase_and_uppercas");
}

TEST_CASE("split string") {
  auto res = whl::str::split("int float double", " ") | whl::op::to<std::vector>();
  REQUIRE(res.size() == 3);
  REQUIRE(res[0] == "int");
  REQUIRE(res[1] == "float");
  REQUIRE(res[2] == "double");
}

TEST_CASE("others") {
  std::vector<const char *> v = {"(", "_", ")"};
  whl::println(whl::str::join(v, "O"));
  whl::println(whl::str::join(v.begin(), v.end(), "|"));
  whl::println(whl::str::join(std::array{"a", "b", "c"}, "-"));

  std::vector<std::string>{"ab", "cd", "ef", "gh", "ij", "kl", "mn"}
      | whl::op::foreach ([](auto &&it) { whl::print(it, ' '); })
      | whl::op::foreach_indexed([](auto &&i, auto &&v) { whl::print(i, ' ', v, ' '); })
      | whl::op::take(6)
      | whl::op::drop(1)
      | whl::op::println()
      | whl::op::concat(std::array{"op", "qr", "st"})
      | whl::op::println()
      | whl::op::filter(whl::func::not_equal_with("cd"))
      | whl::op::println()
      | whl::op::flat_map([](auto &&it) { return it + "-"; })
      | whl::op::println()
      | whl::op::map([](auto &&it) { return static_cast<int32_t>(it); })
      | whl::op::println()
      | whl::op::chunk(3)
      | whl::op::println();

  whl::range('a', 'd')
      | whl::op::zip(std::vector{10, 100, 1000})
      | whl::op::println()
      | whl::op::map([](auto &&it) { return std::string{it.first} + std::to_string(it.second); })
      | whl::op::map([](auto &&it) { return it.length(); })
      | whl::op::average<double>()
      | whl::op::println();

  std::vector{1, 3, 5, 7}
      | whl::op::reduce(whl::func::minus)
      | whl::op::println();
  std::vector{2, 4, 6, 8}
      | whl::op::fold(2, whl::func::multiply)
      | whl::op::println();
  std::vector{1, 2, 3, 4}
      | whl::op::all(whl::func::great_than(0))
      | whl::op::println();

  std::vector{"a", "b", "c"}
      | whl::op::zip(std::vector{0, 1, 2})
      | whl::op::unzip()
      | whl::op::println();

  whl::generate(1, [](auto &&it) { return it * 2; })
      | whl::op::take(20)
      | whl::op::println();

  for (auto &&it : whl::range(0, 10)) {
    whl::printf("{} ", it);
  }
  whl::println();

  whl::str::split("int float double", " ") | whl::op::println();
  whl::println(std::vector{std::vector{0, 1}, std::vector{2, 3}, std::vector{4, 5}});
  whl::println(std::make_tuple(1, 'c', "str", std::make_pair(1, 2), std::make_tuple("a", 'b', 'c'), std::array{1, 2, 3, 4}));
}
