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

#include <qlayout.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistview.h>
 
#include "qtodo_item_widget.h"

QTodoEdit::QTodoEdit()
{
	QGridLayout* grid = new QGridLayout(this);
	QGridLayout* grid_edit = new QGridLayout(this);
	
	task = new QTextEdit(this);
	priority = new QComboBox(this);
	priority->insertItem(tr("N/A"),0);
	priority->insertItem(tr("High"),1);
	priority->insertItem(tr("Normal"),2);
	priority->insertItem(tr("Low"),3);	
	start = new QDateEdit(this);
	deadline = new QDateEdit(this);
	
	grid_edit->addWidget(new QLabel(tr("Priority"),this),0,0);
	grid_edit->addWidget(priority,0,1);
	grid_edit->addWidget(new QLabel(tr("Task"),this),1,0);
	grid_edit->addWidget(task,1,1);
	grid_edit->addWidget(new QLabel(tr("Start"),this),2,0);
	grid_edit->addWidget(start,2,1);
	grid_edit->addWidget(new QLabel(tr("Deadline"),this),3,0);
	grid_edit->addWidget(deadline,3,1);
	
	QPushButton* ok = new QPushButton(tr("Ok"),this);
	QPushButton* cancel = new QPushButton(tr("Cancel"),this);
	
	connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
	connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
	
	grid->addMultiCellLayout(grid_edit,0,0,0,1);
	grid->addWidget(ok,1,0);
	grid->addWidget(cancel,1,1);
}

void QTodoEdit::setTodo(QTodoItemWidget* item)
{
	task->setText(item->getTask());
	priority->setCurrentText(item->getPriority());
	start->setDate(QDate::fromString(item->getStart()));
	deadline->setDate(QDate::fromString(item->getDeadline()));
}

QString QTodoEdit::getTask()
{
	return task->text();
}

QString QTodoEdit::getPriority()
{
	return priority->currentText();
}

QString QTodoEdit::getStart()
{
	return start->date().toString();
}

QString QTodoEdit::getDeadline()
{
	return deadline->date().toString();
}

