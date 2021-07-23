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

#ifndef WHEEL_WHL_META_HPP
#define WHEEL_WHL_META_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <type_traits>

namespace whl::meta {

template<auto Val, typename T = decltype(Val)>
struct v {
  using type = T;
  static inline constexpr T value = Val;
};

using t = v<true>;

using f = v<false>;

template<char Val>
using ch = v<Val>;

template<std::int32_t Val>
using i32 = v<Val>;

template<std::uint32_t Val>
using u32 = v<Val>;

struct nil : f {
  using type = nil;
};

template<typename Car, typename Cdr>
struct cons {
  using type = cons<Car, Cdr>;
  using car = Car;
  using cdr = Cdr;
};

template<typename Cons>
using car = typename Cons::car;

template<typename Cons>
using cdr = typename Cons::cdr;

template<typename Cons, typename Arg>
using rplaca = cons<Arg, cdr<Cons>>;

template<typename Cons, typename Arg>
using rplacd = cons<car<Cons>, Arg>;

template<typename Cons>
using cddr = cdr<cdr<Cons>>;

template<typename Cons>
using cdddr = cdr<cddr<Cons>>;

template<typename Cons>
using cddddr = cdr<cdddr<Cons>>;

template<typename Cons>
using cadr = car<cdr<Cons>>;

template<typename Cons>
using caddr = car<cddr<Cons>>;

template<typename Cons>
using cadddr = car<cdddr<Cons>>;

template<typename Cons>
using head = car<Cons>;

template<typename Cons>
using tail = cdr<Cons>;

template<typename Cons>
using rest = cdr<Cons>;

template<typename Cons>
using first = car<Cons>;

template<typename Cons>
using second = cadr<Cons>;

template<typename Cons>
using third = caddr<Cons>;

template<typename Cons>
using fourth = cadddr<Cons>;

template<typename Cons>
using fifth = cadddr<cdr<Cons>>;

template<typename Cons>
using sixth = cadddr<cddr<Cons>>;

template<typename Cons>
using seventh = cadddr<cdddr<Cons>>;

template<typename Cons>
using eighth = cadddr<cddddr<Cons>>;

template<typename Cons>
using nineth = cadddr<cddddr<cdr<Cons>>>;

template<typename Cons>
using tenth = cadddr<cddddr<cddr<Cons>>>;

namespace detail {

template<typename Bool>
using enable_if = std::enable_if_t<Bool::value>;

template<typename Arg, typename If, typename Else>
struct if_impl {
  using type = Else;
};

template<typename If, typename Else>
struct if_impl<t, If, Else> {
  using type = If;
};

template<typename Arg, typename If, typename... Else>
struct cond_impl {
  using type = typename cond_impl<Else...>::type;
};

template<typename If, typename... Else>
struct cond_impl<t, If, Else...> {
  using type = If;
};

template<typename L, typename R>
struct eql_impl {
  using type = f;
};

template<typename T>
struct eql_impl<T, T> {
  using type = t;
};

template<typename Car, typename... Cdr>
struct list_impl {
  using type = cons<Car, typename list_impl<Cdr...>::type>;
};

template<typename Car>
struct list_impl<Car> {
  using type = cons<Car, nil>;
};

template<typename>
struct listp_impl {
  using type = f;
};

template<typename Car, typename Cdr>
struct listp_impl<cons<Car, Cdr>> {
  using type = t;
};

template<>
struct listp_impl<nil> {
  using type = t;
};

template<typename>
struct consp_impl {
  using type = f;
};

template<typename Car, typename Cdr>
struct consp_impl<cons<Car, Cdr>> {
  using type = t;
};

} // namespace detail

template<typename Arg, typename If, typename Else>
using if_ = typename detail::if_impl<Arg, If, Else>::type;

template<typename Arg, typename If, typename... Else>
using cond = typename detail::cond_impl<Arg, If, Else...>::type;

template<typename L, typename R>
using eql = typename detail::eql_impl<L, R>::type;

template<typename Arg>
using type_of = typename Arg::type;

template<typename Arg, typename T>
using typep = eql<type_of<Arg>, T>;

template<typename L, typename R>
using plus = v<L::value + R::value>;

template<typename L, typename R>
using minus = v<L::value - R::value>;

template<typename L, typename R>
using mul = v<L::value * R::value>;

template<typename L, typename R>
using div = v<L::value / R::value>;

template<typename L, typename R>
using rem = v<L::value % R::value>;

template<typename L, typename R>
using mod = rem<plus<rem<L, R>, R>, R>;

template<typename L, typename R>
using gt = v<(L::value > R::value)>;

template<typename L, typename R>
using lt = v<(L::value < R::value)>;

template<typename L, typename R>
using ge = v<L::value >= R::value>;

template<typename L, typename R>
using le = v<L::value <= R::value>;

template<typename L, typename R>
using ne = v<L::value != R::value>;

template<typename L, typename R>
using eq = v<L::value == R::value>;

template<typename L, typename R>
using equal = v<L::value == R::value>;

template<typename L, typename R>
using min = if_<le<L, R>, L, R>;

template<typename L, typename R>
using max = if_<ge<L, R>, L, R>;

template<typename V>
using neg = v<-V::value>;

template<typename V>
using abs = if_<ge<V, v<0>>, V, neg<V>>;

template<typename V>
using succ = v<V::value + 1>;

template<typename V>
using pred = v<V::value - 1>;

template<typename L, typename R>
using and_ = v<L::value && R::value>;

template<typename L, typename R>
using or_ = v<L::value || R::value>;

template<typename V>
using not_ = v<!V::value>;

template<typename V>
using zerop = v<V::value == 0>;

template<typename V>
using chr = ch<V::value>;

template<typename T>
using null = eql<T, nil>;

template<typename Car, typename... Cdr>
using list = typename detail::list_impl<Car, Cdr...>::type;

template<auto... Vals>
using listv = typename detail::list_impl<v<Vals>...>::type;

template<char... Chars>
using str = typename detail::list_impl<ch<Chars>...>::type;

template<typename T>
using listp = typename detail::listp_impl<T>::type;

template<typename T>
using consp = typename detail::consp_impl<T>::type;

namespace detail {

template<typename Size, typename Init, typename = void>
struct make_list_impl {
  using type = cons<Init, typename make_list_impl<pred<Size>, Init>::type>;
};

template<typename Size, typename Init>
struct make_list_impl<Size, Init, enable_if<zerop<Size>>> {
  using type = nil;
};

template<typename L, typename R, typename = void>
struct range_impl {
  using type = cons<L, typename range_impl<succ<L>, R>::type>;
};

template<typename L, typename R>
struct range_impl<L, R, enable_if<equal<L, R>>> {
  using type = nil;
};

template<typename List>
struct length_impl {
  using type = succ<typename length_impl<cdr<List>>::type>;
};

template<>
struct length_impl<nil> {
  using type = u32<0>;
};

template<typename N, typename List, typename = void>
struct nth_impl {
  using type = typename nth_impl<pred<N>, cdr<List>>::type;
};

template<typename T, typename List>
struct nth_impl<T, List, enable_if<zerop<T>>> {
  using type = car<List>;
};

template<typename N, typename List, typename = void>
struct nthcdr_impl {
  using type = typename nthcdr_impl<pred<N>, cdr<List>>::type;
};

template<typename T, typename List>
struct nthcdr_impl<T, List, enable_if<zerop<T>>> {
  using type = List;
};

template<typename List, typename = void>
struct last_impl {
  using type = typename last_impl<cdr<List>>::type;
};

template<typename List>
struct last_impl<List, enable_if<null<cdr<List>>>> {
  using type = List;
};

template<typename List, typename = void>
struct init_impl {
  using type = cons<car<List>, typename init_impl<cdr<List>>::type>;
};

template<typename List>
struct init_impl<List, enable_if<null<cdr<List>>>> {
  using type = nil;
};

template<typename List, typename... Lists>
struct append_impl {
  using type = cons<car<List>, typename append_impl<cdr<List>, Lists...>::type>;
};

template<typename... Lists>
struct append_impl<nil, Lists...> {
  using type = typename append_impl<Lists...>::type;
};

template<typename List>
struct append_impl<nil, List> {
  using type = List;
};

template<typename A, typename B>
struct reverse_impl {
  using type = typename reverse_impl<cdr<A>, cons<car<A>, B>>::type;
};

template<typename B>
struct reverse_impl<nil, B> {
  using type = B;
};

template<template<typename...> typename Fn, typename... Lists>
struct mapcar_impl {
  using type = cons<Fn<car<Lists>...>, typename mapcar_impl<Fn, cdr<Lists>...>::type>;
};

template<template<typename...> typename Fn, typename... Lists>
struct mapcar_impl<Fn, nil, Lists...> {
  using type = nil;
};

template<template<typename...> typename Fn, typename... Lists>
struct maplist_impl {
  using type = cons<Fn<Lists...>, typename maplist_impl<Fn, cdr<Lists>...>::type>;
};

template<template<typename...> typename Fn, typename... Lists>
struct maplist_impl<Fn, nil, Lists...> {
  using type = nil;
};

template<template<typename...> typename Pred, typename List, typename = void>
struct filter_impl {
  using type = typename filter_impl<Pred, cdr<List>>::type;
};

template<template<typename...> typename Pred, typename List>
struct filter_impl<Pred, List, enable_if<Pred<car<List>>>> {
  using type = cons<car<List>, typename filter_impl<Pred, cdr<List>>::type>;
};

template<template<typename...> typename Pred>
struct filter_impl<Pred, nil> {
  using type = nil;
};

template<template<typename...> typename Op, typename Init, typename List>
struct fold_left_impl {
  using type = typename fold_left_impl<Op, Op<Init, car<List>>, cdr<List>>::type;
};

template<template<typename...> typename Op, typename Init>
struct fold_left_impl<Op, Init, nil> {
  using type = Init;
};

template<template<typename...> typename Op, typename Init, typename List>
struct fold_right_impl {
  using type = Op<car<List>, typename fold_right_impl<Op, Init, cdr<List>>::type>;
};

template<template<typename...> typename Op, typename Init>
struct fold_right_impl<Op, Init, nil> {
  using type = Init;
};

template<template<typename...> typename Op, typename List, typename Init>
struct reduce_impl {
  using type = typename fold_left_impl<Op, Init, List>::type;
};

template<template<typename...> typename Op, typename List>
struct reduce_impl<Op, List, void> {
  using type = typename fold_left_impl<Op, car<List>, cdr<List>>::type;
};

template<template<typename...> typename Pred, typename List, typename = void>
struct span_impl {
  using type = cons<nil, List>;
};

template<template<typename...> typename Pred, typename List>
struct span_impl<Pred, List, enable_if<Pred<car<List>>>> {
  using spanned_cdr = typename span_impl<Pred, cdr<List>>::type;
  using type = cons<cons<car<List>, car<spanned_cdr>>, cdr<spanned_cdr>>;
};

template<typename List, template<typename...> typename Eq>
struct group_impl {
  template<typename Val>
  using eq_car = Eq<car<List>, Val>;
  using spanned_cdr = typename span_impl<eq_car, cdr<List>>::type;
  using type = cons<cons<car<List>, car<spanned_cdr>>, typename group_impl<cdr<spanned_cdr>, Eq>::type>;
};

template<template<typename...> typename Eq>
struct group_impl<nil, Eq> {
  using type = nil;
};

} // namespace detail

template<typename Size, typename Init = nil>
using make_list = typename detail::make_list_impl<Size, Init>::type;

template<typename L, typename R>
using range = typename detail::range_impl<L, R>::type;

template<typename List>
using length = typename detail::length_impl<List>::type;

template<typename List>
using len = typename detail::length_impl<List>::type;

template<typename N, typename List>
using nth = typename detail::nth_impl<N, List>::type;

template<typename N, typename List>
using nthcdr = typename detail::nthcdr_impl<N, List>::type;

template<typename List>
using last = typename detail::last_impl<List>::type;

template<typename List>
using init = typename detail::init_impl<List>::type;

template<typename List, typename... Lists>
using append = typename detail::append_impl<List, Lists...>::type;

template<typename List>
using reverse = typename detail::reverse_impl<List, nil>::type;

template<template<typename...> typename Fn, typename... Lists>
using mapcar = typename detail::mapcar_impl<Fn, Lists...>::type;

template<template<typename...> typename Fn, typename... Lists>
using maplist = typename detail::maplist_impl<Fn, Lists...>::type;

template<template<typename...> typename Fn, typename... Lists>
using map = typename detail::mapcar_impl<Fn, Lists...>::type;

template<template<typename...> typename Pred, typename List>
using filter = typename detail::filter_impl<Pred, List>::type;

template<template<typename...> typename Op, typename Init, typename List>
using fold_left = typename detail::fold_left_impl<Op, Init, List>::type;

template<template<typename...> typename Op, typename Init, typename List>
using fold_right = typename detail::fold_right_impl<Op, Init, List>::type;

template<template<typename...> typename Op, typename Init, typename List>
using fold = typename detail::fold_left_impl<Op, Init, List>::type;

template<template<typename...> typename Op, typename List, typename Init = void>
using reduce = typename detail::reduce_impl<Op, List, Init>::type;

template<typename List, typename Zero = v<0>>
using sum = reduce<plus, List, Zero>;

template<typename List>
using concat = fold<append, nil, List>;

template<template<typename...> typename Fn, typename... Lists>
using concat_map = concat<map<Fn, Lists...>>;

template<typename List>
using flatten = fold<append, nil, List>;

template<template<typename...> typename Fn, typename... Lists>
using flat_map = flatten<map<Fn, Lists...>>;

template<template<typename...> typename Pred, typename List>
using span = typename detail::span_impl<Pred, List>::type;

template<typename List, template<typename...> typename Eq = eql>
using group = typename detail::group_impl<List, Eq>::type;

template<typename List, typename Fn>
inline void dolist(Fn fn) {
  if constexpr (!null<List>::value) {
    fn(car<List>{});
    dolist<cdr<List>>(fn);
  }
}

template<typename List, typename Fn>
inline void foreach (Fn fn) {
  dolist<List>(fn);
}

} // namespace whl::meta

#endif // WHEEL_WHL_META_HPP
