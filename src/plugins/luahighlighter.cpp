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

#include "luahighlighter.h"
#include "scanner/luascanner.h"
#include "scanner/luaformattoken.h"

namespace LuaEditor { namespace Internal {

LuaHighlighter::LuaHighlighter()
{
	
	static QVector<TextEditor::TextStyle> categories;
	if(categories.isEmpty()) {
		// see Format in scanner/luaformattoken.h
		categories << TextEditor::C_NUMBER
				   << TextEditor::C_STRING
				   << TextEditor::C_TYPE
				   << TextEditor::C_FIELD
				   << TextEditor::C_JS_SCOPE_VAR
				   << TextEditor::C_KEYWORD
				   << TextEditor::C_OPERATOR
				   << TextEditor::C_COMMENT
				   << TextEditor::C_COMMENT
				   << TextEditor::C_TEXT
				   << TextEditor::C_VISUAL_WHITESPACE
				   << TextEditor::C_STRING;
	}
	setTextFormatCategories(categories);
}
void LuaHighlighter::highlightBlock(QString const& text)
{
	int initialState = previousBlockState();
	if(initialState == -1)
		initialState = 0;
	setCurrentBlockState(highlightLine(text, initialState));
}

static bool isImportKeyword(QString const& keyword)
{
	return keyword == QLatin1String("require");
}

int LuaHighlighter::highlightLine(QString const& text, int initialState)
{
	Scanner scanner(text.constData(),text.length());
	scanner.setState(initialState);
	
	FormatToken tk;
	bool hasOnlyWhitespace = true;
	while((tk = scanner.read()).format() != Format_EndOfBlock)
	{
		Format format = tk.format();
		if(format == Format_Keyword) {
			if(isImportKeyword(scanner.value(tk)) && hasOnlyWhitespace)
			{
				setFormat(tk.begin(), tk.length(), formatForCategory(format));
				highlightImport(scanner);
				break;
			}
		}
		
		setFormat(tk.begin(), tk.length(), formatForCategory(format));
		if(format != Format_Whitespace)
			hasOnlyWhitespace = false;
	}
	return scanner.state();
}

void LuaHighlighter::highlightImport(Scanner& scanner)
{
	FormatToken tk;
	while((tk = scanner.read()).format() != Format_EndOfBlock)
	{
		Format format = tk.format();
		if(format == Format_Identifier)
			format = Format_RequiredModule;
		setFormat(tk.begin(), tk.length(), formatForCategory(format));
	}
}

} }
