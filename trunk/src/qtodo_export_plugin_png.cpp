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

#include <qbuffer.h>

#include "qtodo_export_plugin_png.h"
#include "qtodo_list_header_widget.h"
#include "qtodo_misc.h"

QTodoExportPluginRegistrar* QTodoExportPluginPNG::registrar = new QTodoExportPluginRegistrar(&QTodoExportPluginPNG::get);
QTodoExportPluginBase* QTodoExportPluginPNG::instance = 0;

QTodoExportPluginPNG::QTodoExportPluginPNG() : QTodoExportPluginBase()
{
	setEncoding(QTextStream::UnicodeUTF8);
	setBinary(true);
}

void QTodoExportPluginPNG::doExport()
{
	pixmap.resize(0,0);

	exportHead();
	exportBody();

	QBuffer buf(byte_result);
	buf.open(IO_WriteOnly);
	pixmap.save(&buf,"PNG",100);
	buf.close();
}

void QTodoExportPluginPNG::exportHead()
{
	/*QPixmap item_pixmap = QPixmap::grabWidget(header()); 

	int width = pixmap.width();
	if(width < item_pixmap.width())
		width = item_pixmap.width();
	int old_height = pixmap.height();

	pixmap.resize(width,pixmap.height()+item_pixmap.height());

	copyBlt(&pixmap,0,old_height,&item_pixmap,0,0,item_pixmap.width(),item_pixmap.height());*/
}

void QTodoExportPluginPNG::exportBody( )
{
	for(; listIterator().current(); ++listIterator())
	{
		if(QTodoItem* item = listIterator().item())
		{
			exportItem(*item);
		}
	}
}

void QTodoExportPluginPNG::exportItem(const QTodoItem& item)
{
	QPixmap item_pixmap = QPixmap::grabWidget(const_cast<QTodoItem*>(&item));

	int width = pixmap.width();
	if(width < item_pixmap.width())
		width = item_pixmap.width();
	int old_height = pixmap.height();

	pixmap.resize(width,pixmap.height()+item_pixmap.height()+5);

	QPixmap whitetmp(width,5);
	whitetmp.fill(item.list()->paletteBackgroundColor());

	copyBlt(&pixmap,0,old_height,&whitetmp,0,0,whitetmp.width(),whitetmp.height());
	copyBlt(&pixmap,0,old_height+5,&item_pixmap,0,0,item_pixmap.width(),item_pixmap.height());
}

QString QTodoExportPluginPNG::documentTypeName()
{
	return "PNG";
}

QString QTodoExportPluginPNG::pluginDescription()
{
	return QObject::tr("Save to portable-network-graphics file");
}

QString QTodoExportPluginPNG::documentTypeExtension()
{
	return "png";
}



