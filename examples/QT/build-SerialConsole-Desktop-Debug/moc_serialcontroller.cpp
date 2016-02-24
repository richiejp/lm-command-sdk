/****************************************************************************
** Meta object code from reading C++ file 'serialcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SerialConsole/serialcontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SerialController_t {
    QByteArrayData data[18];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialController_t qt_meta_stringdata_SerialController = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SerialController"
QT_MOC_LITERAL(1, 17, 6), // "rxData"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 4), // "data"
QT_MOC_LITERAL(4, 30, 5), // "error"
QT_MOC_LITERAL(5, 36, 7), // "message"
QT_MOC_LITERAL(6, 44, 4), // "info"
QT_MOC_LITERAL(7, 49, 5), // "level"
QT_MOC_LITERAL(8, 55, 10), // "foundPorts"
QT_MOC_LITERAL(9, 66, 5), // "ports"
QT_MOC_LITERAL(10, 72, 14), // "foundBaudRates"
QT_MOC_LITERAL(11, 87, 5), // "rates"
QT_MOC_LITERAL(12, 93, 10), // "openedPort"
QT_MOC_LITERAL(13, 104, 10), // "closedPort"
QT_MOC_LITERAL(14, 115, 12), // "requestPorts"
QT_MOC_LITERAL(15, 128, 15), // "requestOpenPort"
QT_MOC_LITERAL(16, 144, 4), // "name"
QT_MOC_LITERAL(17, 149, 11) // "queueTxData"

    },
    "SerialController\0rxData\0\0data\0error\0"
    "message\0info\0level\0foundPorts\0ports\0"
    "foundBaudRates\0rates\0openedPort\0"
    "closedPort\0requestPorts\0requestOpenPort\0"
    "name\0queueTxData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    1,   67,    2, 0x06 /* Public */,
       6,    2,   70,    2, 0x06 /* Public */,
       8,    1,   75,    2, 0x06 /* Public */,
      10,    1,   78,    2, 0x06 /* Public */,
      12,    0,   81,    2, 0x06 /* Public */,
      13,    0,   82,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    0,   83,    2, 0x0a /* Public */,
      15,    1,   84,    2, 0x0a /* Public */,
      17,    1,   87,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    5,    7,
    QMetaType::Void, QMetaType::QVariantList,    9,
    QMetaType::Void, QMetaType::QVariantList,   11,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void SerialController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialController *_t = static_cast<SerialController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rxData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->info((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->foundPorts((*reinterpret_cast< QVariantList(*)>(_a[1]))); break;
        case 4: _t->foundBaudRates((*reinterpret_cast< QVariantList(*)>(_a[1]))); break;
        case 5: _t->openedPort(); break;
        case 6: _t->closedPort(); break;
        case 7: _t->requestPorts(); break;
        case 8: _t->requestOpenPort((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->queueTxData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SerialController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::rxData)) {
                *result = 0;
            }
        }
        {
            typedef void (SerialController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::error)) {
                *result = 1;
            }
        }
        {
            typedef void (SerialController::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::info)) {
                *result = 2;
            }
        }
        {
            typedef void (SerialController::*_t)(QVariantList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::foundPorts)) {
                *result = 3;
            }
        }
        {
            typedef void (SerialController::*_t)(QVariantList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::foundBaudRates)) {
                *result = 4;
            }
        }
        {
            typedef void (SerialController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::openedPort)) {
                *result = 5;
            }
        }
        {
            typedef void (SerialController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SerialController::closedPort)) {
                *result = 6;
            }
        }
    }
}

const QMetaObject SerialController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialController.data,
      qt_meta_data_SerialController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SerialController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SerialController.stringdata0))
        return static_cast<void*>(const_cast< SerialController*>(this));
    return QObject::qt_metacast(_clname);
}

int SerialController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SerialController::rxData(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialController::error(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialController::info(QString _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialController::foundPorts(QVariantList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SerialController::foundBaudRates(QVariantList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialController::openedPort()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void SerialController::closedPort()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
