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

/****************************************************************************
** $Id: qt/qdatetimeedit.cpp   3.3.5   edited Aug 31 12:13 $
**
** Implementation of date and time edit classes
**
** Created : 001103
**
** Copyright (C) 2000-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <private/qinternal_p.h>
#include <private/qrichtext_p.h>
#include <qrangecontrol.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qstyle.h>

#include "qtodo_datetimeedit.h"

#if defined(Q_WS_WIN)
#include <qt_windows.h>
#endif

#define QDATETIMEEDIT_HIDDEN_CHAR '0'

class QTodoNumberSection
{
public:
	QTodoNumberSection( int selStart = 0, int selEnd = 0, bool separat = TRUE, int actual = -1 )
			: selstart( selStart ), selend( selEnd ), act( actual ), sep( separat )
	{}
	int selectionStart() const { return selstart; }
	void setSelectionStart( int s ) { selstart = s; }
	int selectionEnd() const { return selend; }
	void setSelectionEnd( int s ) { selend = s; }
	int width() const { return selend - selstart; }
	int index() const { return act; }
	bool separator() const { return sep; }
	Q_DUMMY_COMPARISON_OPERATOR( QTodoNumberSection )
private:
int selstart :12;
int selend	 :12;
int act	 :7;
bool sep	 :1;
};

static QString	*lDateSep = 0;
static QString	*lTimeSep = 0;
static bool	lAMPM	  = FALSE;
static QString	*lAM	  = 0;
static QString	*lPM	  = 0;
static QTodoDateEdit::Order	lOrder = QTodoDateEdit::YMD;
static int refcount = 0;

static void cleanup()
{
	delete lDateSep;
	lDateSep = 0;
	delete lTimeSep;
	lTimeSep = 0;
	delete lAM;
	lAM = 0;
	delete lPM;
	lPM = 0;
}

/*!
\internal
try to get the order of DMY and the date/time separator from the locale settings
*/
static void readLocaleSettings()
{
	int dpos, mpos, ypos;
	cleanup();

	lDateSep = new QString();
	lTimeSep = new QString();

#if defined(Q_WS_WIN)
	QT_WA( {
	           TCHAR data[10];
	           GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDATE, data, 10 );
	           *lDateSep = QString::fromUcs2( (ushort*)data );
	           GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIME, data, 10 );
	           *lTimeSep = QString::fromUcs2( (ushort*)data );
	           GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_ITIME, data, 10 );
	           lAMPM = QString::fromUcs2( (ushort*)data ).toInt()==0;
	           GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_S1159, data, 10 );
	           QString am = QString::fromUcs2( (ushort*)data );
	           if ( !am.isEmpty() )
	           lAM = new QString( am );
	           GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_S2359, data, 10 );
	           QString pm = QString::fromUcs2( (ushort*)data );
	           if ( !pm.isEmpty()  )
		           lPM = new QString( pm );
		       } , {
		           char data[10];
		           GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_SDATE, (char*)&data, 10 );
		           *lDateSep = QString::fromLocal8Bit( data );
		           GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_STIME, (char*)&data, 10 );
		           *lTimeSep = QString::fromLocal8Bit( data );
		           GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_ITIME, (char*)&data, 10 );
		           lAMPM = QString::fromLocal8Bit( data ).toInt()==0;
		           GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_S1159, (char*)&data, 10 );
		           QString am = QString::fromLocal8Bit( data );
		           if ( !am.isEmpty() )
			           lAM = new QString( am );
			           GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_S2359, (char*)&data, 10 );
			           QString pm = QString::fromLocal8Bit( data );
			           if ( !pm.isEmpty() )
				           lPM = new QString( pm );
				       } );
#else
	*lDateSep = "-";
	*lTimeSep = ":";
#endif
	QString d = QDate( 1999, 11, 22 ).toString( Qt::LocalDate );
	dpos = d.find( "22" );
	mpos = d.find( "11" );
	ypos = d.find( "99" );
	if ( dpos > -1 && mpos > -1 && ypos > -1 )
	{
		// test for DMY, MDY, YMD, YDM
		if ( dpos < mpos && mpos < ypos )
		{
			lOrder = QTodoDateEdit::DMY;
		}
		else if ( mpos < dpos && dpos < ypos )
		{
			lOrder = QTodoDateEdit::MDY;
		}
		else if ( ypos < mpos && mpos < dpos )
		{
			lOrder = QTodoDateEdit::YMD;
		}
		else if ( ypos < dpos && dpos < mpos )
		{
			lOrder = QTodoDateEdit::YDM;
		}
		else
		{
			// cannot determine the dateformat - use the default
			return;
		}

		// this code needs to change if new formats are added

#ifndef Q_WS_WIN
		QString sep = d.mid( QMIN( dpos, mpos ) + 2, QABS( dpos - mpos ) - 2 );
		if ( d.contains( sep ) == 2 )
		{
			*lDateSep = sep;
		}
#endif

	}

#ifndef Q_WS_WIN
	QString t = QTime( 11, 22, 33 ).toString( Qt::LocalDate );
	dpos = t.find( "11" );
	mpos = t.find( "22" );
	ypos = t.find( "33" );
	// We only allow hhmmss
	if ( dpos > -1 && dpos < mpos && mpos < ypos )
	{
		QString sep = t.mid( dpos + 2, mpos - dpos - 2 );
		if ( sep == t.mid( mpos + 2, ypos - mpos - 2 ) )
		{
			*lTimeSep = sep;
		}
	}
#endif
}

static QTodoDateEdit::Order localOrder()
{
	if ( !lDateSep )
	{
		readLocaleSettings();
	}
	return lOrder;
}

static QString localDateSep()
{
	if ( !lDateSep )
	{
		readLocaleSettings();
	}
	return *lDateSep;
}

static QString localTimeSep()
{
	if ( !lTimeSep )
	{
		readLocaleSettings();
	}
	return *lTimeSep;
}

class QTodoDateTimeEditorPrivate
{
public:
	QTodoDateTimeEditorPrivate()
			: frm( TRUE ),
			parag( new QTextParagraph( 0, 0, 0, FALSE ) ),
			focusSec(0)
	{
		parag->formatter()->setWrapEnabled( FALSE );
		cursor = new QTextCursor( 0 );
		cursor->setParagraph( parag );
		offset = 0;
		sep = localDateSep();
		refcount++;
	}
	~QTodoDateTimeEditorPrivate()
	{
		delete parag;
		delete cursor;
		if ( !--refcount )
			cleanup();
	}

	void appendSection( const QTodoNumberSection& sec )
	{
		sections.append( sec );

	}
	void clearSections()
	{
		sections.clear();
	}
	void setSectionSelection( int sec, int selstart, int selend )
	{
		if ( sec < 0 || sec > (int)sections.count() )
			return;
		sections[sec].setSelectionStart( selstart );
		sections[sec].setSelectionEnd( selend );
	}
uint sectionCount() const { return (uint)sections.count(); }
	void setSeparator( const QString& s ) { sep = s; }
	QString separator() const { return sep; }

	void setFrame( bool f ) { frm = f; }
	bool frame() const { return frm; }

	int focusSection() const { return focusSec; }
	int section( const QPoint& p )
	{
		cursor->place( p + QPoint( offset, 0 ), parag );
		int idx = cursor->index();
		for ( uint i = 0; i < sections.count(); ++i )
		{
			if ( idx >= sections[i].selectionStart() &&
			        idx <= sections[i].selectionEnd() )
				return i;
		}
		return -1;
	}
	QTodoNumberSection section( int idx ) const
	{
		return sections[idx];
	}
	bool setFocusSection( int idx )
	{
		if ( idx > (int)sections.count()-1 || idx < 0 )
			return FALSE;
		if ( idx != focusSec )
		{
			focusSec = idx;
			applyFocusSelection();
			return TRUE;
		}
		return FALSE;
	}

	bool inSectionSelection( int idx )
	{
		for ( uint i = 0; i < sections.count(); ++i )
		{
			if ( idx >= sections[i].selectionStart() &&
			        idx <= sections[i].selectionEnd() )
				return TRUE;
		}
		return FALSE;
	}

	void paint( const QString& txt, bool focus, QPainter& p,
	            const QColorGroup& cg, const QRect& rect, QStyle& style )
	{
		int fw = 0;
		if ( frm )
			fw = style.pixelMetric(QStyle::PM_DefaultFrameWidth);

		parag->truncate( 0 );
		parag->append( txt );
		if ( !focus )
			parag->removeSelection( QTextDocument::Standard );
		else
		{
			applyFocusSelection();
		}

		/* color all QDATETIMEEDIT_HIDDEN_CHAR chars to background color */
		QTextFormat *fb = parag->formatCollection()->format( p.font(),
		                  cg.base() );
		QTextFormat *nf = parag->formatCollection()->format( p.font(),
		                  cg.text() );
		for ( uint i = 0; i < txt.length(); ++i )
		{
			parag->setFormat( i, 1, nf );
			if ( inSectionSelection( i ) )
				continue;
			if ( txt.at(i) == QDATETIMEEDIT_HIDDEN_CHAR )
				parag->setFormat( i, 1, fb );
			else
				parag->setFormat( i, 1, nf );
		}
		fb->removeRef();
		nf->removeRef();

		QRect r( rect.x(), rect.y(), rect.width()  * (/* 2 */+ fw ), rect.height() );
		parag->pseudoDocument()->docRect = r;
		parag->invalidate(0);
		parag->format();

		int xoff = /*2*/ + -2 + fw - offset;
		int yoff = 0 + ( rect.height() - parag->rect().height() + 1 ) / 2;
		/*if ( yoff < 0 )
			yoff = 0;*/

		p.translate( xoff, yoff );
		parag->paint( p, cg, 0, TRUE );
		if ( frm )
			p.translate( -xoff, -yoff );
	}

void resize( const QSize& size ) { sz = size; }

