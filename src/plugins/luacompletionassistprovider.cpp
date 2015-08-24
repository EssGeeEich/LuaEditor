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

#include "luacompletionassistprovider.h"
#include "luacompletionassistprocessor.h"
#include "luaeditorconstants.h"
#include <coreplugin/id.h>

namespace LuaEditor { namespace Internal {

static bool isActivationChar(QChar const& ch)
{
	switch(ch.unicode())
	{
	case '(':
	case '.':
	case ':':
	case ',':
		return true;
	}
	return false;
}

bool LuaCompletionAssistProvider::supportsEditor(Core::Id editorId) const
{
	return editorId == Constants::C_LUAEDITOR_ID;
}

TextEditor::IAssistProcessor* LuaCompletionAssistProvider::createProcessor() const
{
	return new LuaCompletionAssistProcessor;
}

int LuaCompletionAssistProvider::activationCharSequenceLength() const
{
	return 1;
}

bool LuaCompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
	return isActivationChar(sequence.at(0));
}
} }
