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
**	File created on: 16/08/2015
*/

#include "luaeditorfactory.h"
#include "luaeditorconstants.h"
#include "luaeditorwidget.h"

#include "luaautocompleter.h"
#include "luahighlighter.h"
#include "luahoverhandler.h"
#include "luaindenter.h"
#include "luacompletionassistprovider.h"

#include <texteditor/textdocument.h>
#include <texteditor/texteditoractionhandler.h>
#include <QGuiApplication>

namespace LuaEditor { namespace Internal {

LuaEditorFactory::LuaEditorFactory()
{
	USES_LOGGER();
	setId(Constants::C_LUAEDITOR_ID);
	setDisplayName(qApp->translate("OpenWith::Editors", Constants::C_LUAEDITOR_DISPLAY_NAME));
	addMimeType(Constants::LUA_SOURCE_MIMETYPE);
	addMimeType(Constants::LUA_HEADER_MIMETYPE);
	
	addHoverHandler(new LuaHoverHandler);
	setDocumentCreator([](){return new TextEditor::TextDocument(Constants::C_LUAEDITOR_ID);});
	setEditorWidgetCreator([](){return new LuaEditorWidget;});
	setSyntaxHighlighterCreator([](){return new LuaHighlighter;});
	setIndenterCreator([](){return new LuaIndenter;});
	setAutoCompleterCreator([](){return new LuaAutoCompleter;});
	setCompletionAssistProvider(new LuaCompletionAssistProvider);
	
    setCommentStyle(Utils::CommentDefinition::NoStyle);
    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);
    setCodeFoldingSupported(true);

    setEditorActionHandlers(TextEditor::TextEditorActionHandler::Format
                          | TextEditor::TextEditorActionHandler::UnCollapseAll);
}

} }
