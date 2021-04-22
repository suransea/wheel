//
// Copyright 2021 sea
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef WHEEL_WHL_COLLECTION_HPP
#define WHEEL_WHL_COLLECTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cctype>
#include <iterator>
#include <utility>

#include "whl/function.hpp"
#include "whl/literals.hpp"

namespace whl {

template<typename Size = std::size_t, typename Iter, typename Fn>
inline void for_each_indexed(Iter first, Iter last, Fn fn) {
  for (Size i{}; first != last; ++first, ++i) {
    fn(i, *first);
  }
}

template<typename C, typename Fn>
inline void for_each_indexed(const C &coll, Fn fn) {
  for_each_indexed<decltype(std::size(coll))>(std::begin(coll), std::end(coll), fn);
}

template<typename C>
struct with_index {
  const C &coll;

  template<typename Iter>
  struct iter_indexed {
    Iter it;
    decltype(std::size(coll)) index{};

    constexpr explicit iter_indexed(Iter it) : it(it) {}

    constexpr auto operator*() const {
      return std::make_pair(index, *it);
    }

    iter_indexed &operator++() {
      ++it;
      ++index;
      return *this;
    }

    bool operator!=(const iter_indexed &iter) const {
      return it != iter.it;
    }
  };

  constexpr explicit with_index(const C &coll) : coll(coll) {}

  constexpr auto begin() {
    return iter_indexed<decltype(std::begin(coll))>(std::begin(coll));
  }

  constexpr auto end() {
    return iter_indexed<decltype(std::end(coll))>(std::end(coll));
  }
};

} // namespace whl

#endif // WHEEL_WHL_COLLECTION_HPP
