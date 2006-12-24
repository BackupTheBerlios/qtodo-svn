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

#include "qtodo_export_plugin_rtf.h"
#include "qtodo_misc.h"

QTodoExportPluginRegistrar* QTodoExportPluginRTF::registrar = new QTodoExportPluginRegistrar(&QTodoExportPluginRTF::get);
QTodoExportPluginBase* QTodoExportPluginRTF::instance = 0;

QTodoExportPluginRTF::QTodoExportPluginRTF() : QTodoExportPluginBase()
{
}

void QTodoExportPluginRTF::doExport()
{
	exportHead();
	exportBody();
}

void QTodoExportPluginRTF::exportHead()
{
	addLine("{\\rtf1\\ansi\\deff1\\adeflang1025");
	addLine("{\\fonttbl{\\f0\\froman\\fprq2\\fcharset0 Times New Roman;}{\\f1\\froman\\fprq2\\fcharset0 Times New Roman;}{\\f2\\fswiss\\fprq2\\fcharset0 Arial;}{\\f3\\froman\\fprq2\\fcharset0 Times New Roman;}{\\f4\\fmodern\\fprq1\\fcharset0 Courier New;}{\\f5\\fswiss\\fprq2\\fcharset0 Arial;}{\\f6\\fnil\\fprq0\\fcharset2 StarSymbol{\\*\\falt Arial Unicode MS};}{\\f7\\fnil\\fprq2\\fcharset0 Lucida Sans Unicode;}{\\f8\\fswiss\\fprq2\\fcharset0 Lucida Sans Unicode;}{\\f9\\fnil\\fprq2\\fcharset0 Mincho{\\*\\falt msmincho};}{\\f10\\fnil\\fprq2\\fcharset0 Tahoma;}{\\f11\\fnil\\fprq0\\fcharset0 Tahoma;}{\\f12\\fswiss\\fprq2\\fcharset0 Tahoma;}{\\f13\\fswiss\\fprq0\\fcharset0 Tahoma;}}");
	addLine("{\\colortbl;\\red0\\green0\\blue0;\\red102\\green102\\blue102;\\red128\\green128\\blue128;}");
	addLine("{\\stylesheet{\\s1\\cf0{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af1\\afs24\\lang255\\ltrch\\dbch\\af1\\langfe255\\hich\\f1\\fs24\\lang1031\\loch\\f1\\fs24\\lang1031\\snext1 Normal;}");
	addLine("{\\s2\\sb240\\sa120\\keepn\\cf0{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af10\\afs28\\lang255\\ltrch\\dbch\\af7\\langfe255\\hich\\f2\\fs28\\lang1031\\loch\\f2\\fs28\\lang1031\\sbasedon1\\snext3 Heading;}");
	addLine("{\\s3\\sa120\\cf0{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af1\\afs24\\lang255\\ltrch\\dbch\\af1\\langfe255\\hich\\f1\\fs24\\lang1031\\loch\\f1\\fs24\\lang1031\\sbasedon1\\snext3 Body Text;}");
	addLine("{\\s4\\sa120\\cf0{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af11\\afs24\\lang255\\ltrch\\dbch\\af1\\langfe255\\hich\\f1\\fs24\\lang1031\\loch\\f1\\fs24\\lang1031\\sbasedon3\\snext4 List;}");
	addLine("{\\s5\\sb120\\sa120\\cf0{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af11\\afs24\\lang255\\ai\\ltrch\\dbch\\af1\\langfe255\\hich\\f1\\fs24\\lang1031\\i\\loch\\f1\\fs24\\lang1031\\i\\sbasedon1\\snext5 caption;}");
	addLine("{\\s6\\cf0{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af11\\afs24\\lang255\\ltrch\\dbch\\af1\\langfe255\\hich\\f1\\fs24\\lang1031\\loch\\f1\\fs24\\lang1031\\sbasedon1\\snext6 Index;}");
	addLine("{\\s7\\aspalpha\\rtlch\\af12\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs24\\lang255\\loch\\fs24\\lang255\\snext7 Standard;}");
	addLine("{\\s8\\sa120\\aspalpha\\rtlch\\af12\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs24\\lang255\\loch\\fs24\\lang255\\sbasedon7\\snext8 Textk\\u246\\'3frper;}");
	addLine("{\\s9\\aspalpha\\rtlch\\af13\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs24\\lang255\\loch\\fs24\\lang255\\sbasedon8\\snext9 Liste;}");
	addLine("{\\s10\\tqc\\tx4818\\tqr\\tx9637\\aspalpha\\rtlch\\af12\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs24\\lang255\\loch\\fs24\\lang255\\sbasedon7\\snext10 Kopfzeile;}");
	addLine("{\\s11\\sb120\\sa120\\aspalpha\\rtlch\\af13\\afs20\\lang255\\ai\\ltrch\\dbch\\af8\\langfe255\\hich\\fs20\\lang255\\i\\loch\\fs20\\lang255\\i\\sbasedon7\\snext11 Beschriftung;}");
	addLine("{\\s12\\aspalpha\\rtlch\\af13\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs24\\lang255\\loch\\fs24\\lang255\\sbasedon7\\snext12 Verzeichnis;}");
	addLine("{\\s13\\aspalpha\\rtlch\\af4\\afs20\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs20\\lang255\\loch\\f4\\fs20\\lang255\\sbasedon7\\snext13 Vorformatierter Text;}");
	addLine("{\\s14\\sb240\\sa120\\keepn\\aspalpha\\rtlch\\af1\\afs28\\lang255\\ltrch\\dbch\\af9\\langfe255\\hich\\f5\\fs28\\lang255\\loch\\f5\\fs28\\lang255\\sbasedon7\\snext8 \\u220\\'3fberschrift;}");
	addLine("{\\s15\\cf0\\tqc\\tx4320\\tqr\\tx8640{\\*\\hyphen2\\hyphlead2\\hyphtrail2\\hyphmax0}\\rtlch\\af1\\afs24\\lang255\\ltrch\\dbch\\af1\\langfe255\\hich\\f1\\fs24\\lang1031\\loch\\f1\\fs24\\lang1031\\sbasedon1\\snext15 header;}");
	addLine("{\\*\\cs17\\rtlch\\af12\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs24\\lang255\\loch\\fs24\\lang255 Nummerierungszeichen;}");
	addLine("{\\*\\cs18\\rtlch\\af6\\afs18\\lang255\\ltrch\\dbch\\af6\\langfe255\\hich\\f6\\fs18\\lang255\\loch\\f6\\fs18\\lang255 Aufz\\u61668\\'3f\\u61544\\'3f\\u61548\\'3f\\u61557\\'3f\\u61550\\'3f\\u61543\\'3f\\u61555\\'3f\\u61562\\'3f\\u61541\\'3f\\u61545\\'3f\\u61539\\'3f\\u61544\\'3f\\u61541\\'3f\\u61550\\'3f\\u61499\\'3f;}");
	addLine("{\\*\\cs19\\cf2\\rtlch\\af12\\afs24\\lang255\\ltrch\\dbch\\af8\\langfe255\\hich\\fs16\\lang255\\b\\loch\\fs16\\lang255\\b TodoKopf;}");
	addLine("}");
	addLine("{\\info{\\creatim\\yr2006\\mo4\\dy10\\hr0\\min51}{\\revtim\\yr1601\\mo1\\dy1\\hr0\\min0}{\\printim\\yr1601\\mo1\\dy1\\hr0\\min0}{\\comment StarWriter}{\\vern6800}}\\deftab709");
	addLine("{\\*\\pgdsctbl");
	addLine("{\\pgdsc0\\pgdscuse195\\pgwsxn11905\\pghsxn16837\\marglsxn1134\\margrsxn1134\\margtsxn1134\\margbsxn1134\\headery0{\\*\\headeryb451\\headerxl0\\headerxr0\\headeryh734}{\\header \\pard\\plain \\s13\\qr\\aspalpha\\brdrb\\brdrs\\brdrw1\\brdrcf1\\brsp42{\\*\\brdrb\\brdlncol1\\brdlnin0\\brdlnout1\\brdlndist0}\\brsp42\\rtlch\\af4\\afs36\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs36\\lang255\\loch\\f4\\fs36\\lang255 {\\rtlch \\ltrch\\loch\\f4\\fs36\\lang255\\i0\\b0 "+unicodify(QObject::tr("Todo-List:"))+" "+name()+"\\tab {\\fs16 "+unicodify(QObject::tr("Standing:"))+" "+unicodify(QTodoMisc::dateTime2LocalString(QDateTime::currentDateTime()))+"}}");
	addLine("\\par }");
	addLine("\\pgdscnxt0 Standard;}}");
	addLine("{\\*\\pgdscno0}\\paperh16837\\paperw11905\\margl1134\\margr1134\\margt1134\\margb1134\\sectd\\sbknone\\pgwsxn11905\\pghsxn16837\\marglsxn1134\\margrsxn1134\\margtsxn1868\\margbsxn1134\\headery1134{\\header \\pard\\plain \\s13\\qr\\aspalpha\\brdrb\\brdrs\\brdrw1\\brdrcf1\\brsp42{\\*\\brdrb\\brdlncol1\\brdlnin0\\brdlnout1\\brdlndist0}\\brsp42\\rtlch\\af4\\afs36\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs36\\lang255\\loch\\f4\\fs36\\lang255 {\\rtlch \\ltrch\\loch\\f4\\fs36\\lang255\\i0\\b0 "+unicodify(QObject::tr("Todo-List:"))+" "+name()+"\\tab {\\fs16 "+unicodify(QObject::tr("Standing:"))+" "+unicodify(QTodoMisc::dateTime2LocalString(QDateTime::currentDateTime()))+"}}");
	addLine("\\par }");
	addLine("\\ftnbj\\ftnstart1\\ftnrstcont\\ftnnar\\aenddoc\\aftnrstcont\\aftnstart1\\aftnnrlc");
}

