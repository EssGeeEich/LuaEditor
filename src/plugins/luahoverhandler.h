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

#ifndef LUAHOVERHANDLER_H
#define LUAHOVERHANDLER_H
#include "luaeditor_global.h"
#include <texteditor/basehoverhandler.h>

namespace LuaEditor { namespace Internal {

class LuaHoverHandler : public TextEditor::BaseHoverHandler
{
public:
	LuaHoverHandler() {}
private:
	virtual void identifyMatch(TextEditor::TextEditorWidget *editorWidget, int pos) override;
	virtual void decorateToolTip() override;
};

} }
#endif // LUAHOVERHANDLER_H
