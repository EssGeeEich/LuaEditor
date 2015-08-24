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

#ifndef RECURSIVECLASSMEMBERS_H
#define RECURSIVECLASSMEMBERS_H
#include "../luaeditor_global.h"
#include <vector>

namespace LuaEditor { namespace Internal {

class RecursiveClassMembers {
	typedef std::list<RecursiveClassMembers> value_type;
	QString parentName;
	value_type childs;
	RecursiveClassMembers* mparent;
	
	void logRecursive();
public:
	typedef value_type::iterator iterator;
	typedef value_type::const_iterator const_iterator;
	
	RecursiveClassMembers();
	RecursiveClassMembers(QString const& name, RecursiveClassMembers* parent);
	
	QStringList buildDirectory() const;
	RecursiveClassMembers* matchesChilds(QStringList matchRecursiveList);
	RecursiveClassMembers const* parent() const;
	RecursiveClassMembers* parent();
	void clear();
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	
	QString const& key() const;
	RecursiveClassMembers& operator[](QString const& childName);
	void removeChild(QString const& childName);
	iterator find(QString const& childName);
	const_iterator find(QString const& childName) const;
};

} }
#endif // RECURSIVECLASSMEMBERS_H
