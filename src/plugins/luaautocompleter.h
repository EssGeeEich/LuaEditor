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

#ifndef LUAAUTOCOMPLETER_H
#define LUAAUTOCOMPLETER_H
#include "luaeditor_global.h"
#include <texteditor/autocompleter.h>

namespace LuaEditor { namespace Internal {

class LuaAutoCompleter : public TextEditor::AutoCompleter
{
public:
	LuaAutoCompleter();
	virtual ~LuaAutoCompleter();
	virtual bool contextAllowsAutoParentheses(const QTextCursor &cursor, const QString &textToInsert) const;
	virtual bool contextAllowsElectricCharacters(const QTextCursor &cursor) const;
	virtual bool isInComment(const QTextCursor &cursor) const;
	//virtual QString insertMatchingBrace(const QTextCursor &cursor, const QString &text, QChar la, int *skippedChars) const;
	//virtual QString insertParagraphSeparator(const QTextCursor &cursor) const;
};

} }
#endif // LUAAUTOCOMPLETER_H
