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

#ifndef LUAFUNCTIONHINTPROPOSALMODEL_H
#define LUAFUNCTIONHINTPROPOSALMODEL_H
#include "luaeditor_global.h"
#include <texteditor/codeassist/ifunctionhintproposalmodel.h>
#include "luaengine/luaengine.h"

namespace LuaEditor { namespace Internal {

class LuaFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
	LuaFunctionHintProposalModel(QVector<LuaEngine::LuaInfo> const& functionSymbols);
	void reset();
	int size() const;
	QString text(int index) const;
	int activeArgument(QString const& prefix) const;
private:
	QVector<LuaEngine::LuaInfo> m_items;
};

} }
#endif // LUAFUNCTIONHINTPROPOSALMODEL_H
