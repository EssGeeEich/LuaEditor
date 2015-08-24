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

#include "luafunctionhintproposalmodel.h"
#include "scanner/luascanner.h"

namespace LuaEditor { namespace Internal {

LuaFunctionHintProposalModel::LuaFunctionHintProposalModel(const QVector<LuaEngine::LuaInfo> &functionSymbols)
	: m_items(functionSymbols) {}
void LuaFunctionHintProposalModel::reset() {}
int LuaFunctionHintProposalModel::size() const { return m_items.size(); }
QString LuaFunctionHintProposalModel::text(int index) const
{ return QString::fromStdString(m_items.at(index).m_text); }
int LuaFunctionHintProposalModel::activeArgument(const QString &prefix) const
{
	Scanner scanner(prefix.constData(),prefix.size());
	
	int parcount[3] = {0,0,0};
	int argnum = 0;
	
	FormatToken tk;
	while((tk = scanner.read()).format() != Format_EndOfBlock)
	{
		if(tk.format() == Format_Operator)
		{
			QString operators = scanner.value(tk);
			for(QString::const_iterator it = operators.begin(); it != operators.end(); ++it)
			{
				if(*it == QLatin1Char('('))
					++parcount[0];
				else if(*it == QLatin1Char('['))
					++parcount[1];
				else if(*it == QLatin1Char('{'))
					++parcount[2];
				else if(*it == QLatin1Char(')'))
					--parcount[0];
				else if(*it == QLatin1Char(']'))
					--parcount[1];
				else if(*it == QLatin1Char('}'))
					--parcount[2];
				else if(*it == QLatin1Char(',')
						&& !parcount[0]
						&& !parcount[1]
						&& !parcount[2])
					++argnum;
			}
		}
	}
	
	if( (parcount[0] < 0)
		|| (parcount[1] < 0)
		|| (parcount[2] < 0))
		return -1;
	return argnum;
}

} }
