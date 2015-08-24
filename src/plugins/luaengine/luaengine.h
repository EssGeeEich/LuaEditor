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
	struct LuaInfo {
		enum Token {
			T_AND =0,
			T_BREAK,
			T_DO,
			T_ELSE,
			T_ELSEIF,
			T_END,
			T_FALSE,
			T_FOR,
			T_FUNCTION,
			T_GOTO,
			T_IF,
			T_IN,
			T_LOCAL,
			T_NIL,
			T_NOT,
			T_OR,
			T_REPEAT,
			T_RETURN,
			T_THEN,
			T_TRUE,
			T_UNTIL,
			T_WHILE,
			T_IDIV,
			T_CONCAT,
			T_DOTS,
			T_EQ,
			T_GE,
			T_LE,
			T_NE,
			T_SHL,
			T_SHR,
			T_DBCOLON,
			T_EOS,
			T_FLT,
			T_INT,
			T_NAME,
			T_STRING,
			
			T_ASSIGN,
			T_S_SEMICOLON,	// ;
			T_S_COLON,		// :
			T_S_COMMA,		// ,
			T_S_PERIOD,		// .
			
			T_S_LT,
			T_S_LSQBRACKET,
			T_S_LBRACKET,
			T_S_LPARENT,
			
			T_S_GT,
			T_S_RSQBRACKET,
			T_S_RBRACKET,
			T_S_RPARENT,
			
			T_U_MINUS,
			T_U_NOT,
			T_U_LENGTH,
			
			T_B_ADD,
			T_B_MUL,
			T_B_MOD,
			T_B_POW,
			T_B_DIV,
			T_B_BAND,
			T_B_BOR,
			
			T_B_BXOR = T_U_NOT,
			T_B_SUB = T_U_MINUS
		} m_token;
		std::string m_text;
		Location m_pos;
		
		inline LuaInfo() : m_token(T_EOS) {}
	};
	
	std::vector<LuaInfo*> filter(std::vector<LuaInfo>&, std::vector<LuaInfo::Token> const&);
	std::vector<LuaInfo*> filter(std::vector<LuaInfo>&, LuaInfo::Token const&);
	
	struct ParseResult {
		std::vector<LuaInfo> m_luainfos;
		std::string m_error;
		Location m_pos;
		
		
		static ParseResult parseLua(std::string);
	};
}

#endif // LUAENGINE_H
