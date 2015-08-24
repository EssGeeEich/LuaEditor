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

#ifndef LUAEDITORWIDGET_H
#define LUAEDITORWIDGET_H
#include "luaeditor_global.h"
#include <texteditor/texteditor.h>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(QComboBox)
namespace LuaEngine { struct Location; }

namespace LuaEditor { namespace Internal {

class LuaEditorWidget : public TextEditor::TextEditorWidget
{
	void updateDocument();
	QTextEdit::ExtraSelection CreateExtraSelection(int lineNumber, QTextCharFormat const& errorFormat,
												   LuaEngine::Location const* errorLocation, bool isFirstLine, bool isLastLine);
	
	QTimer m_updateDocumentTimer;
	QComboBox* m_outlineCombo;
public:
	LuaEditorWidget();
};

} }
#endif // LUAEDITORWIDGET_H
