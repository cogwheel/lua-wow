/*
** This file implements lua API aliases (and a few specialized functions)
** specific to WoW.
*/

#define LUA_LIB

#include <string.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int wow_strtrim(lua_State *L)
{
	int front = 0, back;
	const char *str = luaL_checklstring(L, 1, (size_t *)(&back));
	const char *del = luaL_optstring(L, 2, "\t\n\r ");
	--back;

	while (front <= back && strchr(del, str[front]))
		++front;
	while (back > front && strchr(del, str[back]))
		--back;
	
	lua_pushlstring(L, &str[front], back - front + 1);
	return 1;
}

/* strsplit & strjoin adapted from code by Norganna */
static int wow_strsplit(lua_State *L)
{
	const char *sep = luaL_checkstring(L, 1);
	const char *str = luaL_checkstring(L, 2);
	int limit = luaL_optint(L, 3, 0);
	int count = 0;
	/* Set the stack to a predictable size */
	lua_settop(L, 0);
	/* Initialize the result count */
	/* Tokenize the string */
	if(!limit || limit > 1) {
		const char *end = str;
		while(*end) {
			int issep = 0;
			const char *s = sep;
			for(; *s; ++s) {
				if(*s == *end) {
					issep = 1;
					break;
				}
			}
			if(issep) {
				luaL_checkstack(L, count+1, "too many results");
				lua_pushlstring(L, str, (end-str));
				++count;
				str = end+1;
				if(count == (limit-1)) {
					break;
				}
			}
			++end;
		}
	}
	/* Add the remainder */
	luaL_checkstack(L, count+1, "too many results");
	lua_pushstring(L, str);
	++count;
	/* Return with the number of values found */
	return count;
}

static int wow_strjoin(lua_State *L)
{
	size_t seplen;
	int entries;
	const char *sep = luaL_checklstring(L, 1, &seplen);
	
	/* Guarantee we have 1 stack slot free */
	lua_remove(L, 1);

	entries = lua_gettop(L);

	if (seplen == 0) /* If there's no seperator, then this is the same as a concat */
		lua_concat(L, entries);
	else if (entries == 0) /* If there are no entries then we can't concatenate anything */
		lua_pushstring(L, "");
	else if (entries == 1) /* If there's only one entry, just return it */
		;
	else {
		luaL_Buffer b;
		int i;

		/* Set up buffer to store resulting string */
		luaL_buffinit(L, &b);
		for(i = 1; i <= entries; ++i) {
			/* Push the current entry and add it to the buffer */
			lua_pushvalue(L, i);
			luaL_addvalue(&b);
			/* Add the separator to the buffer */
			if (i < entries) {
				luaL_addlstring(&b, sep, seplen);
			}
		}
		luaL_pushresult(&b);
	}

	return 1;
}

static int wow_strconcat(lua_State *L)
{
	lua_concat(L, lua_gettop(L));
	return 1;
}

static int wow_strreplace(lua_State *L)
{
	const char *subject = luaL_checkstring(L, 1);
	const char *search = luaL_checkstring(L, 2);
	const char *replace = luaL_checkstring(L, 3);

	char *replaced = (char*)calloc(1, 1), *temp = NULL;
	const char *p = subject, *p3 = subject, *p2;
	int  found = 0;
	int count = 0;

	while ( (p = strstr(p, search)) != NULL) {
		found = 1;
		count++;
		temp = realloc(replaced, strlen(replaced) + (p - p3) + strlen(replace) + 1);
		if (temp == NULL) {
			free(replaced);
			luaL_error(L, "Unable to allocate memory");
			return 0;
		}
		replaced = temp;
		strncat(replaced, p - (p - p3), p - p3);
		strcat(replaced, replace);
		p3 = p + strlen(search);
		p += strlen(search);
		p2 = p;
	}

	if (found == 1) {
		if (strlen(p2) > 0) {
			temp = realloc(replaced, strlen(replaced) + strlen(p2) + 1);
			if (temp == NULL) {
				free(replaced);
				luaL_error(L, "Unable to allocate memory");
				return 0;
			}
			replaced = temp;
			strcat(replaced, p2);
		}
	} else {
		temp = realloc(replaced, strlen(subject) + 1);
		if (temp != NULL) {
			replaced = temp;
			strcpy(replaced, subject);
		}
	}

	lua_pushstring(L, replaced);
	lua_pushinteger(L, count);
	return 2;
}

static int wow_getglobal(lua_State *L)
{
	lua_getglobal(L, luaL_checkstring(L, 1));
	return 1;
}

