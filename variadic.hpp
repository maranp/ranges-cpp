/*
 * variadic.hpp
 *
 *  Created on: 08-Aug-2017
 *      Author: maran
 */

#ifndef VARIADIC_HPP_
#define VARIADIC_HPP_

#include <tuple>
#include <utility>

// paired_args called one time to get a lambda that takes a pair
// to call a function which takes 2 args whose value are
// members of the pair
template <typename Res, typename Arg1, typename Arg2>
auto paired_args(Res (*func)(Arg1, Arg2)) {
  return [func](std::pair<Arg1 const &, Arg2 const &> arg) {
    return func(arg.first, arg.second);
  };
}

template <typename Func, typename Args, std::size_t... index>
auto tupled_args_impl(Func func, Args const & args, std::index_sequence<index...>) {
  return func(std::get<index>(args)...);
}

template <typename Res, typename... Args>
auto tupled_args(Res (*func)(Args...)) {
  return [func](std::tuple<Args const &...> args) {
    return tupled_args_impl(func, args, std::make_index_sequence<sizeof...(Args)>{});
  };
}

template <std::size_t n, typename T1, typename... Ts>
struct get_type {
  // nth argument in the <T1, Ts...> list is
  // (n-1)th argument in the <Ts...> list
  // note that counting starts from zero.
  // ie, 0th argument is actually first in the list
  using type = typename get_type<n - 1, Ts...>::type;
};

template <typename T1, typename... Ts>
struct get_type<0, T1, Ts...> {
  using type = T1;
};

// to get the type of nth argument from a list of argument types
// define an alias that queries the member type of a structure that
// takes all argument types as its template parameter
template <std::size_t n, typename... Ts>
using get_type_t = typename get_type<n, Ts...>::type;

template <std::size_t n, typename T1, typename... Ts>
struct Get_value {
  auto & operator()(T1 &&, Ts &&... ts) {
    return Get_value<n - 1, Ts...>()(std::forward<Ts>(ts)...);
  }
};

template <typename T1, typename... Ts>
struct Get_value<0, T1, Ts...> {
  auto & operator()(T1 && t, Ts &&...) {
    return t;
  }
};

// to get nth argument's value from variadic arguments
// create a structure that takes the n arguments type as template parameters
// define a operator() which recursively strips the first argument
// until the nth parameter becomes the first and return the first args's value.
template <std::size_t n, typename T1, typename... Ts>
auto & get_value(T1 && t, Ts &&... ts) {
  return Get_value<n, T1, Ts...>()(std::forward<T1>(t), std::forward<Ts>(ts)...);
}

#endif /* VARIADIC_HPP_ */
