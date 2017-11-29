#pragma once

#ifndef _LUAX_GLOBAL_H_
#define _LUAX_GLOBAL_H_


#include <luax\base.h>
#include <luax\vm.h>
#include <luax\stack.h>
#include <luax\params.h>
#include <luax\function.h>


namespace luax {
namespace global {

inline ::luax::stack::handler get (::luax::VM vm, const char *name) {
  lua_getglobal(vm, name);
  return ::luax::stack::handler(vm);
}

inline void set (::luax::VM vm, const char *name) {
  lua_setglobal(vm, name);
}

template <typename T>
inline void set (::luax::VM vm, const char *name, const T &value) {
  ::luax::stack::push(vm, value);
  ::luax::global::set(vm, name);
}

template <typename ...Args>
inline void pcall (::luax::VM vm, const char *name, const ::luax::Params<Args...> &args) {
  ::luax::global::get(vm, name);
  ::luax::pcall(vm, args);
}


// Current VM realization

inline ::luax::stack::handler get (const char *name) {
  return get(luax_current_vm, name);
}

inline void set (const char *name) {
  set(luax_current_vm, name);
}

template <typename T>
inline void set (const char *name, const T &value) {
  set(luax_current_vm, name, value);
}

template <typename ...Args>
inline void pcall (const char *name, const ::luax::Params<Args...> &args) {
  return pcall(luax_current_vm, name, args);
}


} // namespace global
} // namespace luax


#endif  // _LUAX_GLOBAL_H_