	int mapSection( int sec )
	{
		return sections[sec].index();
	}

protected:
	void applyFocusSelection()
	{
		if ( focusSec > -1 )
		{
			int selstart = sections[ focusSec ].selectionStart();
			int selend = sections[ focusSec ].selectionEnd();
			parag->setSelection( QTextDocument::Standard, selstart, selend );
			parag->format();
			if ( parag->at( selstart )->x < offset ||
			        parag->at( selend )->x + parag->string()->width( selend ) > offset + sz.width() )
			{
				//offset = parag->at( selstart )->x;
			}
		}
	}
private:
	bool frm;
	QTextParagraph *parag;
	QTextCursor *cursor;
	QSize sz;
	int focusSec;
	QValueList< QTodoNumberSection > sections;
	QString sep;
	int offset;
};

#include <qdrawutil.h>

class QDateTimeSpinWidget : public QSpinWidget
{
public:
	QDateTimeSpinWidget( QWidget *parent, const char *name )
			: QSpinWidget( parent, name ), up_pressed(false), down_pressed(false)
	{}

	void enabledChange(bool notenabled)
	{
		QTodoDateEdit *de = ::qt_cast<QTodoDateEdit*>(parentWidget());
		if (de && !notenabled)
		{
			setUpEnabled(de->date() < de->maxValue());
			setDownEnabled(de->date() > de->minValue());
		}
		else
		{
			setUpEnabled(!notenabled);
			setDownEnabled(!notenabled);
		}
	}

    void mousePressEvent( QMouseEvent *e )
	{
		up_pressed = false;
		down_pressed = false;
    		if(e->button() == LeftButton )
		{
			if(upRect().contains(e->pos()))
				up_pressed  = true;
			else if(downRect().contains(e->pos()))
				down_pressed = true;
		}

		QSpinWidget::mousePressEvent(e);
	}

    void mouseReleaseEvent( QMouseEvent *e )
	{
		up_pressed = false;
		down_pressed = false;

		QSpinWidget::mouseReleaseEvent(e);
		repaint();
	}

	bool up_pressed;
	bool down_pressed;

void paintEvent( QPaintEvent * )
{
    QPainter p( this );

    QStyle::SFlags flags = QStyle::Style_Default;
    if (isEnabled())
	flags |= QStyle::Style_Enabled;
    if (hasFocus() || focusProxy() && focusProxy()->hasFocus())
	flags |= QStyle::Style_HasFocus;

    QStyle::SCFlags active;
/*    if ( d->theButton & 1 )
	active = QStyle::SC_SpinWidgetDown;
    else if ( d->theButton & 2 )
	active = QStyle::SC_SpinWidgetUp;
    else*/
	if(up_pressed)
		active = QStyle::SC_SpinWidgetUp;
	else if(down_pressed)
		active = QStyle::SC_SpinWidgetDown;
	else
		active = QStyle::SC_None;

    QRect fr = QStyle::visualRect(
	style().querySubControlMetrics( QStyle::CC_SpinWidget, this,
					QStyle::SC_SpinWidgetButtonField /*Frame */), this );

	//if ( controls & SC_SpinWidgetUp ) {
//	     flags = QStyle::Style_Default | QStyle::Style_Enabled;
	    /*if (active == SC_SpinWidgetUp ) {
		flags |= Style_On;
		flags |= Style_Sunken;
	    } else*/
		//flags |= QStyle::Style_Plain; //Raised
/*	    if ( sw->buttonSymbols() == QSpinWidget::PlusMinus )
		pe = PE_SpinWidgetPlus;
	    else
		pe = PE_SpinWidgetUp;*/

	    //QRect re = sw->upRect();
	    //QColorGroup ucg = sw->isUpEnabled() ? cg : sw->palette().disabled();
	    //style().drawPrimitive(QStyle::PE_ButtonBevel, &p, fr, colorGroup() /*ucg*/, flags);
	   //style().drawPrimitive(pe, &p, fr, colorGroup() /*ucg*/, flags);
	//}

    style().drawComplexControl( QStyle::CC_SpinWidget, &p, this,
				fr, colorGroup(),
				flags,
				(uint)QStyle::SC_All, //SpinWidgetButtonField, //SC_ALL
				active );

	fr = QStyle::visualRect(
	style().querySubControlMetrics( QStyle::CC_SpinWidget, this,
					QStyle::SC_SpinWidgetFrame ), this );
	qDrawPlainRect( &p, fr, Qt::white, 1);
}

protected:
#ifndef QT_NO_WHEELEVENT
	void wheelEvent( QWheelEvent *e )
	{
		QTodoDateTimeEditor *editor = (QTodoDateTimeEditor*)editWidget()->qt_cast( "QTodoDateTimeEditor" );
		Q_ASSERT( editor );
		if ( !editor )
			return;

		int section = editor->sectionAt( e->pos() );
		editor->setFocusSection( section );

		if ( section == -1 )
			return;
		QSpinWidget::wheelEvent( e );
	}
#endif
};

/*!
    Constructs an empty datetime editor with parent \a parent and
    called \a name.
*/
QTodoDateTimeEditor::QTodoDateTimeEditor( QTodoDateTimeEditBase * widget, QWidget *parent,
        const char * name )
		: QWidget( parent, name, WNoAutoErase )
{
	d = new QTodoDateTimeEditorPrivate();
	cw = widget;
	init();
}

void QTodoDateTimeEditor::focusInEvent(QFocusEvent*)
{
	emit focusInSignal();
}

void QTodoDateTimeEditor::focusOutEvent(QFocusEvent*)
{
	emit focusOutSignal();
}

/*!
    Destroys the object and frees any allocated resources.
*/

QTodoDateTimeEditor::~QTodoDateTimeEditor()
{
	delete d;
}

/*! \internal
 
*/

void QTodoDateTimeEditor::init()
{
	setBackgroundMode( PaletteBase );
	setFocusSection( -1 );
	installEventFilter( this );
	setFocusPolicy( WheelFocus );
}


/*! \reimp
 
*/

bool QTodoDateTimeEditor::event( QEvent *e )
{
	if ( e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut )
	{
		if ( e->type() == QEvent::FocusOut )
			qApp->sendEvent( cw, e );
		update( rect() );
	}
	else if ( e->type() == QEvent::AccelOverride )
	{
		QKeyEvent* ke = (QKeyEvent*) e;
		if ( ke->state() == AltButton )
		{
			ke->ignore();
			QWidget::keyPressEvent(ke);	
			return true;
		}

		switch ( ke->key() )
		{
		case Key_Delete:
		case Key_Backspace:
		case Key_Up:
		case Key_Down:
		case Key_Left:
		case Key_Right:
			ke->accept();
		default:
			break;
		}
	}
	return QWidget::event( e );
}

/*! \reimp
 
*/

void QTodoDateTimeEditor::resizeEvent( QResizeEvent *e )
{
	d->resize( e->size() );
	QWidget::resizeEvent( e );
}


/*! \reimp
 
*/

void QTodoDateTimeEditor::paintEvent( QPaintEvent * )
{
	QString txt;
	for ( uint i = 0; i < d->sectionCount(); ++i )
	{
		txt += cw->sectionFormattedText( i );
		if ( i < d->sectionCount()-1 )
		{
			if ( d->section( i+1 ).separator() )
				txt += d->separator();
			else
				txt += " ";
		}
	}

	QSharedDoubleBuffer buffer( this );
	const QBrush &bg =
	    colorGroup().brush( isEnabled() ? QColorGroup::Base : QColorGroup::Background );
	buffer.painter()->fillRect( 0, 0, width(), height(), bg );
	d->paint( txt, hasFocus(), *buffer.painter(), colorGroup(), rect(),
	         style() );
	buffer.end();
}


/*!
    Returns the section index at point \a p.
*/
int QTodoDateTimeEditor::sectionAt( const QPoint &p )
{
	return d->section( p );
}

int QTodoDateTimeEditor::mapSection( int sec )
{
	return d->mapSection( sec );
}


/*! \reimp
 
*/

void QTodoDateTimeEditor::mousePressEvent( QMouseEvent *e )
{
	QPoint p( e->pos().x(), 0 );
	int sec = sectionAt( p );
	if ( sec != -1 )
	{
		cw->setFocusSection( sec );
		repaint( rect(), FALSE );
	}
}

