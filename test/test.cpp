#include "whl.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <optional>
#include <algorithm>
#include <array>
#include <deque>

int main() {
  {
    using namespace whl::literals;
    auto sz = 0_sz;
    whl::println("size of size_t: ", sizeof(sz));
  }

  std::vector vec = {0, 4, 5};
  for (auto[i, v] : whl::with_index(vec)) {
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
      whl::str::join(whl::str::split<std::deque<std::string>>("int float double", " "), "__")
  );
  std::vector<const char *> v = {"(", "_", ")"};
  whl::println(whl::str::join(v, "O"));
  whl::println(whl::str::join(v.begin(), v.end(), "|"));
  whl::println(whl::str::join(std::array{"a", "b", "c"}, "-"));

  std::vector<std::string> ss = {"aa", "bb", "cc", "dd"};
  ss | whl::op::tap([](auto &&it) {
    whl::print(it, ' ');
  }) | whl::op::tap_indexed([](auto &&i, auto &&v) {
    whl::print(i, ' ', v, ' ');
  }) | whl::op::filter([](auto &&it) {
    return it != "aa";
  }) | whl::op::map<std::deque<std::int32_t>>([](auto &&it) {
    return it[0] + it[1];
  }) | whl::op::print();

  whl::println();
  whl::str::split("int float double", " ") | whl::op::print();

  return 0;
}
