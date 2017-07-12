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

#include "luacompletionassistprocessor.h"
#include "scanner/luascanner.h"
#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/genericproposal.h>
#include <QTextCursor>
#include <QTextDocument>

namespace LuaEditor { namespace Internal {

static QStringList const g_special = {
	QStringLiteral("self")
};

static QStringList const g_types = {
	QStringLiteral("local")
};

static QStringList const g_keyword_beginning = {
	QStringLiteral("break"),
	QStringLiteral("do"),
	QStringLiteral("else"),
	QStringLiteral("elseif"),
	QStringLiteral("end"),
	QStringLiteral("for"),
	QStringLiteral("function"),
	QStringLiteral("goto"),
	QStringLiteral("if"),
	QStringLiteral("in"),
	QStringLiteral("repeat"),
	QStringLiteral("return"),
	QStringLiteral("then"),
	QStringLiteral("until"),
	QStringLiteral("while")
};

static QStringList const g_keyword_fcall = {
	QStringLiteral("true"),
	QStringLiteral("false"),
	QStringLiteral("nil"),
	QStringLiteral("and"),
	QStringLiteral("not"),
	QStringLiteral("or")
};

static QStringList const g_magics = {
	QStringLiteral("__index"),
	QStringLiteral("__newindex"),
	QStringLiteral("__concat"),
	QStringLiteral("__call"),
	QStringLiteral("__add"),
	QStringLiteral("__band"),
	QStringLiteral("__bnot"),
	QStringLiteral("__bor"),
	QStringLiteral("__bxor"),
	QStringLiteral("__div"),
	QStringLiteral("__eq"),
	QStringLiteral("__idiv"),
	QStringLiteral("__le"),
	QStringLiteral("__len"),
	QStringLiteral("__lt"),
	QStringLiteral("__mod"),
	QStringLiteral("__mul"),
	QStringLiteral("__pow"),
	QStringLiteral("__shl"),
	QStringLiteral("__shr"),
	QStringLiteral("__sub"),
	QStringLiteral("__unm")
};

LuaCompletionAssistProcessor::LuaCompletionAssistProcessor()
	: m_startPosition(0),
	  m_varIcon(QLatin1String(":/LuaEditor/images/var.png")),
	  m_functionIcon(QLatin1String(":/LuaEditor/images/func.png")),
	  m_memIcon(QLatin1String(":/LuaEditor/images/attributes.png")),
	  m_keywordIcon(QLatin1String(":/LuaEditor/images/keyword.png"))
{}
LuaCompletionAssistProcessor::~LuaCompletionAssistProcessor(){}

static TextEditor::AssistProposalItem* createCompletionItem(QString const& text, QIcon const& icon, int order =0)
{
	TextEditor::AssistProposalItem* item = new TextEditor::AssistProposalItem;
	item->setText(text); item->setIcon(icon); item->setOrder(order);
	return item;
}

struct PriorityList {
	PriorityList(QString const& s, int pr)
		: m_str{s}, m_pr(pr) {}
	PriorityList(QStringList const& s, int pr)
		: m_str(s), m_pr(pr) {}
	
