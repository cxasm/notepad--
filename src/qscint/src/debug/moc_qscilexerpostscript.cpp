/****************************************************************************
** Meta object code from reading C++ file 'qscilexerpostscript.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Qsci/qscilexerpostscript.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscilexerpostscript.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QsciLexerPostScript_t {
    QByteArrayData data[9];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QsciLexerPostScript_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QsciLexerPostScript_t qt_meta_stringdata_QsciLexerPostScript = {
    {
QT_MOC_LITERAL(0, 0, 19), // "QsciLexerPostScript"
QT_MOC_LITERAL(1, 20, 11), // "setTokenize"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "tokenize"
QT_MOC_LITERAL(4, 42, 8), // "setLevel"
QT_MOC_LITERAL(5, 51, 5), // "level"
QT_MOC_LITERAL(6, 57, 14), // "setFoldCompact"
QT_MOC_LITERAL(7, 72, 4), // "fold"
QT_MOC_LITERAL(8, 77, 13) // "setFoldAtElse"

    },
    "QsciLexerPostScript\0setTokenize\0\0"
    "tokenize\0setLevel\0level\0setFoldCompact\0"
    "fold\0setFoldAtElse"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QsciLexerPostScript[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       4,    1,   37,    2, 0x0a /* Public */,
       6,    1,   40,    2, 0x0a /* Public */,
       8,    1,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Bool,    7,

       0        // eod
};

void QsciLexerPostScript::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QsciLexerPostScript *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setTokenize((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->setLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setFoldCompact((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setFoldAtElse((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QsciLexerPostScript::staticMetaObject = { {
    &QsciLexer::staticMetaObject,
    qt_meta_stringdata_QsciLexerPostScript.data,
    qt_meta_data_QsciLexerPostScript,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QsciLexerPostScript::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QsciLexerPostScript::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QsciLexerPostScript.stringdata0))
        return static_cast<void*>(this);
    return QsciLexer::qt_metacast(_clname);
}

int QsciLexerPostScript::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QsciLexer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
