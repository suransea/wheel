#include "whl.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <optional>
#include <algorithm>
#include <array>

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

  whl::for_each_indexed<std::size_t>(vec.begin(), vec.end(), [](auto i, auto val) {
    whl::print(i, val);
  });
  whl::println();

  std::string str = "lowercase_and_UPPERCASE";
  whl::str::toupper_inplace(str);
  whl::println(str);
  whl::println(whl::str::tolower(str));
  whl::println(str);
  whl::println(whl::str::toupper<std::string>("str"));

  whl::println(
      whl::str::join(whl::str::split(str, "_"), ","), "\n",
      whl::str::join(whl::str::split<std::string>("int float double", "o"), ','), "\n",
      whl::str::join(whl::str::split<std::string, std::list<std::string>>("int float double", "t"), ","), "\n",
      whl::str::join<std::list<std::string>, std::string>(whl::str::split<std::string, std::list<std::string>>(
          "int float double",
          " "), "__")
  );
  std::vector<const char *> v = {"(", "_", ")"};
  whl::println(whl::str::join<std::vector<const char *>, std::string>(v, "O"));
  return 0;
}
