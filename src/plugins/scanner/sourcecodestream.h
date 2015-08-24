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

#ifndef SOURCECODESTREAM_H
#define SOURCECODESTREAM_H
#include "../luaeditor_global.h"
#include <QString>

namespace LuaEditor { namespace Internal {

class SourceCodeStream
{
public:
	SourceCodeStream(QChar const* text, int const length) :
		m_text(text),
		m_textLength(length),
		m_position(0),
		m_markedPosition(0) {}
	
	inline void anchorTo(int location) { m_position = location; setAnchor(); }
	inline void setAnchor() { m_markedPosition = m_position; }
	inline void move(size_t count =1) { m_position += count; }
	inline int length() const { return m_position - m_markedPosition; }
	inline int anchor() const { return m_markedPosition; }
	inline bool isEnd() const { return m_position >= m_textLength; }
	inline QChar peek(int offset = 0) const
	{
		int pos = m_position + offset;
		if(pos >= m_textLength)
			return QLatin1Char('\0');
		return m_text[pos];
	}
	inline QString value() const
	{
		return QString(m_text + m_markedPosition, length());
	}
	inline QString value(int begin, int length) const
	{
		return QString(m_text + begin, length);
	}
private:
	QChar const* m_text;
	int m_textLength;
	int m_position;
	int m_markedPosition;
};

} }
#endif // SOURCECODESTREAM_H
