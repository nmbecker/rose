/****************************************************************************
** Meta object code from reading C++ file 'QRButtons.h'
**
** Created: Mon Oct 20 12:09:34 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QRButtons.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QRButtons.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qrs__QRButtons[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   16,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_qrs__QRButtons[] = {
    "qrs::QRButtons\0\0id\0clicked(int)\0"
    "clickEvent(int)\0"
};

const QMetaObject qrs::QRButtons::staticMetaObject = {
    { &QRPanel::staticMetaObject, qt_meta_stringdata_qrs__QRButtons,
      qt_meta_data_qrs__QRButtons, 0 }
};

const QMetaObject *qrs::QRButtons::metaObject() const
{
    return &staticMetaObject;
}

void *qrs::QRButtons::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qrs__QRButtons))
        return static_cast<void*>(const_cast< QRButtons*>(this));
    return QRPanel::qt_metacast(_clname);
}

int qrs::QRButtons::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QRPanel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: clickEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void qrs::QRButtons::clicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
