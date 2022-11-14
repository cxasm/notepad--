/****************************************************************************
** Meta object code from reading C++ file 'qscilexer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Qsci/qscilexer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscilexer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QsciLexer_t {
    QByteArrayData data[21];
    char stringdata0[201];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QsciLexer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QsciLexer_t qt_meta_stringdata_QsciLexer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "QsciLexer"
QT_MOC_LITERAL(1, 10, 12), // "colorChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 1), // "c"
QT_MOC_LITERAL(4, 26, 5), // "style"
QT_MOC_LITERAL(5, 32, 14), // "eolFillChanged"
QT_MOC_LITERAL(6, 47, 9), // "eolfilled"
QT_MOC_LITERAL(7, 57, 11), // "fontChanged"
QT_MOC_LITERAL(8, 69, 1), // "f"
QT_MOC_LITERAL(9, 71, 12), // "paperChanged"
QT_MOC_LITERAL(10, 84, 15), // "propertyChanged"
QT_MOC_LITERAL(11, 100, 11), // "const char*"
QT_MOC_LITERAL(12, 112, 4), // "prop"
QT_MOC_LITERAL(13, 117, 3), // "val"
QT_MOC_LITERAL(14, 121, 18), // "setAutoIndentStyle"
QT_MOC_LITERAL(15, 140, 15), // "autoindentstyle"
QT_MOC_LITERAL(16, 156, 8), // "setColor"
QT_MOC_LITERAL(17, 165, 10), // "setEolFill"
QT_MOC_LITERAL(18, 176, 7), // "eoffill"
QT_MOC_LITERAL(19, 184, 7), // "setFont"
QT_MOC_LITERAL(20, 192, 8) // "setPaper"

    },
    "QsciLexer\0colorChanged\0\0c\0style\0"
    "eolFillChanged\0eolfilled\0fontChanged\0"
    "f\0paperChanged\0propertyChanged\0"
    "const char*\0prop\0val\0setAutoIndentStyle\0"
    "autoindentstyle\0setColor\0setEolFill\0"
    "eoffill\0setFont\0setPaper"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QsciLexer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   84,    2, 0x06 /* Public */,
       5,    2,   89,    2, 0x06 /* Public */,
       7,    2,   94,    2, 0x06 /* Public */,
       9,    2,   99,    2, 0x06 /* Public */,
      10,    2,  104,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    1,  109,    2, 0x0a /* Public */,
      16,    2,  112,    2, 0x0a /* Public */,
      16,    1,  117,    2, 0x2a /* Public | MethodCloned */,
      17,    2,  120,    2, 0x0a /* Public */,
      17,    1,  125,    2, 0x2a /* Public | MethodCloned */,
      19,    2,  128,    2, 0x0a /* Public */,
      19,    1,  133,    2, 0x2a /* Public | MethodCloned */,
      20,    2,  136,    2, 0x0a /* Public */,
      20,    1,  141,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, QMetaType::QColor, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    6,    4,
    QMetaType::Void, QMetaType::QFont, QMetaType::Int,    8,    4,
    QMetaType::Void, QMetaType::QColor, QMetaType::Int,    3,    4,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 11,   12,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::QColor, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,   18,    4,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void, QMetaType::QFont, QMetaType::Int,    8,    4,
    QMetaType::Void, QMetaType::QFont,    8,
    QMetaType::Void, QMetaType::QColor, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QColor,    3,

       0        // eod
};

void QsciLexer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QsciLexer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->colorChanged((*reinterpret_cast< const QColor(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->eolFillChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->fontChanged((*reinterpret_cast< const QFont(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->paperChanged((*reinterpret_cast< const QColor(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->propertyChanged((*reinterpret_cast< const char*(*)>(_a[1])),(*reinterpret_cast< const char*(*)>(_a[2]))); break;
        case 5: _t->setAutoIndentStyle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->setColor((*reinterpret_cast< const QColor(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->setColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 8: _t->setEolFill((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->setEolFill((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->setFont((*reinterpret_cast< const QFont(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->setFont((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 12: _t->setPaper((*reinterpret_cast< const QColor(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->setPaper((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QsciLexer::*)(const QColor & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QsciLexer::colorChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QsciLexer::*)(bool , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QsciLexer::eolFillChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QsciLexer::*)(const QFont & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QsciLexer::fontChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QsciLexer::*)(const QColor & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QsciLexer::paperChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QsciLexer::*)(const char * , const char * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QsciLexer::propertyChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QsciLexer::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_QsciLexer.data,
    qt_meta_data_QsciLexer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QsciLexer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QsciLexer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QsciLexer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QsciLexer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void QsciLexer::colorChanged(const QColor & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QsciLexer::eolFillChanged(bool _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QsciLexer::fontChanged(const QFont & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QsciLexer::paperChanged(const QColor & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QsciLexer::propertyChanged(const char * _t1, const char * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
