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
**	File created on: 22/08/2015
*/

#ifndef LUAFORMATTOKEN_H
#define LUAFORMATTOKEN_H
#include "../luaeditor_global.h"

namespace LuaEditor { namespace Internal {

// see LuaHighlighter::LuaHighlighter in luahighlighter.cpp
enum Format {
	Format_Number =0,
	Format_String,
	Format_Local,
	Format_ClassField,
	Format_MagicAttr,
	Format_Keyword,
	Format_Operator,
	Format_Comment,
	Format_Identifier,
	Format_Whitespace,
	Format_RequiredModule,
	
	Format_FormatsAmount,
	Format_EndOfBlock
};

struct FormatToken {
	FormatToken(Format format = Format_FormatsAmount, size_t position =0, size_t length =0)
		: m_format(format), m_position(position), m_length(length) {}
	
	inline Format format() const { return m_format; }
	inline size_t begin() const { return m_position; }
	inline size_t end() const { return m_position+m_length; }
	inline size_t length() const { return m_length; }
private:
	Format m_format;
	size_t m_position;
	size_t m_length;
};

} }
#endif // LUAFORMATTOKEN_H
