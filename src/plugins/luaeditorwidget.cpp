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

#include "luaeditorwidget.h"
#include <QComboBox>
#include <QHeaderView>
#include <QTextBlock>
#include <QTreeView>
#include <utility>

#include "luaengine/luaengine.h"

enum {
	UPDATE_DOCUMENT_DEFAULT_INTERVAL = 100
};

namespace LuaEditor { namespace Internal {

LuaEditorWidget::LuaEditorWidget()
	: m_outlineCombo(new QComboBox)
{
	m_updateDocumentTimer.setInterval(UPDATE_DOCUMENT_DEFAULT_INTERVAL);
	m_updateDocumentTimer.setSingleShot(true);
	connect(&m_updateDocumentTimer, &QTimer::timeout, this, &LuaEditorWidget::updateDocument);
	connect(this, &QPlainTextEdit::textChanged, [this](){m_updateDocumentTimer.start();});
	
	m_outlineCombo->setMinimumContentsLength(22);
	
	QTreeView* treeView = new QTreeView;
	treeView->header()->hide();
	treeView->setItemsExpandable(false);
	treeView->setRootIsDecorated(false);
		m_outlineCombo->setView(treeView);
	treeView->expandAll();
	
	QSizePolicy policy = m_outlineCombo->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Expanding);
	m_outlineCombo->setSizePolicy(policy);
	
	insertExtraToolBarWidget(TextEditorWidget::Left, m_outlineCombo);
}

QTextEdit::ExtraSelection LuaEditorWidget::CreateExtraSelection(int lineNumber, const QTextCharFormat &errorFormat, const LuaEngine::Location *errorLocation, bool isFirstLine, bool isLastLine)
{
	QTextCursor cursor(document()->findBlockByLineNumber(lineNumber));
	
	bool isSet = false;
	
	if(isFirstLine)
	{
		if(errorLocation->m_begin.m_offset == std::string::npos)
		{
			cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
			isSet = true;
		}
		else if(errorLocation->m_begin.m_offset)
		{
			cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, errorLocation->m_begin.m_offset);
			isSet = true;
		}
	}
	
	if(!isSet)
	{
		cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
#if 0
		if(!cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor))
			cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor);
		else
			cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
#endif
	}
	
	isSet = false;
	
	if(isLastLine)
	{
		if(!errorLocation->m_end.m_offset)
		{
			cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
			isSet = true;
		}
		else if(errorLocation->m_end.m_offset != std::string::npos)
		{
			cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, errorLocation->m_end.m_offset);
			isSet = true;
		}
	}
	
	if(!isSet)
	{
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
#if 0
		if(!cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor))
			cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
		cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
#endif
	}
	
	return {std::move(cursor),errorFormat};
}

void LuaEditorWidget::updateDocument()
{
	m_updateDocumentTimer.stop();
	
	::LuaEngine::ParseResult result;
	try { result = ::LuaEngine::ParseResult::parseLua(toPlainText().toStdString()); } catch(...) {}
	
	if(!result.m_error.empty())
	{
		QTextCharFormat errorFormat;
			errorFormat.setBackground(QBrush(QColor(255,246,246)));
			errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
			errorFormat.setUnderlineColor(Qt::red);
			errorFormat.setToolTip(QString::fromStdString(result.m_error));
			
		std::string::size_type endLine = std::min<std::string::size_type>(
					result.m_pos.m_end.m_line,
					static_cast<std::string::size_type>(std::max<int>(document()->lineCount(),0)));
		
		QList<QTextEdit::ExtraSelection> sels;
		for(std::string::size_type i = result.m_pos.m_begin.m_line; i <= endLine; ++i)
		{
			sels.append(
					CreateExtraSelection(static_cast<int>(i),errorFormat, &(result.m_pos), i==result.m_pos.m_begin.m_line, i==endLine)
				);
		}
		
		setExtraSelections(CodeWarningsSelection, sels);
	}
	else
		setExtraSelections(CodeWarningsSelection, QList<QTextEdit::ExtraSelection>{});
}

} }
