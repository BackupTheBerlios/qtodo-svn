/***************************************************************************
 *   Copyright (C) 2006 by Tobias Glaesser                                 *
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

#include "qtodo_export_plugin_csv.h"
#include "qtodo_misc.h"

QTodoExportPluginRegistrar* QTodoExportPluginCSV::registrar = new QTodoExportPluginRegistrar(&QTodoExportPluginCSV::get);
QTodoExportPluginBase* QTodoExportPluginCSV::instance = 0;

QTodoExportPluginCSV::QTodoExportPluginCSV() : QTodoExportPluginBase()
{
	setEncoding(QTextStream::UnicodeUTF8);
}

void QTodoExportPluginCSV::doExport()
{
	elements_in_cur_row = 0;
	current_id = 0;
	exportHead();
	exportBody();
}

void QTodoExportPluginCSV::exportHead()
{
	addData(QObject::tr("Todo-List:")+" "+name()+" "+QObject::tr("Description: ")+" "+description()+" "+QObject::tr("Standing:")+" "+QTodoMisc::dateTime2LocalString(QDateTime::currentDateTime()));
	newRow();
	addData(QObject::tr("ID"));
	addData(QObject::tr("Parent"));
	addData(QObject::tr("Priority"));
	addData(QObject::tr("Status"));
	addData(QObject::tr("Deadline"));
	addData(QObject::tr("Assigned to"));
	addData(QObject::tr("Created"));
	addData(QObject::tr("Done"));
	addData(QObject::tr("Task"));
	newRow();
}

void QTodoExportPluginCSV::exportBody( )
{
	for(; listIterator().current(); ++listIterator())
	{
		if(QTodoItem* item = listIterator().item())
			exportItem(*item);
	}
}

void QTodoExportPluginCSV::exportItem(const QTodoItem& item)
{
	addData(QString::number(current_id));
	QTodoItem* parent = item.list()->parentTodo(&item);
	if(parent)
		addData(QString::number(item.list()->itemPos(item.list()->parentTodo(&item))+1));
	else
		addData("");
	addData(QTodo::priorityToString(item.getPriority()));
	addData(QTodo::statusToString(item.getStatus()));
	addData(item.getDeadlineLocalString());
	addData(item.getAgent());
	addData(item.getCreatedLocalString());
	addData(item.getDoneLocalString());
	addData(item.getTaskPlainText());
	newRow();
}

QString QTodoExportPluginCSV::documentTypeName()
{
	return "CSV";
}

QString QTodoExportPluginCSV::pluginDescription()
{
	return QObject::tr("Save to comma-separated-values file");
}

QString QTodoExportPluginCSV::documentTypeExtension()
{
	return "csv";
}

void QTodoExportPluginCSV::addData(const QString& data)
{
	if(elements_in_cur_row)
		add(",");
	QString s = data;
	s.replace("\"","\"\"");
	add('"'+s+'"');

	++elements_in_cur_row;
}

void QTodoExportPluginCSV::newRow()
{
	add("\n");
	elements_in_cur_row = 0;
	++current_id;
}

