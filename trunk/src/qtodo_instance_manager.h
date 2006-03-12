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

#include <list>

#ifndef QTODO_INSTANCE_MANAGER
#define QTODO_INSTANCE_MANAGER

class QTodoInstanceManagerObject;

class QTodoInstanceManager
{
	public:
	QTodoInstanceManager();
	~QTodoInstanceManager();
	void add(QTodoInstanceManagerObject*);
	void remove(QTodoInstanceManagerObject*);
	
	unsigned int count();

	private:
	std::list<QTodoInstanceManagerObject*> instance_objects;
};

class QTodoInstanceManagerObject
{
	public:
	QTodoInstanceManagerObject();
	virtual ~QTodoInstanceManagerObject();

	protected:
	virtual QTodoInstanceManager* getInstanceManager() { return 0; };
	//static QTodoInstanceManager instance_manager;
};

#endif