void QTodoExportPluginRTF::exportBody( )
{
	for(; listIterator().current(); ++listIterator())
	{
		if(QTodoItem* item = listIterator().item())
			exportItem(*item);
	}
	addLine("}");
}

void QTodoExportPluginRTF::exportItem(const QTodoItem& item)
{
	QString text = item.getTaskPlainText();
	text.replace("\n","}\n\\par \\pard\\plain \\ltrpar\\s13\\aspalpha\\ql\\rtlch\\af4\\afs20\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs20\\lang255\\loch\\f4\\fs20\\lang255 {\\rtlch \\ltrch\\loch"+QString().leftJustify(item.getDepth()*2+4,' ')+"}{\\rtlch \\ltrch\\loch\\f4\\fs20\\lang255\\i0\\b0 ");
	addLine("\\par \\pard\\plain \\ltrpar\\s13\\aspalpha\\ql\\rtlch\\af4\\afs20\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs20\\lang255\\loch\\f4\\fs20\\lang255{\\*\\cs19\\cf2\\rtlch\\ltrch\\dbch\\hich\\fs16\\lang255\\b\\loch\\fs16\\lang255\\b{\\rtlch \\ltrch\\loch"+QString().leftJustify(item.getDepth()*2,' ')+"}}{\\*\\cs19\\cf2{\\rtlch \\ltrch\\loch\\f1\\fs16\\lang255\\i0\\b - "+unicodify(QObject::tr("Priority"))+": "+unicodify(QTodo::priorityToString(item.getPriority()))+"  "+unicodify(QObject::tr("Status"))+": "+unicodify(QTodo::statusToString(item.getStatus()))+"}}");
	addLine("\\par \\pard\\plain \\ltrpar\\s13\\aspalpha\\ql\\rtlch\\af4\\afs20\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs20\\lang255\\loch\\f4\\fs20\\lang255{\\*\\cs19\\cf2\\rtlch\\ltrch\\dbch\\hich\\fs16\\lang255\\b\\loch\\fs16\\lang255\\b{\\rtlch \\ltrch\\loch"+QString().leftJustify(item.getDepth()*2+2,' ')+"}}{\\*\\cs19\\cf2{\\rtlch \\ltrch\\loch\\f1\\fs16\\lang255\\i0\\b "+unicodify(QObject::tr("Created"))+": "+unicodify(item.getCreatedLocalString())+"  "+(item.getDeadlineDateTime().isValid()?unicodify(QObject::tr("Deadline"))+": "+unicodify(item.getDeadlineLocalString()):"")+"}}");
	addLine("\\par \\pard\\plain \\ltrpar\\s13\\aspalpha\\ql\\rtlch\\af4\\afs20\\lang255\\ltrch\\dbch\\af4\\langfe255\\hich\\f4\\fs20\\lang255\\loch\\f4\\fs20\\lang255 {\\rtlch \\ltrch\\loch"+QString().leftJustify(item.getDepth()*2+4,' ')+"}{\\rtlch \\ltrch\\loch\\f4\\fs20\\lang255\\i0\\b0 "+unicodify(text)+"}");
}

QString QTodoExportPluginRTF::documentTypeName()
{
	return "RTF";
}

QString QTodoExportPluginRTF::pluginDescription()
{
	return QObject::tr("Save to rich-text-format file");
}

QString QTodoExportPluginRTF::documentTypeExtension()
{
	return "rtf";
}

const QString QTodoExportPluginRTF::unicodify(const QString& str)
{
	QString ustr;
	for(unsigned int i = 0; i < str.length(); ++i)
	{
		if(str[i] < char(128))
			ustr += str[i];
		else
			ustr += "\\u"+QString::number(str[i].unicode()).rightJustify(4,'0')+str[i];
	}
	return ustr;
}

