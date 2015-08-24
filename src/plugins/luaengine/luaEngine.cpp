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

static LuaInfo::Token fromLexerToken(int luatoken)
{
	switch(luatoken)
	{
	case '=':
		return LuaInfo::T_ASSIGN;
	case ';':
		return LuaInfo::T_S_SEMICOLON;
	case ':':
		return LuaInfo::T_S_COLON;
	case ',':
		return LuaInfo::T_S_COMMA;
	case '.':
		return LuaInfo::T_S_PERIOD;
	case '<':
		return LuaInfo::T_S_LT;
	case '[':
		return LuaInfo::T_S_LSQBRACKET;
	case '{':
		return LuaInfo::T_S_LBRACKET;
	case '(':
		return LuaInfo::T_S_LPARENT;
	case '>':
		return LuaInfo::T_S_GT;
	case ']':
		return LuaInfo::T_S_RSQBRACKET;
	case '}':
		return LuaInfo::T_S_RBRACKET;
	case ')':
		return LuaInfo::T_S_RPARENT;
	case '-':
		return LuaInfo::T_U_MINUS;
	case '~':
		return LuaInfo::T_U_NOT;
	case '#':
		return LuaInfo::T_U_LENGTH;
	case '+':
		return LuaInfo::T_B_ADD;
	case '*':
		return LuaInfo::T_B_MUL;
	case '%':
		return LuaInfo::T_B_MOD;
	case '^':
		return LuaInfo::T_B_POW;
	case '/':
		return LuaInfo::T_B_DIV;
	case '&':
		return LuaInfo::T_B_BAND;
	case '|':
		return LuaInfo::T_B_BOR;
	case TK_AND:
		return LuaInfo::T_AND;
	case TK_BREAK:
		return LuaInfo::T_BREAK;
	case TK_DO:
		return LuaInfo::T_DO;
	case TK_ELSE:
		return LuaInfo::T_ELSE;
	case TK_ELSEIF:
		return LuaInfo::T_ELSEIF;
	case TK_END:
		return LuaInfo::T_END;
	case TK_FALSE:
		return LuaInfo::T_FALSE;
	case TK_FOR:
		return LuaInfo::T_FOR;
	case TK_FUNCTION:
		return LuaInfo::T_FUNCTION;
	case TK_GOTO:
		return LuaInfo::T_GOTO;
	case TK_IF:
		return LuaInfo::T_IF;
	case TK_IN:
		return LuaInfo::T_IN;
	case TK_LOCAL:
		return LuaInfo::T_LOCAL;
	case TK_NIL:
		return LuaInfo::T_NIL;
	case TK_NOT:
		return LuaInfo::T_NOT;
	case TK_OR:
		return LuaInfo::T_OR;
	case TK_REPEAT:
		return LuaInfo::T_REPEAT;
	case TK_RETURN:
		return LuaInfo::T_RETURN;
	case TK_THEN:
		return LuaInfo::T_THEN;
	case TK_TRUE:
		return LuaInfo::T_TRUE;
	case TK_UNTIL:
		return LuaInfo::T_UNTIL;
	case TK_WHILE:
		return LuaInfo::T_WHILE;
	case TK_IDIV:
		return LuaInfo::T_IDIV;
	case TK_CONCAT:
		return LuaInfo::T_CONCAT;
	case TK_DOTS:
		return LuaInfo::T_DOTS;
	case TK_EQ:
		return LuaInfo::T_EQ;
	case TK_GE:
		return LuaInfo::T_GE;
	case TK_LE:
		return LuaInfo::T_LE;
	case TK_NE:
		return LuaInfo::T_NE;
	case TK_SHL:
		return LuaInfo::T_SHL;
	case TK_SHR:
		return LuaInfo::T_SHR;
	case TK_DBCOLON:
		return LuaInfo::T_DBCOLON;
	case TK_EOS:
		return LuaInfo::T_EOS;
	case TK_FLT:
		return LuaInfo::T_FLT;
	case TK_INT:
		return LuaInfo::T_INT;
	case TK_NAME:
		return LuaInfo::T_NAME;
	case TK_STRING:
		return LuaInfo::T_STRING;
	default:
		return LuaInfo::T_EOS;
	}
}

