#pragma once

#ifndef _LUAX_FUNCTION_H_
#define _LUAX_FUNCTION_H_


#include <utility>

#include <luax\base.h>
#include <luax\vm.h>
#include <luax\stack.h>


namespace luax {

struct function {
  LuaUserPointer native;
  lua_CFunction  closure;

  template <typename Ret, typename ...Args>
  struct builder {
    using Native = Ret(*)(Args...);

    template <size_t ...Is>
    static function build (Native func, ::std::index_sequence<Is...>) {
      return {
        reinterpret_cast<LuaUserPointer>(func),
        [](VM vm)->int {
          static_assert(sizeof(Native) == sizeof(void*), "No support for 'fat' pointers");
          auto f = reinterpret_cast<Native>(::luax::stack::get<LuaUserPointer>(vm, lua_upvalueindex(1)));
          Ret ret = f(::luax::stack::get<Args>(vm, (Is + 1))...);
          ::luax::stack::push(vm, ret);
          return 1;
        }
      };
    }

    static function build (Native func) {
      return build(func, ::std::index_sequence_for<Args...>());
    }
  };

  template <typename ...Args>
  struct builder<void, Args...> {
    using Native = void(*)(Args...);

    template <size_t ...Is>
    static function build (Native func, ::std::index_sequence<Is...>) {
      return {
        reinterpret_cast<LuaUserPointer>(func),
        [](VM vm)->int {
          static_assert(sizeof(Native) == sizeof(void*), "No support for 'fat' pointers");
          auto f = reinterpret_cast<Native>(::luax::stack::get<LuaUserPointer>(vm, lua_upvalueindex(1)));
          f(::luax::stack::get<Args>(vm, (Is + 1))...);
          return 1;
        }
      };
    }

    static function build (Native func) {
      return build(func, ::std::index_sequence_for<Args...>());
    }
  };

  template <typename Ret, typename ...Args>
  static function build (Ret(*func)(Args...)) {
    return builder<Ret, Args...>::build(func);
  }
};


template <typename ...Args>
inline void pcall (::luax::VM vm, const ::luax::Params<Args...> &args) {
  ::luax::stack::push(vm, args);
  lua_pcall(vm, (int)args.size, 1, 0);
}


// Current VM realization

template <typename ...Args>
inline void pcall (const ::luax::Params<Args...> &args) {
  pcall(luax_current_vm, args);
}

} // namespace luax


LUAX_DECLARE_STACK_VALUE(::luax::function) {
  LUAX_PUSH {
    ::luax::stack::push(_vm_, value.native);
    lua_pushcclosure(_vm_, value.closure, 1);
  }
};


#endif  // _LUAX_FUNCTION_H_
