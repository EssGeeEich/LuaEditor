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
**	File created on: 23/08/2015
*/

#include "luaautocompleter.h"
#include "scanner/luascanner.h"

namespace LuaEditor { namespace Internal {

LuaAutoCompleter::LuaAutoCompleter() {}
LuaAutoCompleter::~LuaAutoCompleter() {}

bool LuaAutoCompleter::contextAllowsAutoParentheses(const QTextCursor &cursor, const QString &textToInsert) const
{
	QChar ch;
	if(!textToInsert.isEmpty())
		ch = textToInsert.at(0);
	
	if(!(ch == QLatin1Char('\'')
		 || ch == QLatin1Char('"')))
		return false;
	else if(isInComment(cursor))
		return false;
	return true;
}

bool LuaAutoCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
{
	QString blockText = cursor.block().text();
	Scanner scanner(blockText.constData(),blockText.size());
	scanner.setState(Scanner::TakeBackwardsState(cursor.block().previous()));
	FormatToken tk = scanner.tokenAt(cursor.positionInBlock());
	
	return (tk.format() == Format_Comment)
			|| (tk.format() == Format_String);
}

bool LuaAutoCompleter::isInComment(const QTextCursor &cursor) const
{
	QString blockText = cursor.block().text();
	Scanner scanner(blockText.constData(),blockText.size());
	scanner.setState(Scanner::TakeBackwardsState(cursor.block().previous()));
	FormatToken tk = scanner.tokenAt(cursor.positionInBlock());
	
	return (tk.format() == Format_Comment);
}

} }
