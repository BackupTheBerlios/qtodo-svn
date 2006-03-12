/***************************************************************************
 *   Copyright (C) 2005 by Tobias Glaesser                                 *
 *   tobi.web@gmx.de                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <qobject.h>

#include "qtodo_instance_manager.h"

//QTodoInstanceManager<T>::std::map<QTodoInstanceManager<T>*,T*> instances;
//QTodoInstanceManager QTodoInstanceManagerObject::instance_manager;

QTodoInstanceManager::QTodoInstanceManager()
{
}

QTodoInstanceManager::~QTodoInstanceManager()
{
	//instances.erase(this);
}


void QTodoInstanceManager::add(QTodoInstanceManagerObject* o)
{
	qDebug("add");
	instance_objects.push_back(o);
}

void QTodoInstanceManager::remove(QTodoInstanceManagerObject* o)
{
	qDebug("remove");
	instance_objects.remove(o);
}

unsigned int QTodoInstanceManager::count()
{
	return instance_objects.size();
}

QTodoInstanceManagerObject::QTodoInstanceManagerObject()
{
	getInstanceManager()->add(this);
}

QTodoInstanceManagerObject::~QTodoInstanceManagerObject()
{
	getInstanceManager()->remove(this);
}

