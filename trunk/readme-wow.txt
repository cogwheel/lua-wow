World of Warcraft version by Matthew Orlando

For questions or comments, please visit
http://luaforge.net/projects/lua-wow/

This version of Lua is configured to match (more or less) the lua environment
of World of Warcraft.

* All 5.0-5.1 compatability options have been disabled except openlib
* The bitlib library is included
* WoW-specific, non-API functions are implemented (see below)
* Global aliases to Lua functions are provided (see below)
* string.format can use argument selection from Lua 4.0. E.g:
    string.format("%2$d, %1$d, %d", 1, 2) == "2, 1, 2"
* Ignores the UTF-8 Byte Order Mark at the beginning of a file
* Max stack size increased to 4096

Note: the Windows binaries included in this download are statically linked for
performance and file size. They may not function as expected when used with
certain third-party packages. Please compile it from source (available at the
above URL).

Version History

v1.10.1
- Added msvcr80.dll to the windows project

v1.10
- Reorganized readme files
- Renamed msvc projects and the names of the exe output
- msvc project includes the Lua icon and version resources for the executables

v1.9
- Added PI
- Global versions of trigonometric functions now correctly use degrees
- strsplit will print an appropriate error message if it overflows the stack
- It should no longer be practically possible to overflow the stack with strjoin

v1.8
- Re-enabled 5.0 openlib compatibility since WoW does not use libs anyway

v1.7
- Changed GCPAUSE to 110 per WoW 2.1
- Eliminated a few miscellaneous compiler warnings

v1.6.1
- Added #define LUA_LIB to lwowlib & lbitlib to fix compilation problems

v1.6
- Updated bitlib to the latest version
- Bitlib now correctly uses 32-bit integers as does WoW
- The global aliases to bitlib functions and table.setn have been removed since
they do not exist in WoW

v1.5.1
- Ignores UTF-8 Byte Order Mark

v1.5
- Added debugstack() (not fully implemented--see below)
- Made some slight optimizations/code simplifications

v1.4
- Added argument selection to string.format

v1.3.1
- Fixed Makefile problem

v1.3
- Added blizzard-specific functions & global aliases (see below)

v1.2
- This package can now be used as a drop-in replacement for the standard lua
source distribution: The MSVC project (including precompiled windows binaries)
is now found in the msvc subdirectory; the rest of the directory structure is
untouched.

v1.1
- Added BitLib. Optimized file size of Luac.exe. Includes test Lua programs that
come with standard Lua distribution.

1.0 - Initial Release



WoW-specific functions

All WoW-specific functions are implemented in the 'wow' package for simplicity's
sake:

wow.strtrim
wow.strsplit
wow.strjoin
wow.strconcat
wow.getglobal
wow.setglobal
wow.debugstack (currently ignores top and bottom counts)

The global versions (and the few that are added to the string table) are aliased
below.



*** Aliases ***

os

date = os.date
time = os.time
difftime = os.difftime

math

abs = math.abs
acos = function (x) return math.deg(math.acos(x)) end
asin = function (x) return math.deg(math.asin(x)) end
atan = function (x) return math.deg(math.atan(x)) end
atan2 = function (x,y) return math.deg(math.atan2(x,y)) end
ceil = math.ceil
cos = function (x) return math.cos(math.rad(x)) end
deg = math.deg
exp = math.exp
floor = math.floor
frexp = math.frexp
ldexp = math.ldexp
log = math.log
log10 = math.log10
max = math.max
min = math.min
mod = math.fmod
PI = math.pi
rad = math.rad
random = math.random
randomseed = math.randomseed
sin = function (x) return math.sin(math.rad(x)) end
sqrt = math.sqrt
tan = function (x) return math.tan(math.rad(x)) end

string

format = string.format
gmatch = string.gmatch
gsub = string.gsub
strbyte = string.byte
strchar = string.char
strfind = string.find
strlen = string.len
strlower = string.lower
strmatch = string.match
strrep = string.rep
strrev = string.reverse
strsub = string.sub
strupper = string.upper

table

foreach = table.foreach
foreachi = table.foreachi
getn = table.getn
sort = table.sort
tinsert = table.insert
tremove = table.remove

wow

strtrim = wow.strtrim
strsplit = wow.strsplit
strjoin = wow.strjoin
strconcat = wow.strconcat
getglobal = wow.getglobal
setglobal = wow.setglobal
debugstack = wow.debugstack
string.trim = wow.strtrim
string.split = wow.strsplit
string.join = wow.strjoin
