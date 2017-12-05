#pragma once

#ifndef _LUAX_BASE_H_
#define _LUAX_BASE_H_


extern "C" {

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

}


#ifdef LUAX_USE_CXX17
#define LUAX_NODISCARD [[nodiscard]]
#else
#define LUAX_NODISCARD
#endif


namespace luax {

using VM = lua_State*;
using LuaUserPointer = void*;

constexpr int self = 1;

namespace base {

/**
  Base handler type for use in luax_use & luax_using macros.
  You do not need to inherit this type. Just make sure, you've implemented it's interface.
*/
LUAX_NODISCARD struct handler {
  handler (const handler &) = delete;
  handler &operator = (const handler &) = delete;

  handler (handler &&) = default;
  handler &operator = (handler &&) = default;

  virtual ~handler () {}
  virtual operator bool () { return false; }
};


} // namespace base
} // namespace luax


#define _LUAX_CONCAT(_1, _2) _1##_2
#define _LUAX_HANDLER_ID() _LUAX_CONCAT(_h_, __LINE__)

#define luax_use(_Handler) auto _LUAX_HANDLER_ID() = _Handler
#define luax_using(_Handler) if (luax_use(_Handler))


#endif  // _LUAX_BASE_H_