static int wow_setglobal(lua_State *L)
{
	lua_settop(L, 2);
	lua_setglobal(L, luaL_checkstring(L, 1));
	return 0;
}

static int wow_debugstack(lua_State *L)
{
	int start = luaL_optint(L, 1, 1);
	/*int ntop = luaL_optint(L, 2, 12); Ignored for now... May implement more accurate version in the future
	int nbot = luaL_optint(L, 3, 10);*/
	const char *trace;
	size_t tracelen;

	lua_getglobal(L, "debug");
	lua_pushstring(L, "traceback");
	lua_gettable(L, -2);
	lua_pushstring(L, "");
	lua_pushnumber(L, start);

	if (lua_pcall(L, 2, 1, 0) != 0)
		luaL_error(L, "error running `debug.traceback': %s", lua_tostring(L, -1));

	if (!lua_isstring(L, -1))
		luaL_error(L, "function `debug.traceback' did not return a string");

	trace = lua_tolstring(L, -1, &tracelen);

	/* TODO: Implement head/tail code here

	lua_checkstack(L, 1);
	lua_pushstring(L, ret);	*/

	return 1;
}

static const struct luaL_reg wowlib[] = {
	{"strtrim",		wow_strtrim},
	{"strsplit",	wow_strsplit},
	{"strjoin",		wow_strjoin},
	{"strconcat",	wow_strconcat},
	{"strreplace",  wow_strreplace},
	{"getglobal",	wow_getglobal},
	{"setglobal",	wow_setglobal},
	{"debugstack",	wow_debugstack},
	{NULL,			NULL}
};

/* This is a lua chunk that sets up the global aliases to library functions */
static const char *aliases =
	"---------------- os ----------------\n"
	"date = os.date\n"
	"time = os.time\n"
	"difftime = os.difftime\n"

	"---------------- math ----------------\n"
	"abs = math.abs\n"
	"acos = function (x) return math.deg(math.acos(x)) end\n"
	"asin = function (x) return math.deg(math.asin(x)) end\n"
	"atan = function (x) return math.deg(math.atan(x)) end\n"
	"atan2 = function (x,y) return math.deg(math.atan2(x,y)) end\n"
	"ceil = math.ceil\n"
	"cos = function (x) return math.cos(math.rad(x)) end\n"
	"deg = math.deg\n"
	"exp = math.exp\n"
	"floor = math.floor\n"
	"frexp = math.frexp\n"
	"ldexp = math.ldexp\n"
	"log = math.log\n"
	"log10 = math.log10\n"
	"max = math.max\n"
	"min = math.min\n"
	"mod = math.fmod\n"
	"PI = math.pi\n"
	"rad = math.rad\n"
	"random = math.random\n"
	"randomseed = math.randomseed\n"
	"sin = function (x) return math.sin(math.rad(x)) end\n"
	"sqrt = math.sqrt\n"
	"tan = function (x) return math.tan(math.rad(x)) end\n"

	"---------------- string ----------------\n"
	"format = string.format\n"
	"gmatch = string.gmatch\n"
	"gsub = string.gsub\n"
	"strbyte = string.byte\n"
	"strchar = string.char\n"
	"strfind = string.find\n"
	"strlen = string.len\n"
	"strlower = string.lower\n"
	"strmatch = string.match\n"
	"strrep = string.rep\n"
	"strrev = string.reverse\n"
	"strsub = string.sub\n"
	"strupper = string.upper\n"

	"---------------- table ----------------\n"
	"foreach = table.foreach\n"
	"foreachi = table.foreachi\n"
	"getn = table.getn\n"
	"sort = table.sort\n"
	"tinsert = table.insert\n"
	"tremove = table.remove\n"

	"---------------- wow ----------------\n"
	"strtrim = wow.strtrim\n"
	"strsplit = wow.strsplit\n"
	"strjoin = wow.strjoin\n"
	"strconcat = wow.strconcat\n"
	"strreplace = wow.strreplace\n"
	"getglobal = wow.getglobal\n"
	"setglobal = wow.setglobal\n"
	"debugstack = wow.debugstack\n"
	"string.trim = wow.strtrim\n"
	"string.split = wow.strsplit\n"
	"string.join = wow.strjoin\n"
	"string.replace = wow.strreplace\n";


LUALIB_API int luaopen_wow(lua_State *L)
{
	/* Set up new functions */
	luaL_register(L, LUA_WOWLIBNAME, wowlib);
	
	/* Set up aliases */
	luaL_loadstring(L, aliases);
	lua_call(L, 0, 0);

	return 0;
}
