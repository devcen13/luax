#pragma once

#ifndef _LUAX_PARAMS_H_
#define _LUAX_PARAMS_H_


#include <tuple>
#include <utility>

#include <luax/base.h>
#include <luax/vm.h>
#include <luax/stack.h>


namespace luax {

template <typename ...Args>
struct Params {
  Params (Args ...args) {
    _params = std::make_tuple(args...);
  }

  template <size_t I>
  typename std::tuple_element<I, std::tuple<Args...>>::type &&get () noexcept {
    return std::move(std::get<I>(_params));
  }

  std::tuple<Args...> _params;
};

template <typename ...Args>
inline Params<Args...> build_params (Args ...args) {
  return Params<Args...>(args...);
}

} // namespace luax


template <typename ...Args>
struct ::luax::stack::details::StackValue<::luax::Params<Args...>> {
  using ParamsType = ::luax::Params<Args...>;

  template <size_t ...Is>
  static void push (::luax::VM vm, ParamsType params, ::std::index_sequence<Is...>) {
    ::luax::stack::push(vm, params.get<Is>()...);
  }

  static void push (::luax::VM vm, const ParamsType &value) {
    push(vm, value, ::std::index_sequence_for<Args...>());
  }
};


#define luax_args(...) ::luax::build_params(__VA_ARGS__)


#endif  // _LUAX_PARAMS_H_
