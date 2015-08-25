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

#ifndef LUAEDITOR_GLOBAL_H
#define LUAEDITOR_GLOBAL_H

#include <QtGlobal>
#include <QFile>
#include <QString>
#include <sstream>

#ifdef ALLOW_LOGGING
	#define USES_LOGGER() static bool g_isFileOpen = []()->bool{return openLogFile();}(); Q_UNUSED(g_isFileOpen)
	#define LOG(x) [&](){std::ostringstream oss; {oss << getTabs() << x;} logFile().write(oss.str().c_str(),oss.str().size()); logFile().write("\n",1); qWarning("%s",oss.str().c_str()); logFile().flush();}()
	#define LOG_SECTION(x) FunctionScopeLogger fsx_dnd(x); Q_UNUSED(fsx_dnd)
	
	struct FunctionScopeLogger {
		FunctionScopeLogger();
		FunctionScopeLogger(std::string const& name);
		~FunctionScopeLogger();
	};

	std::string getTabs();
	QFile& logFile();
	bool openLogFile();
#else
	#define USES_LOGGER()
	#define LOG(x) [&](){std::ostringstream oss; {oss << x;}}()
	#define LOG_SECTION(x)
#endif

#if defined(LUAEDITOR_LIBRARY)
#  define LUAEDITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LUAEDITORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LUAEDITOR_GLOBAL_H

