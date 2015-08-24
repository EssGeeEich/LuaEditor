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

#ifndef LUACOMPLETIONASSISTPROVIDER_H
#define LUACOMPLETIONASSISTPROVIDER_H
#include "luaeditor_global.h"
#include <texteditor/codeassist/completionassistprovider.h>

namespace LuaEditor { namespace Internal {

class LuaCompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
	Q_OBJECT
public:
	bool supportsEditor(Core::Id editorId) const;
	TextEditor::IAssistProcessor* createProcessor() const;
	int activationCharSequenceLength() const;
	bool isActivationCharSequence(QString const& sequence) const;
};

} }
#endif // LUACOMPLETIONASSISTPROVIDER_H
