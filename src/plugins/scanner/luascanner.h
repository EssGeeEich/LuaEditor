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

#ifndef LUASCANNER_H
#define LUASCANNER_H
#include "../luaeditor_global.h"
#include "luaformattoken.h"
#include "sourcecodestream.h"
#include "recursiveclassmembers.h"
#include "../luaengine/luaengine.h"
#include <QTextBlock>
#include <QMap>
#include <QSet>

namespace LuaEditor { namespace Internal {

class Scanner
{
	Scanner(Scanner const&) =delete;
	Scanner& operator= (Scanner const&) =delete;
public:
	enum State {
		State_Default			=0x00000000,
		State_String			=0x00000001,
		State_MultiLineString	=0x00000002,
		State_MultiLineComment	=0x00000003,
		State_Default_ClassMem	=0x00000004
	};
	
	Scanner(QChar const* text, int const length);
	
	void setState(int);
	int state() const;
	FormatToken read();
	QString value(FormatToken const& tk) const;
	FormatToken tokenAt(int offset);
	
	static void TakeBackwardsMember(QTextBlock block, RecursiveClassMembers& targetIdentifier);
	static int TakeBackwardsState(QTextBlock block, RecursiveClassMembers* targetIdentifiers =nullptr);
private:
	FormatToken onDefaultState();
	
	void checkEscapeSequence(QChar quoteChar);
	FormatToken readStringLiteral(QChar quoteChar);
	FormatToken readMultiLineStringLiteral(int literalId);
	FormatToken readIdentifier();
	FormatToken readNumber();
	FormatToken readFloatNumber();
	FormatToken readComment();
	FormatToken readMultiLineComment(int literalId);
	FormatToken readWhiteSpace();
	FormatToken readOperator();
	
	void clearState();
	void saveState(State state, QChar savedData);
	void parseState(State& state, QChar& savedData) const;
	
	SourceCodeStream m_src;
	int m_state;
};

} }

#endif // LUASCANNER_H
