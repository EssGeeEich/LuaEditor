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

#ifndef LUAHIGHLIGHTER_H
#define LUAHIGHLIGHTER_H
#include "luaeditor_global.h"
#include <texteditor/syntaxhighlighter.h>

namespace LuaEditor { namespace Internal {

class Scanner;

class LuaHighlighter : public TextEditor::SyntaxHighlighter
{
public:
	LuaHighlighter();
	
protected:
	void highlightBlock(QString const& text);
	
private:
	int highlightLine(QString const& text, int initialState);
	void highlightImport(Scanner& scanner);
};

} }
#endif // LUAHIGHLIGHTER_H
