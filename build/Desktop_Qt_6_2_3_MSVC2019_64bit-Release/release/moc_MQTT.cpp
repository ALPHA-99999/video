/****************************************************************************
** Meta object code from reading C++ file 'MQTT.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../MQTT.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MQTT.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MQTT_t {
    const uint offsetsAndSize[20];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MQTT_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MQTT_t qt_meta_stringdata_MQTT = {
    {
QT_MOC_LITERAL(0, 4), // "MQTT"
QT_MOC_LITERAL(5, 11), // "onConnected"
QT_MOC_LITERAL(17, 0), // ""
QT_MOC_LITERAL(18, 12), // "onSubscribed"
QT_MOC_LITERAL(31, 5), // "topic"
QT_MOC_LITERAL(37, 11), // "onPublished"
QT_MOC_LITERAL(49, 14), // "QMQTT::Message"
QT_MOC_LITERAL(64, 7), // "message"
QT_MOC_LITERAL(72, 10), // "onReceived"
QT_MOC_LITERAL(83, 14) // "onDisconnected"

    },
    "MQTT\0onConnected\0\0onSubscribed\0topic\0"
    "onPublished\0QMQTT::Message\0message\0"
    "onReceived\0onDisconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MQTT[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x0a,    1 /* Public */,
       3,    1,   45,    2, 0x0a,    2 /* Public */,
       5,    1,   48,    2, 0x0a,    4 /* Public */,
       8,    1,   51,    2, 0x0a,    6 /* Public */,
       9,    0,   54,    2, 0x0a,    8 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,

       0        // eod
};

void MQTT::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MQTT *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onConnected(); break;
        case 1: _t->onSubscribed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onPublished((*reinterpret_cast< const QMQTT::Message(*)>(_a[1]))); break;
        case 3: _t->onReceived((*reinterpret_cast< const QMQTT::Message(*)>(_a[1]))); break;
        case 4: _t->onDisconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QMQTT::Message >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QMQTT::Message >(); break;
            }
            break;
        }
    }
}

const QMetaObject MQTT::staticMetaObject = { {
    QMetaObject::SuperData::link<QMQTT::Client::staticMetaObject>(),
    qt_meta_stringdata_MQTT.offsetsAndSize,
    qt_meta_data_MQTT,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MQTT_t
, QtPrivate::TypeAndForceComplete<MQTT, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QMQTT::Message &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QMQTT::Message &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *MQTT::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MQTT::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MQTT.stringdata0))
        return static_cast<void*>(this);
    return QMQTT::Client::qt_metacast(_clname);
}

int MQTT::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMQTT::Client::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
