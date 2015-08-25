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

#include "recursiveclassmembers.h"

namespace LuaEditor { namespace Internal {

RecursiveClassMembers::RecursiveClassMembers(QString const& name, RecursiveClassMembers* parent)
	: parentName(name), mparent(parent) {}
RecursiveClassMembers::RecursiveClassMembers()
	: mparent(nullptr) {}

QStringList RecursiveClassMembers::buildDirectory() const
{
	QStringList result;
	RecursiveClassMembers const* e = this;
	for(;;)
	{
		if(e->parent())
		{
			result.push_front(e->parentName);
			e = e->parent();
		}
		else
			break;
	}
	return result;
}

void RecursiveClassMembers::logRecursive() const
{
	LOG_SECTION("logRecursive");
	LOG(parentName.toStdString());
	for(auto it = begin(); it != end(); ++it)
		it->logRecursive();
}

RecursiveClassMembers* RecursiveClassMembers::matchesChilds(QStringList matchRecursiveList)
{
	RecursiveClassMembers* e = this;
	for(;;)
	{
		if(matchRecursiveList.empty())
			return e;
		QString thisMatch = matchRecursiveList.first();
		matchRecursiveList.pop_front();
		
		iterator it = e->find(thisMatch);
		if(it == e->end())
			return nullptr;
		e = &(*it);
	}
}
RecursiveClassMembers const* RecursiveClassMembers::parent() const { return mparent; }
RecursiveClassMembers* RecursiveClassMembers::parent() { return mparent; }
void RecursiveClassMembers::clear() { childs.clear(); }
RecursiveClassMembers::iterator RecursiveClassMembers::begin() { return childs.begin(); }
RecursiveClassMembers::const_iterator RecursiveClassMembers::begin() const { return childs.begin(); }
RecursiveClassMembers::const_iterator RecursiveClassMembers::cbegin() const { return childs.cbegin(); }
RecursiveClassMembers::iterator RecursiveClassMembers::end() { return childs.end(); }
RecursiveClassMembers::const_iterator RecursiveClassMembers::end() const { return childs.end(); }
RecursiveClassMembers::const_iterator RecursiveClassMembers::cend() const { return childs.cend(); }
QString const& RecursiveClassMembers::key() const { return parentName; }

RecursiveClassMembers& RecursiveClassMembers::operator [](QString const& childName)
{
	iterator k = find(childName);
	if(k != end())
		return *k;
	childs.emplace_front(childName,this);
	return childs.front();
}

void RecursiveClassMembers::removeChild(const QString &childName)
{
	const_iterator k = find(childName);
	if(k != end())
		childs.erase(k);
}

RecursiveClassMembers::iterator RecursiveClassMembers::find(QString const& childName)
{
	for(iterator it = childs.begin(); it != childs.end(); ++it)
	{
		if(it->key() == childName)
			return it;
	}
	return childs.end();
}

RecursiveClassMembers::const_iterator RecursiveClassMembers::find(QString const& childName) const
{
	for(const_iterator it = childs.cbegin(); it != childs.cend(); ++it)
	{
		if(it->key() == childName)
			return it;
	}
	return childs.cend();
}

} }
