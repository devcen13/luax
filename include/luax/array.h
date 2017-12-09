#pragma once

#ifndef _LUAX_ARRAY_H_
#define _LUAX_ARRAY_H_


#include <luax/base.h>
#include <luax/vm.h>
#include <luax/stack.h>
#include <luax/function.h>


namespace luax {
namespace array {


constexpr int default_size = 4;

inline ::luax::stack::handler create (::luax::VM vm, int sz = default_size) {
  lua_createtable(vm, sz, 0);
  return ::luax::stack::handler(vm);
}

inline ::luax::stack::handler create (::luax::VM vm, const char *name, int sz = default_size) {
  ::luax::array::create(vm, sz);
  lua_setfield(vm, -2, name);
  lua_getfield(vm, -1, name);
  return ::luax::stack::handler(vm);
}

template <typename T>
inline T get (::luax::VM vm, int idx) {
  lua_rawgeti(vm, 1, idx);
  auto res = ::luax::stack::get<T>(vm, -1);
  ::luax::stack::pop(vm);
  return res;
}

template <typename T>
inline void push (::luax::VM vm, int idx, const T &value) {
  ::luax::stack::push(vm, value);
  lua_rawseti(vm, -2, idx);
}


// Current VM realization

inline ::luax::stack::handler create (int sz = default_size) {
  return create(luax_current_vm, sz);
}

inline ::luax::stack::handler create (const char *name, int sz = default_size) {
  return create(luax_current_vm, name, sz);
}

template <typename T>
inline T get (int idx) {
  return get(luax_current_vm, idx);
}

template <typename T>
inline void push (int idx, const T &value) {
  return push(luax_current_vm, idx, value);
}

} // namespace array
} // namespace luax


#endif  // _LUAX_ARRAY_H_