/*! \reimp
 
*/
bool QTodoDateTimeEditor::eventFilter( QObject *o, QEvent *e )
{
	if ( o == this )
	{
		if ( e->type() == QEvent::KeyPress )
		{
			QKeyEvent *ke = (QKeyEvent*)e;

			if(ke->state() == AltButton)
			{
				ke->ignore();
				QWidget::keyPressEvent(ke);
				return false;
			}

			switch ( ke->key() )
			{
			case Key_Right:
				if ( d->focusSection() < (int)d->sectionCount()-1 )
				{
					if ( cw->setFocusSection( focusSection()+1 ) )
						repaint( rect(), FALSE );
				}
				return TRUE;
			case Key_Left:
				if ( d->focusSection() > 0 )
				{
					if ( cw->setFocusSection( focusSection()-1 ) )
						repaint( rect(), FALSE );
				}
				return TRUE;
			case Key_Up:
				cw->stepUp();
				return TRUE;
			case Key_Down:
				cw->stepDown();
				return TRUE;
			case Key_Backspace:
				if ( ::qt_cast<QTodoDateEdit*>(cw) )
					((QTodoDateEdit*)cw)->removeFirstNumber( d->focusSection() );
				else if ( ::qt_cast<QTodoTimeEdit*>(cw) )
					((QTodoTimeEdit*)cw)->removeFirstNumber( d->focusSection() );
				return TRUE;
			case Key_Delete:
				cw->removeLastNumber( d->focusSection() );
				return TRUE;
			case Key_Tab:
			case Key_BackTab:
				{
					if ( ke->state() == Qt::ControlButton )
						return FALSE;

					QWidget *w = this;
					bool hadDateEdit = FALSE;
					while ( w )
					{
						if ( ::qt_cast<QDateTimeSpinWidget*>(w) && qstrcmp( w->name(), "qt_spin_widget" ) != 0 ||
						        ::qt_cast<QTodoDateTimeEdit*>(w) )
							break;
						hadDateEdit = hadDateEdit || ::qt_cast<QTodoDateEdit*>(w);
						w = w->parentWidget();
					}

					if ( w )
					{
						if ( !::qt_cast<QTodoDateTimeEdit*>(w) )
						{
							w = w->parentWidget();
						}
						else
						{
							QTodoDateTimeEdit *ed = (QTodoDateTimeEdit*)w;
							if ( hadDateEdit && ke->key() == Key_Tab )
							{
								ed->timeEdit()->setFocus();
								return TRUE;
							}
							else if ( !hadDateEdit && ke->key() == Key_BackTab )
							{
								ed->dateEdit()->setFocus();
								return TRUE;
							}
							else
							{
								while ( w && !::qt_cast<QTodoDateTimeEdit*>(w) )
									w = w->parentWidget();
							}
						}

						qApp->sendEvent( w, e );
						return TRUE;
					}
				}
				break;
			default:
				QString txt = ke->text().lower();
				if ( !txt.isEmpty() && !separator().isEmpty() && txt[0] == separator()[0] )
				{
					// do the same thing as KEY_RIGHT when the user presses the separator key
					if ( d->focusSection() < 2 )
					{
						if ( cw->setFocusSection( focusSection()+1 ) )
							repaint( rect(), FALSE );
					}
					return TRUE;
				}
				else if ( !txt.isEmpty() && ::qt_cast<QTodoTimeEdit*>(cw) && focusSection() == (int) d->sectionCount()-1 )
				{
					// the first character of the AM/PM indicator toggles if the section has focus
					QTodoTimeEdit *te = (QTodoTimeEdit*)cw;
					QTime time = te->time();
					if ( lAMPM && lAM && lPM && (te->display()&QTodoTimeEdit::AMPM) )
					{
						if ( txt[0] == (*lAM).lower()[0] && time.hour() >= 12 )
						{
							time.setHMS( time.hour()-12, time.minute(), time.second(), time.msec() );
							te->setTime( time );
						}
						else if ( txt[0] == (*lPM).lower()[0] && time.hour() < 12 )
						{
							time.setHMS( time.hour()+12, time.minute(), time.second(), time.msec() );
							te->setTime( time );
						}
					}
				}

				int num = txt[0].digitValue();
				if ( num != -1 )
				{
					cw->addNumber( d->focusSection(), num );
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


/*!
    Appends the number section \a sec to the editor.
*/

void QTodoDateTimeEditor::appendSection( const QTodoNumberSection& sec )
{
	d->appendSection( sec );
}

/*!
    Removes all sections from the editor.
*/

void QTodoDateTimeEditor::clearSections()
{
	d->clearSections();
}

/*!
    Sets the selection of \a sec to start at \a selstart and end at \a
    selend.
*/

void QTodoDateTimeEditor::setSectionSelection( int sec, int selstart, int selend )
{
	d->setSectionSelection( sec, selstart, selend );
}

/*!
    Sets the separator for all numbered sections to \a s. Note that
    currently, only the first character of \a s is used.
*/

void QTodoDateTimeEditor::setSeparator( const QString& s )
{
	d->setSeparator( s );
	update();
}


/*!
    Returns the editor's separator.
*/

QString QTodoDateTimeEditor::separator() const
{
	return d->separator();
}

/*!
    Returns the number of the section that has focus.
*/

int QTodoDateTimeEditor::focusSection() const
{
	return d->focusSection();
}


/*!
    Sets the focus to section \a sec. If \a sec does not exist,
    nothing happens.
*/

bool QTodoDateTimeEditor::setFocusSection( int sec )
{
	return d->setFocusSection( sec );
}

/*! \class QTodoDateTimeEditBase
    \brief The QTodoDateTimeEditBase class provides an abstraction for date and edit editors.
 
    Small abstract class that provides some functions that are common
    for both QTodoDateEdit and QTodoTimeEdit. It is used internally by
    QTodoDateTimeEditor.
*/

/*!
    \fn QTodoDateTimeEditBase::QTodoDateTimeEditBase(QWidget *, const char*)
    \internal
*/

/*!
    \fn QTodoDateTimeEditBase::setFocusSection(int)
    \internal
*/

/*! \fn QString QTodoDateTimeEditBase::sectionFormattedText( int sec )
    \internal
 
  Pure virtual function which returns the formatted text of section \a
  sec.
 
*/

/*! \fn void QTodoDateTimeEditBase::stepUp()
    \internal
 
  Pure virtual slot which is called whenever the user increases the
  number in a section by pressing the widget's arrow buttons or the
  keyboard's arrow keys.
*/

/*! \fn void QTodoDateTimeEditBase::stepDown()
    \internal
 
  Pure virtual slot which is called whenever the user decreases the
  number in a section by pressing the widget's arrow buttons or the
  keyboard's arrow keys.
 
*/

/*! \fn void QTodoDateTimeEditBase::addNumber( int sec, int num )
    \internal
 
  Pure virtual function which is called whenever the user types a number.
  \a sec indicates the section where the number should be added. \a
  num is the number that was pressed.
*/

/*! \fn void QTodoDateTimeEditBase::removeLastNumber( int sec )
    \internal
 
  Pure virtual function which is called whenever the user tries to
  remove the last number from \a sec by pressing the delete key.
*/

////////////////

class QTodoDateEditPrivate
{
public:
	int y;
	int m;
	int d;
	// remebers the last entry for the day.
	// if the day is 31 and you cycle through the months,
	// the day will be 31 again if you reach a month with 31 days
	// otherwise it will be the highest day in the month
	int dayCache;
	int yearSection;
	int monthSection;
	int daySection;
	QTodoDateEdit::Order ord;
	bool overwrite;
	bool adv;
	int timerId;
	bool typing;
	QDate min;
	QDate max;
	bool changed;
	QTodoDateTimeEditor *ed;
	QSpinWidget *controls;
};


/*!
    \class QTodoDateEdit qdatetimeedit.h
    \brief The QTodoDateEdit class provides a date editor.
 
    \ingroup advanced
    \ingroup time
    \mainclass
 
    QTodoDateEdit allows the user to edit dates by using the keyboard or
    the arrow keys to increase/decrease date values. The arrow keys
    can be used to move from section to section within the QTodoDateEdit
    box. Dates appear in accordance with the local date/time settings
    or in year, month, day order if the system doesn't provide this
    information. It is recommended that the QTodoDateEdit be initialised
    with a date, e.g.
 
    \code
    QTodoDateEdit *dateEdit = new QTodoDateEdit( QDate::currentDate(), this );
    dateEdit->setRange( QDate::currentDate().addDays( -365 ),
			QDate::currentDate().addDays(  365 ) );
    dateEdit->setOrder( QTodoDateEdit::MDY );
    dateEdit->setAutoAdvance( TRUE );
    \endcode
 
    Here we've created a new QTodoDateEdit object initialised with today's
    date and restricted the valid date range to today plus or minus
    365 days. We've set the order to month, day, year. If the auto
    advance property is TRUE (as we've set it here) when the user
    completes a section of the date, e.g. enters two digits for the
    month, they are automatically taken to the next section.
 
    The maximum and minimum values for a date value in the date editor
    default to the maximum and minimum values for a QDate. You can
    change this by calling setMinValue(), setMaxValue() or setRange().
 
    Terminology: A QTodoDateEdit widget comprises three 'sections', one
    each for the year, month and day. You can change the separator
    character using QTodoDateTimeEditor::setSeparator(), by default the
    separator will be taken from the systems settings. If that is
    not possible, it defaults to "-".
 
    \img datetimewidgets.png Date Time Widgets
 
    \sa QDate QTodoTimeEdit QTodoDateTimeEdit
*/

/*!
    \enum QTodoDateEdit::Order
 
    This enum defines the order in which the sections that comprise a
    date appear.
    \value MDY month-day-year
    \value DMY day-month-year
    \value YMD year-month-day (the default)
    \value YDM year-day-month (included for completeness; but should
    not be used)
*/

/*!
    \enum QTodoTimeEdit::Display
 
    This enum defines the sections that comprise a time
 
    \value Hours The hours section
    \value Minutes The minutes section
    \value Seconds The seconds section
    \value AMPM The AM/PM section
 
    The values can be or'ed together to show any combination.
*/

/*!
    Constructs an empty date editor which is a child of \a parent and
    called name \a name.
*/

QTodoDateEdit::QTodoDateEdit( QWidget * parent, const char * name )
		: QTodoDateTimeEditBase( parent, name )
{
	init();
	updateButtons();
}

/*!
    \overload
 
    Constructs a date editor with the initial value \a date, parent \a
    parent and called \a name.
 
    The date editor is initialized with \a date.
*/

QTodoDateEdit::QTodoDateEdit( const QDate& date, QWidget * parent, const char * name )
		: QTodoDateTimeEditBase( parent, name )
{
	init();
	setDate( date );
}

/*! \internal
*/
void QTodoDateEdit::init()
{
	d = new QTodoDateEditPrivate();
	d->controls = new QDateTimeSpinWidget( this, qstrcmp( name(), "qt_datetime_dateedit" ) == 0 ? "qt_spin_widget" : "date edit controls" );
	d->controls->move(d->controls->x()+1,d->controls->y()-1);
	d->ed = new QTodoDateTimeEditor( this, d->controls, "date editor" );
	d->controls->setEditWidget( d->ed );
	setFocusProxy( d->ed );
	connect( d->controls, SIGNAL( stepUpPressed() ), SLOT( stepUp() ) );
	connect( d->controls, SIGNAL( stepDownPressed() ), SLOT( stepDown() ) );
	connect( this, SIGNAL( valueChanged(const QDate&) ),
	         SLOT( updateButtons() ) );
	connect( d->ed, SIGNAL(focusInSignal()),this,SLOT(slotFocusInSignal()));
	connect( d->ed, SIGNAL(focusOutSignal()),this,SLOT(slotFocusOutSignal()));

	d->ed->appendSection( QTodoNumberSection( 0,4 ) );
	d->ed->appendSection( QTodoNumberSection( 5,7 ) );
	d->ed->appendSection( QTodoNumberSection( 8,10 ) );

	d->yearSection = -1;
	d->monthSection = -1;
	d->daySection = -1;

	d->y = 0;
	d->m = 0;
	d->d = 0;
	d->dayCache = 0;
	setOrder( localOrder() );
	setFocusSection( 0 );
	d->overwrite = TRUE;
	d->adv = FALSE;
	d->timerId = 0;
	d->typing = FALSE;
	d->min = QDate( 1752, 9, 14 );
	d->max = QDate( 8000, 12, 31 );
	d->changed = FALSE;

	setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );

	m_spin_width = d->controls->upRect().width();

	refcount++;
}

/*!
    Destroys the object and frees any allocated resources.
*/

QTodoDateEdit::~QTodoDateEdit()
{
	delete d;
	if ( !--refcount )
		cleanup();
}

/*!
    \property QTodoDateEdit::minValue
 
    \brief the editor's minimum value
 
    Setting the minimum date value is equivalent to calling
    QTodoDateEdit::setRange( \e d, maxValue() ), where \e d is the minimum
    date. The default minimum date is 1752-09-14.
 
    \sa maxValue setRange()
*/

QDate QTodoDateEdit::minValue() const
{
	return d->min;
}

/*!
    \property QTodoDateEdit::maxValue
 
    \brief the editor's maximum value
 
    Setting the maximum date value for the editor is equivalent to
    calling QTodoDateEdit::setRange( minValue(), \e d ), where \e d is the
    maximum date. The default maximum date is 8000-12-31.
 
    \sa minValue setRange()
*/

QDate QTodoDateEdit::maxValue() const
{
	return d->max;
}


/*!
    Sets the valid input range for the editor to be from \a min to \a
    max inclusive. If \a min is invalid no minimum date will be set.
    Similarly, if \a max is invalid no maximum date will be set.
*/

void QTodoDateEdit::setRange( const QDate& min, const QDate& max )
{
	if ( min.isValid() )
		d->min = min;
	if ( max.isValid() )
		d->max = max;
}

/*!
    Sets the separator to \a s. Note that currently only the first
    character of \a s is used.
*/

void QTodoDateEdit::setSeparator( const QString& s )
{
	d->ed->setSeparator( s );
}

/*!
    Returns the editor's separator.
*/

QString QTodoDateEdit::separator() const
{
	return d->ed->separator();
}


/*!
    Enables/disables the push buttons according to the min/max date
    for this widget.
*/

void QTodoDateEdit::updateButtons()
{
	if ( !isEnabled() )
		return;

	bool upEnabled = date() < maxValue();
	bool downEnabled = date() > minValue();

	d->controls->setUpEnabled( upEnabled );
	d->controls->setDownEnabled( downEnabled );
}

/*! \reimp
 */
void QTodoDateEdit::resizeEvent( QResizeEvent * )
{
	d->controls->resize( width(), height() );
}

/*! \reimp
 
*/
QSize QTodoDateEdit::sizeHint() const
{
	constPolish();
	QFontMetrics fm( font() );
//	int fw = style().pixelMetric( QStyle::PM_DefaultFrameWidth, this );
	int h = QMAX( fm.lineSpacing(), 14 ) ; //+ 2;
	d->controls->arrange();
	int w = /*2 +*/ fm.width( '9' ) * 8 + fm.width( d->ed->separator() ) * 2 + m_spin_width /*+ fw * 4*/;

	return QSize( w, QMAX(h + 0 /*fw * 2*/,16) ).expandedTo( QApplication::globalStrut() );
}

/*! \reimp
 
*/
QSize QTodoDateEdit::minimumSizeHint() const
{
	return sizeHint();
}


/*!
    Returns the formatted number for section \a sec. This will
    correspond to either the year, month or day section, depending on
    the current display order.
 
    \sa setOrder()
*/

QString QTodoDateEdit::sectionFormattedText( int sec )
{
	QString txt;
	txt = sectionText( sec );
	if ( d->typing && sec == d->ed->focusSection() )
		d->ed->setSectionSelection( sec, sectionOffsetEnd( sec ) - txt.length(),
		                            sectionOffsetEnd( sec ) );
	else
		d->ed->setSectionSelection( sec, sectionOffsetEnd( sec ) - sectionLength( sec ),
		                            sectionOffsetEnd( sec ) );
	txt = txt.rightJustify( sectionLength( sec ), QDATETIMEEDIT_HIDDEN_CHAR );
	return txt;
}


/*!
    Returns the desired length (number of digits) of section \a sec.
    This will correspond to either the year, month or day section,
    depending on the current display order.
 
    \sa setOrder()
*/

int QTodoDateEdit::sectionLength( int sec ) const
{
	int val = 0;
	if ( sec == d->yearSection )
	{
		val = 4;
	}
	else if ( sec == d->monthSection )
	{
		val = 2;
	}
	else if ( sec == d->daySection )
	{
		val = 2;
	}
	return val;
}

/*!
    Returns the text of section \a sec. This will correspond to either
    the year, month or day section, depending on the current display
    order.
 
    \sa setOrder()
*/

QString QTodoDateEdit::sectionText( int sec ) const
{
	int val = 0;
	if ( sec == d->yearSection )
	{
		val = d->y;
	}
	else if ( sec == d->monthSection )
	{
		val = d->m;
	}
	else if ( sec == d->daySection )
	{
		val = d->d;
	}
	return QString::number( val );
}

/*! \internal
 
  Returns the end of the section offset \a sec.
 
*/

int QTodoDateEdit::sectionOffsetEnd( int sec ) const
{
	if ( sec == d->yearSection )
	{
		switch( d->ord )
		{
		case DMY:
		case MDY:
			return sectionOffsetEnd( sec-1) + separator().length() + sectionLength( sec );
		case YMD:
		case YDM:
			return sectionLength( sec );
		}
	}
	else if ( sec == d->monthSection )
	{
		switch( d->ord )
		{
		case DMY:
		case YDM:
		case YMD:
			return sectionOffsetEnd( sec-1) + separator().length() + sectionLength( sec );
		case MDY:
			return sectionLength( sec );
		}
	}
	else if ( sec == d->daySection )
	{
		switch( d->ord )
		{
		case DMY:
			return sectionLength( sec );
		case YMD:
		case MDY:
		case YDM:
			return sectionOffsetEnd( sec-1 ) + separator().length() + sectionLength( sec );
		}
	}
	return 0;
}


/*!
    \property QTodoDateEdit::order
    \brief the order in which the year, month and day appear
 
    The default order is locale dependent.
 
    \sa Order
*/

void QTodoDateEdit::setOrder( QTodoDateEdit::Order order )
{
	d->ord = order;
	switch( d->ord )
	{
	case DMY:
		d->yearSection = 2;
		d->monthSection = 1;
		d->daySection = 0;
		break;
	case MDY:
		d->yearSection = 2;
		d->monthSection = 0;
		d->daySection = 1;
		break;
	case YMD:
		d->yearSection = 0;
		d->monthSection = 1;
		d->daySection = 2;
		break;
	case YDM:
		d->yearSection = 0;
		d->monthSection = 2;
		d->daySection = 1;
		break;
	}
	if ( isVisible() )
		d->ed->repaint( d->ed->rect(), FALSE );
}


QTodoDateEdit::Order QTodoDateEdit::order() const
{
	return d->ord;
}


/*! \reimp
 
*/
void QTodoDateEdit::stepUp()
{
	int sec = d->ed->focusSection();
	bool accepted = FALSE;
	if ( sec == d->yearSection )
	{
		if ( !outOfRange( d->y+1, d->m, d->d ) )
		{
			accepted = TRUE;
			setYear( d->y+1 );
		}
	}
	else if ( sec == d->monthSection )
	{
		if ( !outOfRange( d->y, d->m+1, d->d ) )
		{
			accepted = TRUE;
			setMonth( d->m+1 );
		}
	}
	else if ( sec == d->daySection )
	{
		if ( !outOfRange( d->y, d->m, d->d+1 ) )
		{
			accepted = TRUE;
			setDay( d->d+1 );
		}
	}
	if ( accepted )
	{
		d->changed = TRUE;
		emit valueChanged( date() );
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}



/*! \reimp
 
*/

void QTodoDateEdit::stepDown()
{
	int sec = d->ed->focusSection();
	bool accepted = FALSE;
	if ( sec == d->yearSection )
	{
		if ( !outOfRange( d->y-1, d->m, d->d ) )
		{
			accepted = TRUE;
			setYear( d->y-1 );
		}
	}
	else if ( sec == d->monthSection )
	{
		if ( !outOfRange( d->y, d->m-1, d->d ) )
		{
			accepted = TRUE;
			setMonth( d->m-1 );
		}
	}
	else if ( sec == d->daySection )
	{
		if ( !outOfRange( d->y, d->m, d->d-1 ) )
		{
			accepted = TRUE;
			setDay( d->d-1 );
		}
	}
	if ( accepted )
	{
		d->changed = TRUE;
		emit valueChanged( date() );
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}

/*!
    Sets the year to \a year, which must be a valid year. The range
    currently supported is from 1752 to 8000.
 
    \sa QDate
*/

void QTodoDateEdit::setYear( int year )
{
	if ( year < 1752 )
		year = 1752;
	if ( year > 8000 )
		year = 8000;
	if ( !outOfRange( year, d->m, d->d ) )
	{
		d->y = year;
		setMonth( d->m );
		int tmp = d->dayCache;
		setDay( d->dayCache );
		d->dayCache = tmp;
	}
}


/*!
    Sets the month to \a month, which must be a valid month, i.e.
    between 1 and 12.
*/

void QTodoDateEdit::setMonth( int month )
{
	if ( month < 1 )
		month = 1;
	if ( month > 12 )
		month = 12;
	if ( !outOfRange( d->y, month, d->d ) )
	{
		d->m = month;
		int tmp = d->dayCache;
		setDay( d->dayCache );
		d->dayCache = tmp;
	}
}


/*!
    Sets the day to \a day, which must be a valid day. The function
    will ensure that the \a day set is valid for the month and year.
*/

void QTodoDateEdit::setDay( int day )
{
	if ( day < 1 )
		day = 1;
	if ( day > 31 )
		day = 31;
	if ( d->m > 0 && d->y > 1752 )
	{
		while ( !QDate::isValid( d->y, d->m, day ) )
			--day;
		if ( !outOfRange( d->y, d->m, day ) )
			d->d = day;
	}
	else if ( d->m > 0 )
	{
		if ( day > 0 && day < 32 )
		{
			if ( !outOfRange( d->y, d->m, day ) )
				d->d = day;
		}
	}
	d->dayCache = d->d;
}


/*!
    \property QTodoDateEdit::date
    \brief the editor's date value.
 
    If the date property is not valid, the editor displays all zeroes
    and QTodoDateEdit::date() will return an invalid date. It is strongly
    recommended that the editor is given a default date value (e.g.
    currentDate()). That way, attempts to set the date property to an
    invalid date will fail.
 
    When changing the date property, if the date is less than
    minValue(), or is greater than maxValue(), nothing happens.
*/

void QTodoDateEdit::setDate( const QDate& date )
{
	if ( !date.isValid() )
	{
		d->y = 0;
		d->m = 0;
		d->d = 0;
		d->dayCache = 0;
	}
	else
	{
		if ( date > maxValue() || date < minValue() )
			return;
		d->y = date.year();
		d->m = date.month();
		d->d = date.day();
		d->dayCache = d->d;
		emit valueChanged( date );
	}
	d->changed = FALSE;
	d->ed->repaint( d->ed->rect(), FALSE );
}

QDate QTodoDateEdit::date() const
{
	if ( QDate::isValid( d->y, d->m, d->d ) )
		return QDate( d->y, d->m, d->d );
	return QDate();
}

/*!  \internal
 
  Returns TRUE if \a y, \a m, \a d is out of range, otherwise returns
  FALSE.
 
  \sa setRange()
 
*/

bool QTodoDateEdit::outOfRange( int y, int m, int d ) const
{
	if ( QDate::isValid( y, m, d ) )
	{
		QDate currentDate( y, m, d );
		if ( currentDate > maxValue() ||
		        currentDate < minValue() )
		{
			//## outOfRange should set overwrite?
			return TRUE;
		}
		return FALSE;
	}
	return FALSE; /* assume ok */
}

/*!  \reimp
 
*/

void QTodoDateEdit::addNumber( int sec, int num )
{
	if ( sec == -1 )
		return;
	killTimer( d->timerId );
	bool overwrite = FALSE;
	bool accepted = FALSE;
	d->typing = TRUE;
	QString txt;
	if ( sec == d->yearSection )
	{
		txt = QString::number( d->y );
		if ( d->overwrite || txt.length() == 4 )
		{
			accepted = TRUE;
			d->y = num;
		}
		else
		{
			txt += QString::number( num );
			if ( txt.length() == 4  )
			{
				int val = txt.toInt();
				if ( val < 1792 )
					d->y = 1792;
				else if ( val > 8000 )
					d->y = 8000;
				else if ( outOfRange( val, d->m, d->d ) )
					txt = QString::number( d->y );
				else
				{
					accepted = TRUE;
					d->y = val;
				}
			}
			else
			{
				accepted = TRUE;
				d->y = txt.toInt();
			}
			if ( d->adv && txt.length() == 4 )
			{
				d->ed->setFocusSection( d->ed->focusSection()+1 );
				overwrite = TRUE;
			}
		}
	}
	else if ( sec == d->monthSection )
	{
		txt = QString::number( d->m );
		if ( d->overwrite || txt.length() == 2 )
		{
			accepted = TRUE;
			d->m = num;
		}
		else
		{
			txt += QString::number( num );
			int temp = txt.toInt();
			if ( temp > 12 )
				temp = num;
			if ( outOfRange( d->y, temp, d->d ) )
				txt = QString::number( d->m );
			else
			{
				accepted = TRUE;
				d->m = temp;
			}
			if ( d->adv && txt.length() == 2 )
			{
				d->ed->setFocusSection( d->ed->focusSection()+1 );
				overwrite = TRUE;
			}
		}
	}
	else if ( sec == d->daySection )
	{
		txt = QString::number( d->d );
		if ( d->overwrite || txt.length() == 2 )
		{
			accepted = TRUE;
			d->d = num;
			d->dayCache = d->d;
		}
		else
		{
			txt += QString::number( num );
			int temp = txt.toInt();
			if ( temp > 31 )
				temp = num;
			if ( outOfRange( d->y, d->m, temp ) )
				txt = QString::number( d->d );
			else
			{
				accepted = TRUE;
				d->d = temp;
				d->dayCache = d->d;
			}
			if ( d->adv && txt.length() == 2 )
			{
				d->ed->setFocusSection( d->ed->focusSection()+1 );
				overwrite = TRUE;
			}
		}
	}
	if ( accepted )
	{
		d->changed = TRUE;
		emit valueChanged( date() );
	}
	d->overwrite = overwrite;
	d->timerId = startTimer( qApp->doubleClickInterval()*4 );
	d->ed->repaint( d->ed->rect(), FALSE );
}


/*! \reimp
 
*/

bool QTodoDateEdit::setFocusSection( int s )
{
	if ( s != d->ed->focusSection() )
	{
		killTimer( d->timerId );
		d->overwrite = TRUE;
		d->typing = FALSE;
		fix(); // will emit valueChanged if necessary
	}
	return d->ed->setFocusSection( s );
}


/*!
    Attempts to fix any invalid date entries.
 
    The rules applied are as follows:
 
    \list
    \i If the year has four digits it is left unchanged.
    \i If the year has two digits, the year will be changed to four
    digits in the range current year - 70 to current year + 29.
    \i If the year has three digits in the range 100..999, the
    current millennium, i.e. 2000, will be added giving a year
    in the range 2100..2999.
    \i If the day or month is 0 then it will be set to 1 or the
    minimum valid day\month in the range.
    \endlist
 
*/

void QTodoDateEdit::fix()
{
	bool changed = FALSE;
	int currentYear = QDate::currentDate().year();
	int year = d->y;
	if ( year < 100 )
	{
		int currentCentury = currentYear / 100;
		year += currentCentury * 100;
		if ( currentYear > year )
		{
			if ( currentYear > year + 70 )
				year += 100;
		}
		else
		{
			if ( year >= currentYear + 30 )
				year -= 100;
		}
		changed = TRUE;
	}
	else if ( year < 1000 )
	{
		int currentMillennium = currentYear / 10;
		year += currentMillennium * 10;
		changed = TRUE;
	}
	else if (d->d == 0)
	{
		d->d = 1;
		changed = TRUE;
	}
	else if (d->m == 0)
	{
		d->m = 1;
		changed = TRUE;
	}
	if ( outOfRange( year, d->m, d->d ) )
	{
		if ( minValue().isValid() && date() < minValue() )
		{
			d->d =  minValue().day();
			d->dayCache = d->d;
			d->m = minValue().month();
			d->y = minValue().year();
		}
		if ( date() > maxValue() )
		{
			d->d =  maxValue().day();
			d->dayCache = d->d;
			d->m = maxValue().month();
			d->y = maxValue().year();
		}
		changed = TRUE;
	}
	else if ( changed )
		setYear( year );
	if ( changed )
	{
		emit valueChanged( date() );
		d->changed = FALSE;
	}
}


/*! \reimp
 
*/

bool QTodoDateEdit::event( QEvent *e )
{
	if( e->type() == QEvent::FocusOut )
	{
		d->typing = FALSE;
		fix();
		// the following can't be done in fix() because fix() called
		// from all over the place and it will break the old behaviour
		if ( !QDate::isValid( d->y, d->m, d->d ) )
		{
			d->dayCache = d->d;
			int i = d->d;
			for ( ; i > 0; i-- )
			{
				d->d = i;
				if ( QDate::isValid( d->y, d->m, d->d ) )
					break;
			}
			d->changed = TRUE;
		}
		if ( d->changed )
		{
			emit valueChanged( date() );
			d->changed = FALSE;
		}
	}
	else if ( e->type() == QEvent::LocaleChange )
	{
		readLocaleSettings();
		d->ed->setSeparator( localDateSep() );
		setOrder( localOrder() );
	}
	return QTodoDateTimeEditBase::event( e );
}

/*!
  \internal
 
  Function which is called whenever the user tries to
  remove the first number from \a sec by pressing the backspace key.
*/

void QTodoDateEdit::removeFirstNumber( int sec )
{
	if ( sec == -1 )
		return;
	QString txt;
	if ( sec == d->yearSection )
	{
		txt = QString::number( d->y );
		txt = txt.mid( 1, txt.length() ) + "0";
		d->y = txt.toInt();
	}
	else if ( sec == d->monthSection )
	{
		txt = QString::number( d->m );
		txt = txt.mid( 1, txt.length() ) + "0";
		d->m = txt.toInt();
	}
	else if ( sec == d->daySection )
	{
		txt = QString::number( d->d );
		txt = txt.mid( 1, txt.length() ) + "0";
		d->d = txt.toInt();
		d->dayCache = d->d;
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}

/*! \reimp
 
*/

void QTodoDateEdit::removeLastNumber( int sec )
{
	if ( sec == -1 )
		return;
	QString txt;
	if ( sec == d->yearSection )
	{
		txt = QString::number( d->y );
		txt = txt.mid( 0, txt.length()-1 );
		d->y = txt.toInt();
	}
	else if ( sec == d->monthSection )
	{
		txt = QString::number( d->m );
		txt = txt.mid( 0, txt.length()-1 );
		d->m = txt.toInt();
	}
	else if ( sec == d->daySection )
	{
		txt = QString::number( d->d );
		txt = txt.mid( 0, txt.length()-1 );
		d->d = txt.toInt();
		d->dayCache = d->d;
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}

/*!
    \property QTodoDateEdit::autoAdvance
    \brief whether the editor automatically advances to the next
    section
 
    If autoAdvance is TRUE, the editor will automatically advance
    focus to the next date section if a user has completed a section.
    The default is FALSE.
*/

void QTodoDateEdit::setAutoAdvance( bool advance )
{
	d->adv = advance;
}


bool QTodoDateEdit::autoAdvance() const
{
	return d->adv;
}

/*! \reimp
*/

void QTodoDateEdit::timerEvent( QTimerEvent * )
{
	d->overwrite = TRUE;
}

/*!
    \fn void QTodoDateEdit::valueChanged( const QDate& date )
 
    This signal is emitted whenever the editor's value changes. The \a
    date parameter is the new value.
*/

///////////

class QTodoTimeEditPrivate
{
public:
	int h;
	int m;
	int s;
	uint display;
	bool adv;
	bool overwrite;
	int timerId;
	bool typing;
	QTime min;
	QTime max;
	bool changed;
	QTodoDateTimeEditor *ed;
	QSpinWidget *controls;
};

/*!
    \class QTodoTimeEdit qdatetimeedit.h
    \brief The QTodoTimeEdit class provides a time editor.
 
    \ingroup advanced
    \ingroup time
    \mainclass
 
    QTodoTimeEdit allows the user to edit times by using the keyboard or
    the arrow keys to increase/decrease time values. The arrow keys
    can be used to move from section to section within the QTodoTimeEdit
    box. The user can automatically be moved to the next section once
    they complete a section using setAutoAdvance(). Times appear in
    hour, minute, second order. It is recommended that the QTodoTimeEdit
    is initialised with a time, e.g.
    \code
    QTime timeNow = QTime::currentTime();
    QTodoTimeEdit *timeEdit = new QTodoTimeEdit( timeNow, this );
    timeEdit->setRange( timeNow, timeNow.addSecs( 60 * 60 ) );
    \endcode
    Here we've created a QTodoTimeEdit widget set to the current time.
    We've also set the minimum value to the current time and the
    maximum time to one hour from now.
 
    The maximum and minimum values for a time value in the time editor
    default to the maximum and minimum values for a QTime. You can
    change this by calling setMinValue(), setMaxValue() or setRange().
 
    Terminology: A QTimeWidget consists of three sections, one each
    for the hour, minute and second. You can change the separator
    character using setSeparator(), by default the separator is read
    from the system's settings.
 
    \img datetimewidgets.png Date Time Widgets
 
    \sa QTime QTodoDateEdit QTodoDateTimeEdit
*/


/*!
    Constructs an empty time edit with parent \a parent and called \a
    name.
*/

QTodoTimeEdit::QTodoTimeEdit( QWidget * parent, const char * name )
		: QTodoDateTimeEditBase( parent, name )
{
	init();
}

/*!
    \overload
 
    Constructs a time edit with the initial time value, \a time,
    parent \a parent and called \a name.
*/

QTodoTimeEdit::QTodoTimeEdit( const QTime& time, QWidget * parent, const char * name )
		: QTodoDateTimeEditBase( parent, name )
{
	init();
	setTime( time );
}

/*! \internal
 */

void QTodoTimeEdit::init()
{
	d = new QTodoTimeEditPrivate();
	d->controls = new QDateTimeSpinWidget( this, qstrcmp( name(), "qt_datetime_timeedit" ) == 0 ? "qt_spin_widget" : "time edit controls" );
	d->controls->move(d->controls->x()+1,d->controls->y()-1);
	d->ed = new QTodoDateTimeEditor( this, d->controls, "time edit base" );
	d->controls->setEditWidget( d->ed );
	setFocusProxy( d->ed );
	connect( d->controls, SIGNAL( stepUpPressed() ), SLOT( stepUp() ) );
	connect( d->controls, SIGNAL( stepDownPressed() ), SLOT( stepDown() ) );
	connect( d->ed, SIGNAL(focusInSignal()),this,SLOT(slotFocusInSignal()));
	connect( d->ed, SIGNAL(focusOutSignal()),this,SLOT(slotFocusOutSignal()));

	d->ed->appendSection( QTodoNumberSection( 0,0, TRUE, 0 ) );
	d->ed->appendSection( QTodoNumberSection( 0,0, TRUE, 1 ) );
	d->ed->appendSection( QTodoNumberSection( 0,0, TRUE, 2 ) );
	d->ed->setSeparator( localTimeSep() );

	d->h = 0;
	d->m = 0;
	d->s = 0;
	d->display = Hours | Minutes | Seconds;
	if ( lAMPM )
	{
		d->display |= AMPM;
		d->ed->appendSection( QTodoNumberSection( 0,0, FALSE, 3 ) );
	}
	d->adv = FALSE;
	d->overwrite = TRUE;
	d->timerId = 0;
	d->typing = FALSE;
	d->min = QTime( 0, 0, 0 );
	d->max = QTime( 23, 59, 59 );
	d->changed = FALSE;

	setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );

	m_spin_width = d->controls->upRect().width();

	refcount++;
}

/*!
    Destroys the object and frees any allocated resources.
*/

QTodoTimeEdit::~QTodoTimeEdit()
{
	delete d;
	if ( !--refcount )
		cleanup();
}

/*!
    \property QTodoTimeEdit::minValue
    \brief the minimum time value
 
    Setting the minimum time value is equivalent to calling
    QTodoTimeEdit::setRange( \e t, maxValue() ), where \e t is the minimum
    time. The default minimum time is 00:00:00.
 
    \sa maxValue setRange()
*/

QTime QTodoTimeEdit::minValue() const
{
	return d->min;
}

/*!
    \property QTodoTimeEdit::maxValue
    \brief the maximum time value
 
    Setting the maximum time value is equivalent to calling
    QTodoTimeEdit::setRange( minValue(), \e t ), where \e t is the maximum
    time. The default maximum time is 23:59:59.
 
    \sa minValue setRange()
*/

QTime QTodoTimeEdit::maxValue() const
{
	return d->max;
}


/*!
    Sets the valid input range for the editor to be from \a min to \a
    max inclusive. If \a min is invalid no minimum time is set.
    Similarly, if \a max is invalid no maximum time is set.
*/

void QTodoTimeEdit::setRange( const QTime& min, const QTime& max )
{
	if ( min.isValid() )
		d->min = min;
	if ( max.isValid() )
		d->max = max;
}

/*!
  \property QTodoTimeEdit::display
  \brief the sections that are displayed in the time edit
 
  The value can be any combination of the values in the Display enum.
  By default, the widget displays hours, minutes and seconds.
*/
void QTodoTimeEdit::setDisplay( uint display )
{
	if ( d->display == display )
		return;

	d->ed->clearSections();
	d->display = display;
	if ( d->display & Hours )
		d->ed->appendSection( QTodoNumberSection( 0,0, TRUE, 0 ) );
	if ( d->display & Minutes )
		d->ed->appendSection( QTodoNumberSection( 0,0, TRUE, 1 ) );
	if ( d->display & Seconds )
		d->ed->appendSection( QTodoNumberSection( 0,0, TRUE, 2 ) );
	if ( d->display & AMPM )
		d->ed->appendSection( QTodoNumberSection( 0,0, FALSE, 3 ) );

	d->ed->setFocusSection( 0 );
	d->ed->update();
}

uint QTodoTimeEdit::display() const
{
	return d->display;
}

/*!
    \property QTodoTimeEdit::time
    \brief the editor's time value.
 
    When changing the time property, if the time is less than
    minValue(), or is greater than maxValue(), nothing happens.
*/

void QTodoTimeEdit::setTime( const QTime& time )
{
	if ( !time.isValid() )
	{
		d->h = 0;
		d->m = 0;
		d->s = 0;
	}
	else
	{
		if ( time > maxValue() || time < minValue() )
			return;
		d->h = time.hour();
		d->m = time.minute();
		d->s = time.second();
		emit valueChanged( time );
	}
	d->changed = FALSE;
	d->ed->repaint( d->ed->rect(), FALSE );
}

QTime QTodoTimeEdit::time() const
{
	if ( QTime::isValid( d->h, d->m, d->s ) )
		return QTime( d->h, d->m, d->s );
	return QTime();
}

/*!
    \property QTodoTimeEdit::autoAdvance
    \brief whether the editor automatically advances to the next
    section
 
    If autoAdvance is TRUE, the editor will automatically advance
    focus to the next time section if a user has completed a section.
    The default is FALSE.
*/

void QTodoTimeEdit::setAutoAdvance( bool advance )
{
	d->adv = advance;
}

bool QTodoTimeEdit::autoAdvance() const
{
	return d->adv;
}

/*!
    Sets the separator to \a s. Note that currently only the first
    character of \a s is used.
*/

void QTodoTimeEdit::setSeparator( const QString& s )
{
	d->ed->setSeparator( s );
}

/*!
    Returns the editor's separator.
*/

QString QTodoTimeEdit::separator() const
{
	return d->ed->separator();
}


/*!
    \fn void QTodoTimeEdit::valueChanged( const QTime& time )
 
    This signal is emitted whenever the editor's value changes. The \a
    time parameter is the new value.
*/

/*! \reimp
 
*/

bool QTodoTimeEdit::event( QEvent *e )
{
	if ( e->type() == QEvent::FocusOut )
	{
		d->typing = FALSE;
		if ( d->changed )
		{
			emit valueChanged( time() );
			d->changed = FALSE;
		}
	}
	else if ( e->type() == QEvent::LocaleChange )
	{
		readLocaleSettings();
		d->ed->setSeparator( localTimeSep() );
	}
	return QTodoDateTimeEditBase::event( e );
}

/*! \reimp
 
*/

void QTodoTimeEdit::timerEvent( QTimerEvent * )
{
	d->overwrite = TRUE;
}


/*! \reimp
 
*/

void QTodoTimeEdit::stepUp()
{
	if (minValue() > maxValue())
	{
		return;
	}
	int sec = d->ed->mapSection( d->ed->focusSection() );
	bool accepted = TRUE;
	switch( sec )
	{
	case 0:
		do
		{
			d->h = (d->h + 1) % 24;
		}
		while (outOfRange(d->h, d->m, d->s));
		break;
	case 1:
		do
		{
			d->m = (d->m + 1) % 60;
		}
		while (outOfRange(d->h, d->m, d->s));
		break;
	case 2:
		do
		{
			d->s = (d->s + 1) % 60;
		}
		while (outOfRange(d->h, d->m, d->s));
		break;
	case 3:
		if ( d->h < 12 )
			setHour( d->h+12 );
		else
			setHour( d->h-12 );
		break;
	default:
		accepted = FALSE;
#ifdef QT_CHECK_RANGE
		qWarning( "QTodoTimeEdit::stepUp: Focus section out of range!" );
#endif
		break;
	}
	if ( accepted )
	{
		d->changed = TRUE;
		emit valueChanged( time() );
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}


/*! \reimp
 
*/

void QTodoTimeEdit::stepDown()
{
	if (minValue() > maxValue())
	{
		return;
	}

	int sec = d->ed->mapSection( d->ed->focusSection() );
	bool accepted = TRUE;
	switch( sec )
	{
	case 0:
		do
		{
			if (--d->h < 0)
				d->h = 23;
		}
		while (outOfRange(d->h, d->m, d->s));
		break;
	case 1:
		do
		{
			if (--d->m < 0)
				d->m = 59;
		}
		while (outOfRange(d->h, d->m, d->s));
		break;
	case 2:
		do
		{
			if (--d->s < 0)
				d->s = 59;
		}
		while (outOfRange(d->h, d->m, d->s));
		break;
	case 3:
		if ( d->h > 11 )
			setHour( d->h-12 );
		else
			setHour( d->h+12 );
		break;
	default:
		accepted = FALSE;
#ifdef QT_CHECK_RANGE
		qWarning( "QTodoTimeEdit::stepDown: Focus section out of range!" );
#endif
		break;
	}
	if ( accepted )
	{
		d->changed = TRUE;
		emit valueChanged( time() );
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}


/*!
    Returns the formatted number for section \a sec. This will
    correspond to either the hour, minute or second section, depending
    on \a sec.
*/

QString QTodoTimeEdit::sectionFormattedText( int sec )
{
	QString txt;
	txt = sectionText( sec );
	txt = txt.rightJustify( 2, QDATETIMEEDIT_HIDDEN_CHAR );
	int offset = sec*2+sec*separator().length() + txt.length();
	if ( d->typing && sec == d->ed->focusSection() )
		d->ed->setSectionSelection( sec, offset - txt.length(), offset );
	else
		d->ed->setSectionSelection( sec, offset - txt.length(), offset );

	return txt;
}


/*! \reimp
 
*/

bool QTodoTimeEdit::setFocusSection( int sec )
{
	if ( sec != d->ed->focusSection() )
	{
		killTimer( d->timerId );
		d->overwrite = TRUE;
		d->typing = FALSE;
		QString txt = sectionText( sec );
		txt = txt.rightJustify( 2, QDATETIMEEDIT_HIDDEN_CHAR );
		int offset = sec*2+sec*separator().length() + txt.length();
		d->ed->setSectionSelection( sec, /*offset -*/ txt.length(), offset );
		if ( d->changed )
		{
			emit valueChanged( time() );
			d->changed = FALSE;
		}
	}
	return d->ed->setFocusSection( sec );
}


/*!
    Sets the hour to \a h, which must be a valid hour, i.e. in the
    range 0..24.
*/

void QTodoTimeEdit::setHour( int h )
{
	if ( h < 0 )
		h = 0;
	if ( h > 23 )
		h = 23;
	d->h = h;
}


/*!
    Sets the minute to \a m, which must be a valid minute, i.e. in the
    range 0..59.
*/

void QTodoTimeEdit::setMinute( int m )
{
	if ( m < 0 )
		m = 0;
	if ( m > 59 )
		m = 59;
	d->m = m;
}


/*!
    Sets the second to \a s, which must be a valid second, i.e. in the
    range 0..59.
*/

void QTodoTimeEdit::setSecond( int s )
{
	if ( s < 0 )
		s = 0;
	if ( s > 59 )
		s = 59;
	d->s = s;
}


/*! \internal
 
  Returns the text of section \a sec.
 
*/

QString QTodoTimeEdit::sectionText( int sec )
{
	sec = d->ed->mapSection( sec );

	QString txt;
	switch( sec )
	{
	case 0:
		if ( !(d->display & AMPM) || ( d->h < 13 && d->h ) )
		{    // I wished the day stared at 0:00 for everybody
			txt = QString::number( d->h );
		}
		else
		{
			if ( d->h )
				txt = QString::number( d->h - 12 );
			else
				txt = "12";
		}
		break;
	case 1:
		txt = QString::number( d->m );
		break;
	case 2:
		txt = QString::number( d->s );
		break;
	case 3:
		if ( d->h < 12 )
		{
			if ( lAM )
				txt = *lAM;
			else
				txt = QString::fromLatin1( "AM" );
		}
		else
		{
			if ( lPM )
				txt = *lPM;
			else
				txt = QString::fromLatin1( "PM" );
		}
		break;
	default:
		break;
	}
	return txt;
}


/*! \internal
 Returns TRUE if \a h, \a m, and \a s are out of range.
 */

bool QTodoTimeEdit::outOfRange( int h, int m, int s ) const
{
	if ( QTime::isValid( h, m, s ) )
	{
		QTime currentTime( h, m, s );
		if ( currentTime > maxValue() ||
		        currentTime < minValue() )
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

/*! \reimp
 
*/

void QTodoTimeEdit::addNumber( int sec, int num )
{
	if ( sec == -1 )
		return;
	sec = d->ed->mapSection( sec );
	killTimer( d->timerId );
	bool overwrite = FALSE;
	bool accepted = FALSE;
	d->typing = TRUE;
	QString txt;

	switch( sec )
	{
	case 0:
		txt = ( d->display & AMPM && d->h > 12 ) ?
		      QString::number( d->h - 12 ) : QString::number( d->h );

		if ( d->overwrite || txt.length() == 2 )
		{
			if ( d->display & AMPM && num == 0 )
				break; // Don't process 0 in 12 hour clock mode
			if ( d->display & AMPM && d->h > 11 )
				num += 12;
			if ( !outOfRange( num, d->m, d->s ) )
			{
				accepted = TRUE;
				d->h = num;
			}
		}
		else
		{
			txt += QString::number( num );
			int temp = txt.toInt();

			if ( d->display & AMPM )
			{
				if ( temp == 12 )
				{
					if ( d->h < 12 )
					{
						temp = 0;
					}
					accepted = TRUE;
				}
				else if ( outOfRange( temp + 12, d->m, d->s ) )
				{
					txt = QString::number( d->h );
				}
				else
				{
					if ( d->h > 11 )
					{
						temp += 12;
					}
					accepted = TRUE;
				}
			}
			else if ( !(d->display & AMPM) && outOfRange( temp, d->m, d->s ) )
			{
				txt = QString::number( d->h );
			}
			else
			{
				accepted = TRUE;
			}

			if ( accepted )
				d->h = temp;

			if ( d->adv && txt.length() == 2 )
			{
				setFocusSection( d->ed->focusSection()+1 );
				overwrite = TRUE;
			}
		}
		break;

	case 1:
		txt = QString::number( d->m );
		if ( d->overwrite || txt.length() == 2 )
		{
			if ( !outOfRange( d->h, num, d->s ) )
			{
				accepted = TRUE;
				d->m = num;
			}
		}
		else
		{
			txt += QString::number( num );
			int temp = txt.toInt();
			if ( temp > 59 )
				temp = num;
			if ( outOfRange( d->h, temp, d->s ) )
				txt = QString::number( d->m );
			else
			{
				accepted = TRUE;
				d->m = temp;
			}
			if ( d->adv && txt.length() == 2 )
			{
				setFocusSection( d->ed->focusSection()+1 );
				overwrite = TRUE;
			}
		}
		break;

	case 2:
		txt = QString::number( d->s );
		if ( d->overwrite || txt.length() == 2 )
		{
			if ( !outOfRange( d->h, d->m, num ) )
			{
				accepted = TRUE;
				d->s = num;
			}
		}
		else
		{
			txt += QString::number( num );
			int temp = txt.toInt();
			if ( temp > 59 )
				temp = num;
			if ( outOfRange( d->h, d->m, temp ) )
				txt = QString::number( d->s );
			else
			{
				accepted = TRUE;
				d->s = temp;
			}
			if ( d->adv && txt.length() == 2 )
			{
				setFocusSection( d->ed->focusSection()+1 );
				overwrite = TRUE;
			}
		}
		break;

	case 3:
		break;

	default:
		break;
	}
	d->changed = accepted;
	if ( accepted )
		emit valueChanged( time() );
	d->overwrite = overwrite;
	d->timerId = startTimer( qApp->doubleClickInterval()*4 );
	d->ed->repaint( d->ed->rect(), FALSE );
}


/*!
  \internal
 
  Function which is called whenever the user tries to
  remove the first number from \a sec by pressing the backspace key.
*/

void QTodoTimeEdit::removeFirstNumber( int sec )
{
	if ( sec == -1 )
		return;
	sec = d->ed->mapSection( sec );
	QString txt;
	switch( sec )
	{
	case 0:
		txt = QString::number( d->h );
		break;
	case 1:
		txt = QString::number( d->m );
		break;
	case 2:
		txt = QString::number( d->s );
		break;
	}
	txt = txt.mid( 1, txt.length() ) + "0";
	switch( sec )
	{
	case 0:
		d->h = txt.toInt();
		break;
	case 1:
		d->m = txt.toInt();
		break;
	case 2:
		d->s = txt.toInt();
		break;
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}

/*! \reimp
 
*/
void QTodoTimeEdit::removeLastNumber( int sec )
{
	if ( sec == -1 )
		return;
	sec = d->ed->mapSection( sec );
	QString txt;
	switch( sec )
	{
	case 0:
		txt = QString::number( d->h );
		break;
	case 1:
		txt = QString::number( d->m );
		break;
	case 2:
		txt = QString::number( d->s );
		break;
	}
	txt = txt.mid( 0, txt.length()-1 );
	switch( sec )
	{
	case 0:
		d->h = txt.toInt();
		break;
	case 1:
		d->m = txt.toInt();
		break;
	case 2:
		d->s = txt.toInt();
		break;
	}
	d->ed->repaint( d->ed->rect(), FALSE );
}

/*! \reimp
 */
void QTodoTimeEdit::resizeEvent( QResizeEvent * )
{
	d->controls->resize( width(), height() );
}

/*! \reimp
*/
QSize QTodoTimeEdit::sizeHint() const
{
	constPolish();
	QFontMetrics fm( font() );
//	int fw = style().pixelMetric( QStyle::PM_DefaultFrameWidth, this );
	int h = fm.lineSpacing() - 2 ; //+ 2;
	int w =/* 2 +*/ fm.width( '9' ) * 4 + fm.width( d->ed->separator() ) /* *2 */ + m_spin_width /*+ fw * 4*/;
	if ( d->display & AMPM )
	{
		if ( lAM )
			w += fm.width( *lAM ) + 4;
		else
			w += fm.width( QString::fromLatin1( "AM" ) ) + 4;
	}
	return QSize( w, QMAX(h + 0 /*fw * 2*/,16)).expandedTo( QApplication::globalStrut() );
}

/*! \reimp
*/
QSize QTodoTimeEdit::minimumSizeHint() const
{
	return sizeHint();
}

/*!
    \internal
    Enables/disables the push buttons according to the min/max time
    for this widget.
*/

// ### Remove in 4.0?

void QTodoTimeEdit::updateButtons()
{
	if ( !isEnabled() )
		return;

	bool upEnabled = time() < maxValue();
	bool downEnabled = time() > minValue();

	d->controls->setUpEnabled( upEnabled );
	d->controls->setDownEnabled( downEnabled );
}


class QTodoDateTimeEditPrivate
{
public:
	bool adv;
};

/*!
    \class QTodoDateTimeEdit qdatetimeedit.h
    \brief The QTodoDateTimeEdit class combines a QTodoDateEdit and QTodoTimeEdit
    widget into a single widget for editing datetimes.
 
    \ingroup advanced
    \ingroup time
    \mainclass
 
    QTodoDateTimeEdit consists of a QTodoDateEdit and QTodoTimeEdit widget placed
    side by side and offers the functionality of both. The user can
    edit the date and time by using the keyboard or the arrow keys to
    increase/decrease date or time values. The Tab key can be used to
    move from section to section within the QTodoDateTimeEdit widget, and
    the user can be moved automatically when they complete a section
    using setAutoAdvance(). The datetime can be set with
    setDateTime().
 
    The date format is read from the system's locale settings. It is
    set to year, month, day order if that is not possible. See
    QTodoDateEdit::setOrder() to change this. Times appear in the order
    hours, minutes, seconds using the 24 hour clock.
 
    It is recommended that the QTodoDateTimeEdit is initialised with a
    datetime, e.g.
    \code
    QTodoDateTimeEdit *dateTimeEdit = new QTodoDateTimeEdit( QDateTime::currentDateTime(), this );
    dateTimeEdit->dateEdit()->setRange( QDateTime::currentDate(),
					QDateTime::currentDate().addDays( 7 ) );
    \endcode
    Here we've created a new QTodoDateTimeEdit set to the current date and
    time, and set the date to have a minimum date of now and a maximum
    date of a week from now.
 
    Terminology: A QTodoDateEdit widget consists of three 'sections', one
    each for the year, month and day. Similarly a QTodoTimeEdit consists
    of three sections, one each for the hour, minute and second. The
    character that separates each date section is specified with
    setDateSeparator(); similarly setTimeSeparator() is used for the
    time sections.
 
    \img datetimewidgets.png Date Time Widgets
 
    \sa QTodoDateEdit QTodoTimeEdit
*/

/*!
    Constructs an empty datetime edit with parent \a parent and called
    \a name.
*/
QTodoDateTimeEdit::QTodoDateTimeEdit( QWidget * parent, const char * name )
		: QWidget( parent, name )
{
	init();
}


/*!
    \overload
 
    Constructs a datetime edit with the initial value \a datetime,
    parent \a parent and called \a name.
*/
QTodoDateTimeEdit::QTodoDateTimeEdit( const QDateTime& datetime,
                              QWidget * parent, const char * name )
		: QWidget( parent, name )
{
	init();
	setDateTime( datetime );
}



/*!
    Destroys the object and frees any allocated resources.
*/

QTodoDateTimeEdit::~QTodoDateTimeEdit()
{
	delete d;
}


/*!
    \reimp
 
    Intercepts and handles resize events which have special meaning
    for the QTodoDateTimeEdit.
*/

void QTodoDateTimeEdit::resizeEvent( QResizeEvent * )
{
	int dw = de->sizeHint().width();
	int tw = te->sizeHint().width();
	int w = width();
	int h = height();
	int extra = w - ( dw + tw );

	if ( tw + extra < 0 )
	{
		dw = w;
	}
	else
	{
		dw += 9 * extra / 16;
	}
	tw = w - dw;

	de->setGeometry( 0, 0, dw, h );
	te->setGeometry( dw, 0, tw, h );
}

/*! \reimp
*/

QSize QTodoDateTimeEdit::minimumSizeHint() const
{
	QSize dsh = de->minimumSizeHint();
	QSize tsh = te->minimumSizeHint();
	return QSize( dsh.width() + tsh.width(),
	              QMAX( dsh.height(), tsh.height() ) );
}

/*!  \internal
 */

void QTodoDateTimeEdit::init()
{
	d = new QTodoDateTimeEditPrivate();
	de = new QTodoDateEdit( this, "qt_datetime_dateedit" );
	te = new QTodoTimeEdit( this, "qt_datetime_timeedit" );
	d->adv = FALSE;
	connect( de, SIGNAL( valueChanged(const QDate&) ),
	         this, SLOT( newValue(const QDate&) ) );
	connect( te, SIGNAL( valueChanged(const QTime&) ),
	         this, SLOT( newValue(const QTime&) ) );
	setFocusProxy( de );
	setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
}

/*! \reimp
 */

QSize QTodoDateTimeEdit::sizeHint() const
{
	constPolish();
	QSize dsh = de->sizeHint();
	QSize tsh = te->sizeHint();
	return QSize( dsh.width() + tsh.width(),
	              QMAX( dsh.height(), tsh.height() ) );
}

/*!
    \property QTodoDateTimeEdit::dateTime
    \brief the editor's datetime value
 
    The datetime edit's datetime which may be an invalid datetime.
*/

void QTodoDateTimeEdit::setDateTime( const QDateTime & dt )
{
	if ( dt.isValid() )
	{
		de->setDate( dt.date() );
		te->setTime( dt.time() );
		emit valueChanged( dt );
	}
}

QDateTime QTodoDateTimeEdit::dateTime() const
{
	return QDateTime( de->date(), te->time() );
}

/*!
    \fn void QTodoDateTimeEdit::valueChanged( const QDateTime& datetime )
 
    This signal is emitted every time the date or time changes. The \a
    datetime argument is the new datetime.
*/


/*! \internal
 
  Re-emits the value \a d.
 */

void QTodoDateTimeEdit::newValue( const QDate& )
{
	QDateTime dt = dateTime();
	emit valueChanged( dt );
}

/*! \internal
  \overload
  Re-emits the value \a t.
 */

void QTodoDateTimeEdit::newValue( const QTime& )
{
	QDateTime dt = dateTime();
	emit valueChanged( dt );
}


/*!
    Sets the auto advance property of the editor to \a advance. If set
    to TRUE, the editor will automatically advance focus to the next
    date or time section if the user has completed a section.
*/

void QTodoDateTimeEdit::setAutoAdvance( bool advance )
{
	de->setAutoAdvance( advance );
	te->setAutoAdvance( advance );
}

/*!
    Returns TRUE if auto-advance is enabled, otherwise returns FALSE.
 
    \sa setAutoAdvance()
*/

bool QTodoDateTimeEdit::autoAdvance() const
{
	return de->autoAdvance();
}

void QTodoDateEdit::slotFocusInSignal()
{
	focus_in_date = date();
}
void QTodoDateEdit::slotFocusOutSignal()
{
	if(focus_in_date != date())
		emit oldValueChanged(focus_in_date);
}

void QTodoTimeEdit::slotFocusInSignal()
{
	focus_in_time = time();
}
void QTodoTimeEdit::slotFocusOutSignal()
{
	if(focus_in_time != time())
		emit oldValueChanged(focus_in_time);
}

/*!
    \fn QTodoDateEdit* QTodoDateTimeEdit::dateEdit()
 
    Returns the internal widget used for editing the date part of the
    datetime.
*/

/*!
    \fn QTodoTimeEdit* QTodoDateTimeEdit::timeEdit()
 
    Returns the internal widget used for editing the time part of the
    datetime.
*/

