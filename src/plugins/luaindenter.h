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

#ifndef LUAINDENTER_H
#define LUAINDENTER_H

#include "luaeditor_global.h"
#include <texteditor/indenter.h>

namespace LuaEditor { namespace Internal {

class LuaIndenter : public TextEditor::Indenter
{
public:
	LuaIndenter();
	virtual ~LuaIndenter();
	
	bool isElectricCharacter(const QChar &ch) const;
	virtual bool isInvElectricCharacter(const QChar &ch) const;
	void indentBlock(QTextDocument *doc, const QTextBlock &block, const QChar &typedChar, const TextEditor::TabSettings &tabSettings);
protected:
	QString getLastKeyword(QString const& line) const;
	int getLineDelta(QString const& line) const;
};

} }

#endif // LUAINDENTER_H
