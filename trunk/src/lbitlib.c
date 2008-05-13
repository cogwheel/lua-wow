/* Bitwise operations library */
/* Reuben Thomas   nov00-08dec06 */

#define LUA_LIB

#include "lauxlib.h"
#include "lualib.h"
#include "lua.h"

#include <limits.h>

#define BIT_WIDTH 32		/* Guarantee specific bit width */
#define MAX_BIT (1 << BIT_WIDTH) - 1
#if USHRT_MAX == MAX_BIT
#	define BIT_TYPE short
#elif UINT_MAX == MAX_BIT
#	define BIT_TYPE int
#elif ULONG_MAX == MAX_BIT
#	define BIT_TYPE long
#elif ULLONG_MAX == MAX_BIT
#	define BIT_TYPE long long
#else
#	error "No integer type found with correct bit width"
#endif

typedef BIT_TYPE Integer;
typedef unsigned BIT_TYPE UInteger;

#define TDYADIC(name, op, type1, type2) \
  static int bit_ ## name(lua_State* L) { \
    lua_pushnumber(L, \
      (type1)(long long)luaL_checknumber(L, 1) op (type2)(long long)luaL_checknumber(L, 2)); \
    return 1; \
  }

#define MONADIC(name, op, type) \
  static int bit_ ## name(lua_State* L) { \
    lua_pushnumber(L, op (type)(long long)luaL_checknumber(L, 1)); \
    return 1; \
  }

#define VARIADIC(name, op, type) \
  static int bit_ ## name(lua_State *L) { \
    int n = lua_gettop(L), i; \
    Integer w = (type)(long long)luaL_checknumber(L, 1); \
    for (i = 2; i <= n; i++) \
      w op (type)(long long)luaL_checknumber(L, i); \
    lua_pushnumber(L, w); \
    return 1; \
  }

MONADIC(bnot,    ~,  Integer)
VARIADIC(band,   &=, Integer)
VARIADIC(bor,    |=, Integer)
VARIADIC(bxor,   ^=, Integer)
TDYADIC(lshift,  <<, Integer, UInteger)
TDYADIC(rshift,  >>, UInteger, UInteger)
TDYADIC(arshift, >>, Integer, UInteger)
TDYADIC(mod,     %,  Integer, Integer)

static const struct luaL_reg bitlib[] = {
  {"bnot",    bit_bnot},
  {"band",    bit_band},
  {"bor",     bit_bor},
  {"bxor",    bit_bxor},
  {"lshift",  bit_lshift},
  {"rshift",  bit_rshift},
  {"arshift", bit_arshift},
  {"mod",     bit_mod},
  {NULL, NULL}
};

LUALIB_API int luaopen_bit (lua_State *L) {
  luaL_register(L, LUA_BITLIBNAME, bitlib);
  return 1;
}
