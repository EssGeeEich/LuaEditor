/*	Copyright (c) 2015 SGH
**	
**	Permission is granted to use, modify and redistribute this software.
**	Modified versions of this software MUST be marked as such.
**	
**	This software is provided "AS IS". In no event shall
**	the authors or copyright holders be liable for any claim,
**	damages or other liability. The above copyright notice
**	and this permission notice shall be included in all copies
**	or substantial portions of the software.
**	
**	File created on: 18/08/2015
*/


// This file is an edited part of the LuaEngine library.
// It was originally intended to use an edited Lua library,
//  to parse tokens and identifiers.
// The library is now only used to detect Lua errors.

#ifndef LUAENGINE_H
#define LUAENGINE_H
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace LuaEngine {
	struct Cursor {
		std::string::size_type m_line;
		std::string::size_type m_offset;
		
		inline Cursor() : m_line(0), m_offset(0) {}
	};
	struct Location {
		Cursor m_begin;
		Cursor m_end;
	};
	struct ParseResult {
		std::string m_error;
		Location m_pos;
		
		static ParseResult parseLua(std::string);
	};
}

#endif // LUAENGINE_H
