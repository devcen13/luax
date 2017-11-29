#pragma once

#ifndef _LUAX_STACK_VALUE_INL_
#define _LUAX_STACK_VALUE_INL_


LUAX_DECLARE_STACK_VALUE(bool) {
  LUAX_PUSH {
    lua_pushboolean(_vm_, value);
  }
  LUAX_GET(idx) {
    LUAX_REQUIRE_TYPE(boolean, idx)
    return !!lua_toboolean(_vm_, idx);
  }
};

LUAX_DECLARE_STACK_VALUE(std::string) {
  LUAX_PUSH {
    lua_pushstring(_vm_, value.c_str());
  }
  LUAX_GET(idx) {
    LUAX_REQUIRE_TYPE(string, idx)
    return lua_tostring(_vm_, idx);
  }
};

LUAX_DECLARE_STACK_VALUE(const char*) {
  LUAX_PUSH {
    lua_pushstring(_vm_, value);
  }
  LUAX_GET(idx) {
    LUAX_REQUIRE_TYPE(string, idx)
    return lua_tostring(_vm_, idx);
  }
};

LUAX_DECLARE_STACK_VALUE(::luax::LuaUserPointer) {
  LUAX_PUSH {
    lua_pushlightuserdata(_vm_, value);
  }
  LUAX_GET(idx) {
    LUAX_REQUIRE_TYPE(lightuserdata, idx)
    return lua_touserdata(_vm_, idx);
  }
};

LUAX_DECLARE_STACK_VALUE(std::nullptr_t) {
  LUAX_PUSH {
    lua_pushnil(_vm_);
  }
};

LUAX_DECLARE_STACK_VALUE(lua_CFunction) {
  LUAX_PUSH {
    lua_pushcclosure(_vm_, value, 0);
  }
};


template <typename T>
struct ::luax::stack::details::StackValue <
    T, std::enable_if_t<std::is_integral<T>::value>
  > : ::luax::stack::details::Value<T>
{
  static void push (::luax::VM vm, const T &value) {
    lua_pushinteger(vm, value);
  }

  static T    get (::luax::VM vm, int idx, const T &def) {
    LUAX_REQUIRE_TYPE(number, idx)
    return (T)lua_tointeger(vm, idx);
  }
};


template <typename T>
struct ::luax::stack::details::StackValue <
    T, std::enable_if_t<std::is_floating_point<T>::value>
  > : ::luax::stack::details::Value<T>
{
  static void push (::luax::VM vm, const T &value) {
    lua_pushnumber(vm, value);
  }

  static T    get (::luax::VM vm, int idx, const T &def) {
    LUAX_REQUIRE_TYPE(number, idx)
    return (T)lua_tonumber(vm, idx);
  }
};


template<typename T, typename _ = void>
struct _luax_is_map
  : std::false_type { };

template<typename Map>
struct _luax_is_map<Map, std::conditional_t<
                     false,
                     decltype(begin(std::declval<Map>())->first),
                     void
                   >
             > : public std::true_type { };


template<typename T, typename _ = void>
struct _luax_is_iterable
  : std::false_type { };

template<typename T>
struct _luax_is_iterable<T, std::conditional_t<
                          false,
                          decltype(begin(std::declval<T>())),
                          void
                        >
                  > : public std::true_type { };


template <class Map>
struct ::luax::stack::details::StackValue<Map, typename std::enable_if<
                                                          _luax_is_map<Map>::value
                                                        >::type> {
  static void push (::luax::VM vm, const Map &map) {
    ::luax::table::create(vm, map.size());

    for (auto &value : map) {
      ::luax::table::push(value.first.c_str(), value.second);
    }
  }
};

template <class Array>
struct ::luax::stack::details::StackValue<Array, typename std::enable_if<
                                                            !_luax_is_map<Array>::value && _luax_is_iterable<Array>::value
                                                          >::type> {
  static void push (::luax::VM vm, const Array &value) {
    ::luax::array::create(vm, (int)value.size());

    int i = 0;
    for (auto v : value) {
      ::luax::array::push(vm, ++i, v);
    }
  }

  static Array get (::luax::VM _vm_, int index, const Array &def) {
    LUAX_REQUIRE_TABLE(index)

    Array value;
    using T = typename std::decay<typename decltype(*begin(std::declval<Array>()))>::type;
    int size = luaL_getn(_vm_, index);
    value.reserve(size);
    LUAX_USING_STACK_VALUE(index) {
      for (int i = 1; i <= size; i++) {
        value.push_back(::luax::array::get<T>(_vm_, i));
      }
    }
    return value;
  }
};


#endif  // _LUAX_STACK_VALUE_INL_
