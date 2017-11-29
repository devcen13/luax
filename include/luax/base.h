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

#define LUAX_HANDLER LUAX_NODISCARD struct


namespace luax {

using VM = lua_State*;
using LuaUserPointer = void*;

constexpr int self = 1;

} // namespace luax


#define _LUAX_CONCAT(_1, _2) _1##_2
#define _LUAX_HANDLER_ID _LUAX_CONCAT(_h_, __LINE__)

#define luax_use(_Handler) auto _LUAX_HANDLER_ID = _Handler
#define luax_using(_Handler) \
  for (luax_use(_Handler), bool _valid = _LUAX_HANDLER_ID.valid(); _valid; _valid = false)

#undef _LUAX_HANDLER_ID
#undef _LUAX_CONCAT


#endif  // _LUAX_BASE_H_