	QStringList m_str;
	int m_pr;
};

TextEditor::IAssistProposal* LuaCompletionAssistProcessor::perform(const TextEditor::AssistInterface *interface)
{
	if(interface->reason() == TextEditor::IdleEditor && !acceptsIdleEditor())
		return 0;
	
	int pos = interface->position() - 1;
	QChar ch = interface->characterAt(pos);
	while(ch.isLetterOrNumber() || ch == QLatin1Char('_') || ch.isSpace())
	{
		ch = interface->characterAt(--pos);
	}
	
	
	bool isFunctionCall = (ch == QLatin1Char('(')) || (ch == QLatin1Char(','));
	bool isMemberCompletion = (ch == QLatin1Char('.'));
	
	QString currentMember;
	if(isMemberCompletion)
	{
		int cpos = pos-1;
		while((cpos >= 0) && interface->characterAt(cpos).isSpace())
			--cpos;
		int cpos_end = cpos;
		
		while((cpos >= 0) && (interface->characterAt(cpos).isLetterOrNumber() || interface->characterAt(cpos) == QLatin1Char('_')))
			--cpos;
		++cpos; ++cpos_end;
		
		currentMember = interface->textAt(cpos,cpos_end-cpos);
	}
	
	QList<PriorityList> globVariables;
	QList<PriorityList> variables;
	QList<PriorityList> keywords;
	QList<PriorityList> magics;
	
	RecursiveClassMembers targetIds;
	Scanner::TakeBackwardsState(interface->textDocument()->findBlockByLineNumber(interface->textDocument()->findBlock(interface->position()).firstLineNumber()-1),&targetIds);
	
	if(isMemberCompletion)
	{
		RecursiveClassMembers writtenTargetId;
		Scanner::TakeBackwardsMember(interface->textDocument()->findBlock(interface->position()),writtenTargetId);
		
		RecursiveClassMembers* deepest = &writtenTargetId;
		for(;;)
		{
			RecursiveClassMembers::iterator it = deepest->begin();
			if(it == deepest->end())
				break;
			deepest = &(*it);
		}
		
		RecursiveClassMembers* mem = targetIds.matchesChilds(deepest->buildDirectory());
				
		if(mem)
		{
			for(auto it = mem->begin(); it != mem->end(); ++it)
			{
				variables.append({it->key(),3});
			}
		}
		for(auto it = targetIds.begin(); it != targetIds.end(); ++it)
		{
			globVariables.append({it->key(),2});
		}
		globVariables.append({g_special,1});
		magics.append({g_magics,0});
	}
	else if(isFunctionCall)
	{
		for(auto it = targetIds.begin(); it != targetIds.end(); ++it)
		{
			variables.append({it->key(),2});
		}
		variables.append({g_special,1});
		keywords.append({g_keyword_fcall,0});
	}
	else
	{
		for(auto it = targetIds.begin(); it != targetIds.end(); ++it)
		{
			variables.append({it->key(),4});
		}
		variables.append({g_special,3});
		keywords.append({g_types,2});
		keywords.append({g_keyword_beginning,1});
		keywords.append({g_keyword_fcall,0});
	}
	
	QList<TextEditor::AssistProposalItemInterface*> m_completions;
	
	QSet<QString> m_usedSuggestions;
	
	for(auto it = variables.begin(); it != variables.end(); ++it)
	{
		PriorityList const& plit = *it;
		for(auto itb = plit.m_str.begin(); itb != plit.m_str.end(); ++itb)
		{
			if(!m_usedSuggestions.contains(*itb))
			{
				m_completions << createCompletionItem(*itb, m_memIcon, plit.m_pr);
				m_usedSuggestions.insert(*itb);
			}
		}
	}
	for(auto it = globVariables.begin(); it != globVariables.end(); ++it)
	{
		PriorityList const& plit = *it;
		for(auto itb = plit.m_str.begin(); itb != plit.m_str.end(); ++itb)
		{
			if(!m_usedSuggestions.contains(*itb))
			{
				m_completions << createCompletionItem(*itb, m_varIcon, plit.m_pr);
				m_usedSuggestions.insert(*itb);
			}
		}
	}
	for(auto it = keywords.begin(); it != keywords.end(); ++it)
	{
		PriorityList const& plit = *it;
		for(auto itb = plit.m_str.begin(); itb != plit.m_str.end(); ++itb)
		{
			if(!m_usedSuggestions.contains(*itb))
			{
				m_completions << createCompletionItem(*itb, m_keywordIcon, plit.m_pr);
				m_usedSuggestions.insert(*itb);
			}
		}
	}
	for(auto it = magics.begin(); it != magics.end(); ++it)
	{
		PriorityList const& plit = *it;
		for(auto itb = plit.m_str.begin(); itb != plit.m_str.end(); ++itb)
		{
			if(!m_usedSuggestions.contains(*itb))
			{
				m_completions << createCompletionItem(*itb, m_functionIcon, plit.m_pr);
				m_usedSuggestions.insert(*itb);
			}
		}
	}
	
	m_startPosition = pos+1;
	return new TextEditor::GenericProposal(m_startPosition, m_completions);
}

bool LuaCompletionAssistProcessor::acceptsIdleEditor() const { return false; }

} }
