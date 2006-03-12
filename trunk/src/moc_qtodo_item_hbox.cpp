/****************************************************************************
** QTodoItem meta object code from reading C++ file 'qtodo_item.h'
**
** Created: Sun Nov 6 15:26:01 2005
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.4   edited Jan 21 18:14 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "qtodo_item.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QTodoItem::className() const
{
    return "QTodoItem";
}

QMetaObject *QTodoItem::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QTodoItem( "QTodoItem", &QTodoItem::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QTodoItem::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QTodoItem", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QTodoItem::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QTodoItem", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QTodoItem::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QHBox::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"actionsMenuActivated", 1, param_slot_0 };
    static const QUMethod slot_1 = {"actionsMenu", 0, 0 };
    static const QUMethod slot_2 = {"addSubTodo", 0, 0 };
    static const QUMethod slot_3 = {"addTodoBelow", 0, 0 };
    static const QUMethod slot_4 = {"addTodoAbove", 0, 0 };
    static const QUParameter param_slot_5[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_5 = {"hasStart", 1, param_slot_5 };
    static const QUParameter param_slot_6[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_6 = {"hasDeadline", 1, param_slot_6 };
    static const QUMethod slot_7 = {"showHBoxBelow", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "actionsMenuActivated(int)", &slot_0, QMetaData::Private },
	{ "actionsMenu()", &slot_1, QMetaData::Private },
	{ "addSubTodo()", &slot_2, QMetaData::Private },
	{ "addTodoBelow()", &slot_3, QMetaData::Private },
	{ "addTodoAbove()", &slot_4, QMetaData::Private },
	{ "hasStart(bool)", &slot_5, QMetaData::Private },
	{ "hasDeadline(bool)", &slot_6, QMetaData::Private },
	{ "showHBoxBelow()", &slot_7, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"QTodoItem", parentObject,
	slot_tbl, 8,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QTodoItem.setMetaObject( metaObj );
    return metaObj;
}

void* QTodoItem::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QTodoItem" ) )
	return this;
    return QHBox::qt_cast( clname );
}

bool QTodoItem::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: actionsMenuActivated((int)static_QUType_int.get(_o+1)); break;
    case 1: actionsMenu(); break;
    case 2: addSubTodo(); break;
    case 3: addTodoBelow(); break;
    case 4: addTodoAbove(); break;
    case 5: hasStart((bool)static_QUType_bool.get(_o+1)); break;
    case 6: hasDeadline((bool)static_QUType_bool.get(_o+1)); break;
    case 7: showHBoxBelow(); break;
    default:
	return QHBox::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QTodoItem::qt_emit( int _id, QUObject* _o )
{
    return QHBox::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QTodoItem::qt_property( int id, int f, QVariant* v)
{
    return QHBox::qt_property( id, f, v);
}

bool QTodoItem::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
