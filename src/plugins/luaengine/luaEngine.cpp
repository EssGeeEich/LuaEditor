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

#include "luaengine.h"
#include "luaengine_requirements.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

namespace LuaEngine {

struct read_string_s {
	read_string_s(std::string& s)
		: string(&s), index(0) {}
	read_string_s()
		: string(nullptr), index(0) {}
	std::string const* string;
	std::string::size_type index;
};

static char const* read_string(lua_State*, void* ud, size_t* sz)
{
	read_string_s& readdata = *static_cast<read_string_s*>(ud);
	
	*sz = std::min<int>(readdata.string->size() - readdata.index, 512);
	std::string::size_type offset = readdata.index; readdata.index += *sz;
	return readdata.string->c_str() + offset;
}

static int parseLuai(lua_State* L)
{
	std::string& contents = *static_cast<std::string*>(lua_touserdata(L,1));
	ParseResult& result = *static_cast<ParseResult*>(lua_touserdata(L,2));
	read_string_s readdata(contents);
	
	// Using lua_load instead of luaL_loadstring forces text parsing.
	// You should not edit a binary file.
	if(lua_load(L,read_string,&readdata,"","t") != LUA_OK)
	{
		size_t size=0; char const* error=lua_tolstring(L,-1,&size);
		
		std::string stderror(error,size);
		std::string buffer;
		std::istringstream iss;
		int errorLocation = 0;
		
		if(
		   (iss = std::istringstream{stderror},iss) &&
		   std::getline(iss,buffer,':') &&
		   (iss >> errorLocation) &&
		   (iss.get(),iss.get(),iss) &&
		   std::getline(iss,result.m_error) &&
		   result.m_error != "syntax error near <eof>")
		{
			iss = std::istringstream{result.m_error};
			
			std::string last[3];
			int id = -1;
			int actualErrorLine;
			bool actualErrorLineFound = false;
			while(iss >> last[(++id)%3])
			{
				int at_id = (id+1)%3;
				int line_id = (id+2)%3;
				int lnum_id = id%3;
				
				if( (last[at_id] == "at") &&
					(last[line_id] == "line"))
				{
					int value = 0;
					bool correct = false;
					try {
						value = std::stoi(last[lnum_id],0,10);
						correct = true;
					}
					catch(std::invalid_argument&) {}
					catch(std::out_of_range&) {value=0;correct=true;}
					catch(...) {}
					
					if(correct)
					{
						actualErrorLine = value;
						actualErrorLineFound = true;
						break;
					}
				}
			}
			
			if(actualErrorLineFound)
			{
				result.m_pos.m_begin.m_line = actualErrorLine>0 ? actualErrorLine-1 : 0;
				result.m_pos.m_begin.m_offset = 0;
				result.m_pos.m_end.m_line = errorLocation-1;
				result.m_pos.m_end.m_offset = std::string::npos;
			}
			else
			{
				result.m_pos.m_begin.m_line = errorLocation-1;
				result.m_pos.m_begin.m_offset = 0;
				result.m_pos.m_end.m_line = errorLocation-1;
				result.m_pos.m_end.m_offset = std::string::npos;
			}
		}
		else
		{
			result.m_pos.m_begin.m_line = 0;
			result.m_pos.m_begin.m_offset = 0;
			result.m_pos.m_end.m_line = std::string::npos;
			result.m_pos.m_end.m_offset = std::string::npos;
		}
	}
	return 0;
}

ParseResult ParseResult::parseLua(std::string contents)
{
	ParseResult result;
	lua_State* L = luaL_newstate();
	if(!L)
	{
		result.m_pos.m_begin.m_line = 0;
		result.m_pos.m_begin.m_offset = 0;
		result.m_pos.m_end.m_line = std::string::npos;
		result.m_pos.m_end.m_offset = std::string::npos;
		result.m_error = "Cannot create LUA state: Not enough memory.";
		return result;
	}
	
	lua_pushcfunction(L,&parseLuai);
	lua_pushlightuserdata(L,&contents);
	lua_pushlightuserdata(L,&result);
	
	if(lua_pcall(L,2,0,0) != LUA_OK)
	{
		result.m_pos.m_begin.m_line = 0;
		result.m_pos.m_begin.m_offset = 0;
		result.m_pos.m_end.m_line = std::string::npos;
		result.m_pos.m_end.m_offset = std::string::npos;
		
		size_t size; char const* error = lua_tolstring(L,-1,&size);
		result.m_error.assign(error,error+size);
	}
	lua_close(L);
	return result;
}

}
