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

#include "luaeditor_global.h"
#include <QDir>
#include <QStandardPaths>

#ifdef ALLOW_LOGGING
static std::string g_tabs;
static std::int32_t g_tab_count;
std::string getTabs() { return g_tabs; }
static void genTabs()
{
	g_tabs.clear();
	for(std::int32_t i=0; i<g_tab_count; ++i)
	{
		g_tabs += "  ";
	}
}
FunctionScopeLogger::FunctionScopeLogger() {
	++g_tab_count; genTabs();
}
FunctionScopeLogger::FunctionScopeLogger(std::string const& name)
{
	LOG("-- " << name << " --");
	++g_tab_count; genTabs();
}
FunctionScopeLogger::~FunctionScopeLogger()
{
	--g_tab_count; genTabs();
	LOG("<<----");
}

QFile& logFile()
{
	static QFile file;
	return file;
}
bool openLogFile()
{
	if(logFile().isOpen())
		return true;
	logFile().setFileName(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)+QLatin1String("/log.txt"));
	logFile().open(QFile::WriteOnly|QFile::Text);
	return logFile().isOpen();
}
#endif