struct read_string_s {
	read_string_s(std::string& s)
		: string(&s), index(0), header_sent(0), footer_sent(0) {}
	read_string_s()
		: string(nullptr), index(0), header_sent(0), footer_sent(0) {}
	std::string const* string;
	std::string::size_type index;
	bool header_sent;
	bool footer_sent;
};

static char const g_header[] = "";
static char const g_footer[] = "";

static char const* read_string(lua_State*, void* ud, size_t* sz)
{
	read_string_s& readdata = *static_cast<read_string_s*>(ud);
	if(!readdata.header_sent && sizeof(g_header) > 1)
	{
		readdata.header_sent = true;
		*sz = sizeof(g_header)-1;
		return g_header;
	}
	if(readdata.index >= readdata.string->size())
	{
		if(!readdata.footer_sent && sizeof(g_footer) > 1)
		{
			readdata.footer_sent = true;
			*sz = sizeof(g_footer)-1;
			return g_footer;
		}
		*sz = 0;
		return nullptr;
	}
	// min(40,512) = 40
	// min(1231,512) = 512
	*sz = std::min<int>(readdata.string->size() - readdata.index, 512);
	std::string::size_type offset = readdata.index; readdata.index += *sz;
	return readdata.string->c_str() + offset;
}

static void lexerinfostore(LexState const* state, SemInfo const*, int current, void* pr)
{
	ParseResult& result = *static_cast<ParseResult*>(pr);
	
	LuaInfo info;
	if(current != TK_EOS)
	{
		if( (info.m_token = fromLexerToken(current) ) != LuaInfo::T_EOS)
		{
			info.m_pos.m_begin.m_line = state->linenumber;
			info.m_pos.m_begin.m_offset = state->sgh_readsz;
			info.m_pos.m_end.m_line = state->linenumber;
			info.m_pos.m_end.m_offset = state->sgh_tknsz + state->sgh_readsz;
			info.m_text = rtxtToken(const_cast<LexState*>(state),current);
			result.m_luainfos.push_back(std::move(info));
		}
		else
		{
			result.m_error = std::string("Lua Lexer Converter: Invalid token detected: ") + rtxtToken(const_cast<LexState*>(state),current);
			result.m_pos.m_begin.m_line = state->linenumber;
			result.m_pos.m_begin.m_offset = 0;
			result.m_pos.m_begin.m_line = state->linenumber;
			result.m_pos.m_begin.m_offset = std::string::npos;
		}
	}
}

static int parseLuai(lua_State* L)
{
	std::string& contents = *static_cast<std::string*>(lua_touserdata(L,1));
	ParseResult& result = *static_cast<ParseResult*>(lua_touserdata(L,2));
	read_string_s readdata(contents);
	
	lua_setlexerinfocallback(L,lexerinfostore,static_cast<void*>(&result));
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

std::vector<LuaInfo*> filter(std::vector<LuaInfo>& collection, const std::vector<LuaInfo::Token>& tokens)
{
	std::vector<LuaInfo*> filtered;
		filtered.reserve(collection.size());
	for(auto it = collection.begin(); it != collection.end(); ++it)
	{
		if(std::find(tokens.begin(),tokens.end(),it->m_token) != tokens.end())
			filtered.push_back(&*it);
	}
	return filtered;
}

std::vector<LuaInfo*> filter(std::vector<LuaInfo>& collection, const LuaInfo::Token& token)
{
	std::vector<LuaInfo*> filtered;
		filtered.reserve(collection.size());
	for(auto it = collection.begin(); it != collection.end(); ++it)
	{
		if(it->m_token == token)
			filtered.push_back(&*it);
	}
	return filtered;
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
