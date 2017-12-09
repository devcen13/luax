#pragma once

#ifndef _LUAX_TABLE_H_
#define _LUAX_TABLE_H_


#include <luax/base.h>
#include <luax/vm.h>
#include <luax/stack.h>
#include <luax/function.h>


namespace luax {
namespace table {


constexpr int default_size = 4;

inline ::luax::stack::handler create (::luax::VM vm, int sz = default_size) {
  lua_createtable(vm, 0, sz);
  return ::luax::stack::handler(vm);
}

inline ::luax::stack::handler create (::luax::VM vm, const char *name, int sz = default_size) {
  ::luax::table::create(vm, sz);
  lua_setfield(vm, -2, name);
  lua_getfield(vm, -1, name);
  return ::luax::stack::handler(vm);
}

template <typename T>
inline T get (::luax::VM vm, const char *name) {
  lua_getfield(vm, -1, name);
  auto res = ::luax::stack::get<T>(vm, -1);
  ::luax::stack::pop(vm);
  return res;
}

template <typename T>
inline void push (::luax::VM vm, const char *name, const T &value) {
  ::luax::stack::push(vm, value);
  lua_setfield(vm, -2, name);
}

template <typename ...Args>
inline void pcall (::luax::VM vm, const char *name, const ::luax::Params<Args...> &args) {
  lua_getfield(vm, -1, name);
  ::luax::pcall(vm, args);
}

template <typename ...Args>
inline void pcall_self (::luax::VM vm, const char *name, const ::luax::Params<Args...> &args) {
  lua_getfield(vm, -1, name);
  ::luax::stack::upvalue(vm, -2);
  ::luax::pcall(vm, args);
}


// Current VM realization

inline ::luax::stack::handler create (int sz = default_size) {
  return create(luax_current_vm, sz);
}

inline ::luax::stack::handler create (const char *name, int sz = default_size) {
  return create(luax_current_vm, name, sz);
}

template <typename T>
inline T get (const char *name) {
  return get(luax_current_vm, name);
}

template <typename T>
inline void push (const char *name, const T &value) {
  return push(luax_current_vm, name, value);
}

template <typename ...Args>
inline void pcall (const char *name, const ::luax::Params<Args...> &args) {
  return pcall(luax_current_vm, name, args);
}

template <typename ...Args>
inline void pcall_self (const char *name, const ::luax::Params<Args...> &args) {
  return pcall_self(luax_current_vm, name, args);
}

} // namespace table
} // namespace luax


#endif  // _LUAX_TABLE_H_
