#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <list>
#include <optional>
#include <utility>
#include <vector>

#include "whl.hpp"

int main() {
  {
    using namespace whl::literals;
    auto sz = 0_sz;
    whl::println("size of size_t: ", sizeof(sz));
  }

  std::vector vec = {0, 4, 5};
  for (auto [i, v] : whl::with_index(vec)) {
    whl::println(i, ": ", v);
  }

  whl::for_each_indexed(vec, [](auto &&i, auto &&val) {
    whl::print(i, val);
  });
  whl::println();

  whl::for_each_indexed(vec.begin(), vec.end(), [](auto &&i, auto &&val) {
    whl::print(i, val);
  });
  whl::println();

  std::string str = "lowercase_and_UPPERCASE";
  whl::str::toupper_inplace(str);
  whl::println(str);
  whl::println(whl::str::tolower(str));
  whl::println(str);
  whl::println(whl::str::toupper("str"));
  whl::println(whl::str::tolower(str.begin() + 1, str.end() - 1));
  whl::for_each_indexed(whl::str::toupper<std::vector<char>>(str.begin() + 1, str.end() - 1), [](auto &&i, auto &&v) {
    whl::print(i, ',', v, ' ');
  });
  whl::println();

  whl::println(
      whl::str::join(whl::str::split(str.begin(), str.end(), "_"), ","), "\n",
      whl::str::join(whl::str::split("int float double", "o"), ','), "\n",
      whl::str::join(whl::str::split<std::list<std::string>>("int float double", "t"), ","), "\n",
      whl::str::join(whl::str::split<std::deque<std::string>>("int float double", " "), "__"));
  std::vector<const char *> v = {"(", "_", ")"};
  whl::println(whl::str::join(v, "O"));
  whl::println(whl::str::join(v.begin(), v.end(), "|"));
  whl::println(whl::str::join(std::array{"a", "b", "c"}, "-"));

  auto &&val = std::vector<std::string>{"aa", "bb", "cc", "dd", "ee", "ff", "gg"}
      | whl::op::tap([](auto &&it) { whl::print(it, ' '); })
      | whl::op::tap_indexed([](auto &&i, auto &&v) { whl::print(i, ' ', v, ' '); })
      | whl::op::take(6)
      | whl::op::drop(1)
      | whl::op::println()
      | whl::op::reverse()
      | whl::op::println()
      | whl::op::sort()
      | whl::op::println()
      | whl::op::filter(whl::func::not_equal_with("bb"))
      | whl::op::println()
      | whl::op::flat_map(whl::func::identity)
      | whl::op::println()
      | whl::op::to<std::deque>()
      | whl::op::map([](auto &&it) { return static_cast<int32_t>(it); })
      | whl::op::println()
      | whl::op::reduce(whl::func::plus);
  whl::println(val);

  std::vector{"a", "b", "c"}
      | whl::op::zip(std::vector{0, 1, 2})
      | whl::op::tap([](auto &&it) { whl::println(it.first, it.second); });

  whl::str::split("int float double", " ") | whl::op::print();
  return 0;
}
