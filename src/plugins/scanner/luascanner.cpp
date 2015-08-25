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

#include "luascanner.h"

namespace LuaEditor { namespace Internal {

static QMap<Format,QSet<QString>> const g_pairs =
{
	{Format_ClassField, {
		QStringLiteral("self")
	}},
	{Format_Local, {
		QStringLiteral("local")
	}},
	{Format_MagicAttr, {
		QStringLiteral("__add"),
		QStringLiteral("__sub"),
		QStringLiteral("__mul"),
		QStringLiteral("__div"),
		QStringLiteral("__mod"),
		QStringLiteral("__pow"),
		QStringLiteral("__unm"),
		QStringLiteral("__idiv"),
		QStringLiteral("__band"),
		QStringLiteral("__bor"),
		QStringLiteral("__bxor"),
		QStringLiteral("__bnot"),
		QStringLiteral("__shl"),
		QStringLiteral("__shr"),
		QStringLiteral("__concat"),
		QStringLiteral("__len"),
		QStringLiteral("__eq"),
		QStringLiteral("__lt"),
		QStringLiteral("__le"),
		QStringLiteral("__index"),
		QStringLiteral("__newindex"),
		QStringLiteral("__call")
	}},
	{Format_Keyword, {
		QStringLiteral("and"),
		QStringLiteral("break"),
		QStringLiteral("do"),
		QStringLiteral("else"),
		QStringLiteral("elseif"),
		QStringLiteral("end"),
		QStringLiteral("false"),
		QStringLiteral("for"),
		QStringLiteral("function"),
		QStringLiteral("goto"),
		QStringLiteral("if"),
		QStringLiteral("in"),
		QStringLiteral("local"),
		QStringLiteral("nil"),
		QStringLiteral("not"),
		QStringLiteral("or"),
		QStringLiteral("repeat"),
		QStringLiteral("return"),
		QStringLiteral("then"),
		QStringLiteral("true"),
		QStringLiteral("until"),
		QStringLiteral("while")
	}}
};

Scanner::Scanner(QChar const* text, int const length)
	: m_src(text, length),
	  m_state(0) {}

void Scanner::setState(int state) { m_state = state; }
int Scanner::state() const { return m_state; }

FormatToken Scanner::read()
{
	m_src.setAnchor();
	if(m_src.isEnd())
		return FormatToken(Format_EndOfBlock, m_src.anchor(), 0);
	
	State state;
	QChar saved;
	parseState(state, saved);
	switch(state) {
	case State_String:
		return readStringLiteral(saved);
	case State_MultiLineString:
		return readMultiLineStringLiteral(static_cast<int>(saved.toLatin1()));
	case State_MultiLineComment:
		return readMultiLineComment(static_cast<int>(saved.toLatin1()));
	default:
		return onDefaultState();
	}
}

QString Scanner::value(FormatToken const& tk) const
{
	return m_src.value(tk.begin(), tk.length());
}

FormatToken Scanner::tokenAt(int offset)
{
	SourceCodeStream old_src = m_src;
	int old_state = m_state;
	FormatToken result;
	
	{
		m_src.anchorTo(0);
		while((result = read()).format() != Format_EndOfBlock)
		{
			if( (offset >= static_cast<int>(result.begin()))
				&& (offset <= static_cast<int>(result.end())) )
				break;
		}
	}
	
	old_src = m_src;
	m_state = old_state;
	
	return result;
}

FormatToken Scanner::onDefaultState()
{
	QChar first = m_src.peek();
	m_src.move();
	
	if(first == QLatin1Char('\\') && m_src.peek() == QLatin1Char('\n'))
	{
		m_src.move();
		return FormatToken(Format_Whitespace, m_src.anchor(), 2);
	}
	
	if(first == QLatin1Char('.') && m_src.peek().isDigit())
		return readFloatNumber();
	
	if(first == QLatin1Char('\'') || first == QLatin1Char('"'))
		return readStringLiteral(first);
	
	if(first.isLetter() || (first == QLatin1Char('_')))
		return readIdentifier();
	
	if(first.isDigit())
		return readNumber();
	
	if(first == QLatin1Char('-') && m_src.peek() == QLatin1Char('-'))
	{
		m_src.move();
		if(m_src.peek() == QLatin1Char('['))
		{
			int count = 0;
			while(m_src.peek(count+1) == QLatin1Char('='))
				++count;
			if(m_src.peek(count+1) == QLatin1Char('['))
			{
				m_src.move(count+2);
				return readMultiLineComment(count);
			}
		}
		return readComment();
	}
	
	if(first == QLatin1Char('['))
	{
		int count = 0;
		while(m_src.peek(count) == QLatin1Char('='))
			++count;
		if(m_src.peek(count) == QLatin1Char('['))
		{
			m_src.move(count+1);
			return readMultiLineStringLiteral(count);
		}
	}
	
	if(first.isSpace())
		return readWhiteSpace();
	
	return readOperator();
}

void Scanner::checkEscapeSequence(QChar quoteChar)
{
	if(m_src.peek() == QLatin1Char('\\')) {
		m_src.move();
		QChar ch = m_src.peek();
		if(ch == QLatin1Char('\n') || ch.isNull())
			saveState(State_String, quoteChar);
	}
}

FormatToken Scanner::readStringLiteral(QChar quoteChar)
{
	QChar ch = m_src.peek();
	
	while(ch != quoteChar && !ch.isNull()) {
		checkEscapeSequence(quoteChar);
		m_src.move();
		ch = m_src.peek();
	}
	if(ch == quoteChar)
		clearState();
	m_src.move();
	return FormatToken(Format_String, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readMultiLineStringLiteral(int literalId)
{
	for(;;) {
		QChar ch = m_src.peek();
		if(ch.isNull())
			break;
		if(ch == QLatin1Char(']'))
		{
			bool bMatching = true;
			for(int i = 0; i < literalId; ++i)
			{
				if(m_src.peek(i+1) != QLatin1Char('='))
				{
					bMatching = false;
					break;
				}
			}
			if(bMatching && m_src.peek(literalId+1) == QLatin1Char(']'))
			{
				clearState();
				m_src.move(literalId+2);
				break;
			}
		}
		m_src.move();
	}
	
	return FormatToken(Format_String, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readIdentifier()
{
	QChar ch = m_src.peek();
	while(ch.isLetterOrNumber() || (ch == QLatin1Char('_'))) {
		m_src.move();
		ch = m_src.peek();
	}
	
	QString value = m_src.value();
	
	Format tkFormat = Format_Identifier;
	for(auto it = g_pairs.begin(); it != g_pairs.end(); ++it)
	{
		if(it.value().contains(value))
		{
			tkFormat = it.key();
			break;
		}
	}
	
	return FormatToken(tkFormat, m_src.anchor(), m_src.length());
}

inline static bool isHexDigit(QChar ch)
{
    return (ch.isDigit()
		|| (ch >= QLatin1Char('a') && ch <= QLatin1Char('f'))
		|| (ch >= QLatin1Char('A') && ch <= QLatin1Char('F')));
}

FormatToken Scanner::readNumber()
{
	if(!m_src.isEnd()) {
		if(m_src.peek().toLower() == QLatin1Char('x'))
		{
			m_src.move();
			while(isHexDigit(m_src.peek()))
				m_src.move();
			if(m_src.peek() == QLatin1Char('.'))
			{
				m_src.move();
				while(isHexDigit(m_src.peek()))
					m_src.move();
			}
			if(m_src.peek().toLower() == QLatin1Char('p'))
			{
				m_src.move();
				if( (m_src.peek() == QLatin1Char('+')) || (m_src.peek() == QLatin1Char('-')))
					m_src.move();
				while(m_src.peek().isDigit())
					m_src.move();
			}
		}
		else if(m_src.peek() == QLatin1Char('.') || m_src.peek().isDigit())
			return readFloatNumber();
	}
	return FormatToken(Format_Number, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readFloatNumber()
{
	if(!m_src.isEnd()) {
		bool hasDot = false;
		bool hasExp = false;
		for(;;) {
			QChar ch = m_src.peek();
			if(ch.isNull())
				break;
			
			if(ch == QLatin1Char('.'))
			{
				if(hasDot)
					break;
				m_src.move();
				hasDot = true;
				continue;
			}
			else if(ch == QLatin1Char('e'))
			{
				if(hasExp)
					break;
				m_src.move();
				if(m_src.peek() == QLatin1Char('-'))
					m_src.move();
				hasExp = true;
				continue;
			}
			else if(!ch.isDigit())
				break;
			m_src.move();
		}
	}
	
	return FormatToken(Format_Number, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readComment()
{
	QChar ch = m_src.peek();
	while(ch != QLatin1Char('\n') && !ch.isNull()) {
		m_src.move();
		ch = m_src.peek();
	}
	return FormatToken(Format_Comment, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readMultiLineComment(int literalId)
{
	for(;;) {
		QChar ch = m_src.peek();
		if(ch.isNull())
			break;
		if(ch == QLatin1Char(']'))
		{
			bool bMatching = true;
			for(int i=0; i < literalId; ++i)
			{
				if(m_src.peek(i+1) != QLatin1Char('='))
				{
					bMatching = false;
					break;
				}
			}
			if(bMatching && m_src.peek(literalId+1) == QLatin1Char(']'))
			{
				clearState();
				m_src.move(literalId+2);
				break;
			}
		}
		m_src.move();
	}
	return FormatToken(Format_String, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readWhiteSpace()
{
	while(m_src.peek().isSpace())
		m_src.move();
	return FormatToken(Format_Whitespace, m_src.anchor(), m_src.length());
}

FormatToken Scanner::readOperator()
{
	QString const EXCLUDED_CHARS = QLatin1String("\"'_");
	QChar ch = m_src.peek();
	while(ch.isPunct() && !EXCLUDED_CHARS.contains(ch)) {
		if(ch == QLatin1Char('-') && m_src.peek(1) == QLatin1Char('-'))
			break;
		m_src.move();
		ch = m_src.peek();
	}
	return FormatToken(Format_Operator, m_src.anchor(), m_src.length());
}

void Scanner::clearState() { m_state = 0; }
void Scanner::saveState(State state, QChar savedData)
{
	m_state = (state << 16) | static_cast<int>(savedData.unicode());
}
void Scanner::parseState(State &state, QChar &savedData) const
{
	state = static_cast<State>(m_state >> 16);
	savedData = static_cast<ushort>(m_state);
}

void Scanner::TakeBackwardsMember(QTextBlock block, RecursiveClassMembers &targetIdentifier)
{
	QString str = block.text();
	
	int pos = str.size()-1;
	QChar ch = str.at(pos);
	while((pos >= 0) && (ch.isSpace()))
		ch = str.at(--pos);
	
	int eos = pos;
	while((pos >= 0) && (ch == QLatin1Char('.') || ch == QLatin1Char('_') || ch.isLetterOrNumber()))
		ch = str.at(--pos);
	
	++pos;
	
	QString result(str.constData()+pos,eos-pos);
	
	QStringList r = result.split(QLatin1Char('.'),QString::KeepEmptyParts);
	
	RecursiveClassMembers* mem = &targetIdentifier;
	for(auto it = r.begin(); it != r.end(); ++it)
	{
		mem = &(*mem)[*it];
	}
}

int Scanner::TakeBackwardsState(QTextBlock block, RecursiveClassMembers* targetIdentifiers)
{
	QList<QTextBlock> blockList;
	
	while(block.isValid())
	{
		blockList.push_front(block);
		block = block.previous();
	}
	
	int state = 0;
	for(auto it = blockList.begin(); it != blockList.end(); ++it)
	{
		QTextBlock const& bk = *it;
		QString str = bk.text();
		Scanner scanner(str.constData(),str.size());
		scanner.setState(state);
		
		FormatToken tk;
		RecursiveClassMembers* lastClassMember = nullptr;
		
		while((tk=scanner.read()).format() != Format_EndOfBlock)
		{
			if(!targetIdentifiers)
				continue;
			
			switch(tk.format())
			{
			case Format_Identifier:
				if(!lastClassMember)
					lastClassMember = targetIdentifiers;
				lastClassMember = &(*lastClassMember)[scanner.value(tk)];
				break;
			case Format_Operator:
				{
					if(scanner.value(tk) != QLatin1String("."))
						lastClassMember = nullptr;
				}
				break;
			default:
				break;
			}
		}
		state = scanner.state();
	}
	return state;
}

} }
