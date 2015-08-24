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

#ifndef LUAEDITORCONSTANTS_H
#define LUAEDITORCONSTANTS_H
#include "luaeditor_global.h"

namespace LuaEditor { namespace Constants {

const char M_CONTEXT[] = "LuaEditor.ContextMenu";
const char M_TOOLS_LUA[] = "LuaEditor.Tools.Menu";

const char C_LUAEDITOR_ID[] = "LuaEditor.LuaEditor";
const char C_LUAEDITOR_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors", "Lua Editor");

const char LUA_SOURCE_MIMETYPE[] = "text/x-luasrc";
const char LUA_HEADER_MIMETYPE[] = "text/x-luahdr";

const char WIZARD_CATEGORY_LUA[] = "U.Lua";
const char WIZARD_TR_CATEGORY_LUA[] = QT_TRANSLATE_NOOP("LuaEditor","Lua");

} }

#endif // LUAEDITORCONSTANTS_H

