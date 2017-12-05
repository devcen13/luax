#pragma once

#ifndef _LUAX_STACK_H_
#define _LUAX_STACK_H_


#include <string>

#include <luax\base.h>
#include <luax\vm.h>


namespace luax {
namespace stack {


inline void push (VM) { }

inline void pop (VM vm, int depth = 1) {
  lua_pop(vm, depth);
}

inline void remove (VM vm, int idx = -1) {
  lua_remove(vm, idx);
}

inline std::string to_string (VM vm, int idx) {
  size_t len;
  const char *cstr = lua_tolstring(vm, idx, &len);
  std::string str(cstr, len);
  ::luax::stack::pop(vm);
  return str;
}


namespace details {


template <typename T, typename ..._>
struct Value {
  using value_type = T;
}


template <typename T, typename Enable = void>
struct StackValue : Value<T> {
  static void push (VM vm, const T &t);
  static T    get  (VM vm, int idx, const T &def);
};

template <typename T>
struct StackValue<const T&>
  : public StackValue<T> { };

} // namespace details


template <typename First, typename ...Others>
inline void push (VM vm, First first, Others ...others) {
  ::luax::stack::details::StackValue<typename std::decay<First>::type>::push(vm, first);
  push(vm, others...);
}

template <typename T>
inline T get (VM vm, int idx, const T &def = T()) {
  return ::luax::stack::details::StackValue<typename std::decay<T>::type>::get(vm, idx, def);
}



LUAX_NODISCARD struct handler final {
  handler (int count = 1)
    : handler(luax_current_vm, count) { }
  handler (::luax::VM vm, int count = 1) 
    : _vm(vm), _count(count) { }

  handler (const handler &) = delete;
  handler &operator = (const handler &) = delete;

  handler (handler &&other)
    : handler(other._vm, other._count) {
    other._count = 0;
  }
  handler &operator = (handler &&other) {
    _vm = other._vm;
    _count = other._count;
    other._count = 0;
    return *this;
  }

  ~handler () {
    if (valid()) {
      ::luax::stack::pop(_vm, _count);
    }
  }

  bool     valid () const { return _count > 0; }
  operator bool ()  const { return valid(); }

  static handler invalid (::luax::VM vm) { return handler(vm, 0); }
  static handler invalid ()              { return invalid(luax_current_vm); }

private:
  ::luax::VM _vm;
  int        _count;
};


inline ::luax::stack::handler upvalue (::luax::VM vm, int idx) {
  lua_pushvalue(vm, idx);
  return ::luax::stack::handler(vm);
}


// Current VM realization

inline void pop (int depth = 1) {
  pop(luax_current_vm, depth);
}

inline void remove (int idx = -1) {
  remove(luax_current_vm, idx);
}

inline std::string to_string (int idx) {
  return to_string(luax_current_vm, idx);
}

template <typename First, typename ...Others>
inline void push (First first, Others ...others) {
  push(luax_current_vm, first, others...);
}

template <typename T>
inline T get (VM vm, int idx, const T &def = T()) {
  return get(luax_current_vm, idx, def);
}

inline ::luax::stack::handler upvalue (int idx) {
  return upvalue(luax_current_vm, idx);
}

} // namespace stack
} // namespace luax


#define LUAX_STACK_VALUE(...) ::luax::stack::details::StackValue<__VA_ARGS__>

#define LUAX_DECLARE_STACK_VALUE(__VA_ARGS__) \
  template <>                                 \
  struct LUAX_STACK_VALUE(__VA_ARGS__) : ::luax::stack::details::Value<__VA_ARGS__>

#define LUAX_PUSH       static void push (::luax::VM _vm_, typename Value::value_type value)
#define LUAX_GET(_Idx)  static auto get  (::luax::VM _vm_, int _Idx, _Type def) -> typename Value::value_type

#define LUAX_REQUIRE_TYPE(check, _Idx) if (!(lua_is ## check(_vm_, _Idx))) { return def; }
#define LUAX_REQUIRE_TABLE(_Idx)       LUAX_REQUIRE_TYPE(table, _Idx)


#define LUAX_DECLARE_ENUM(_Type, _Min, _Max)     \
  LUAX_DECLARE_STACK_VALUE(_Type) {              \
    LUAX_PUSH {                                  \
      lua_pushinteger(_vm_, value);              \
    }                                            \
    LUAX_GET(idx) {                              \
      LUAX_REQUIRE_TYPE(number, idx)             \
                                                 \
      auto value = lua_tointeger(_vm_, idx);     \
      if (value < _Min || value > _Max) {        \
        return def;                              \
      }                                          \
      return (_Type)value;                       \
    }                                            \
  };


#include <luax\stack_value.inl>


#endif  // _LUAX_STACK_H_
