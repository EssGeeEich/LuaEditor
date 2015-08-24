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

#ifndef LUAEDITORPLUGIN_H
#define LUAEDITORPLUGIN_H

#include "luaeditor_global.h"

#include <extensionsystem/iplugin.h>

#include <QtPlugin>
#include <QAction>

namespace LuaEditor { namespace Internal {

class LuaEditorPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "LuaEditor.json")
public:
	LuaEditorPlugin();
	~LuaEditorPlugin();
	
	bool initialize(const QStringList &arguments, QString *errorString) override;
	void extensionsInitialized() override;
	ShutdownFlag aboutToShutdown() override;
};

} }

#endif // LUACODEMODEL_H

