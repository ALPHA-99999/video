/****************************************************************************
** Meta object code from reading C++ file 'mqttframereceiver.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mqttframereceiver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mqttframereceiver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MqttFrameReceiver_t {
    const uint offsetsAndSize[18];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MqttFrameReceiver_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MqttFrameReceiver_t qt_meta_stringdata_MqttFrameReceiver = {
    {
QT_MOC_LITERAL(0, 17), // "MqttFrameReceiver"
QT_MOC_LITERAL(18, 14), // "frame_toproess"
QT_MOC_LITERAL(33, 0), // ""
QT_MOC_LITERAL(34, 9), // "frameData"
QT_MOC_LITERAL(44, 11), // "firstRecvMs"
QT_MOC_LITERAL(56, 11), // "assembledMs"
QT_MOC_LITERAL(68, 12), // "ingestPacket"
QT_MOC_LITERAL(81, 6), // "packet"
QT_MOC_LITERAL(88, 6) // "recvMs"

    },
    "MqttFrameReceiver\0frame_toproess\0\0"
    "frameData\0firstRecvMs\0assembledMs\0"
    "ingestPacket\0packet\0recvMs"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MqttFrameReceiver[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    2,   33,    2, 0x0a,    5 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray, QMetaType::LongLong, QMetaType::LongLong,    3,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray, QMetaType::LongLong,    7,    8,

       0        // eod
};

void MqttFrameReceiver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MqttFrameReceiver *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->frame_toproess((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[3]))); break;
        case 1: _t->ingestPacket((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MqttFrameReceiver::*)(QByteArray , qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MqttFrameReceiver::frame_toproess)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MqttFrameReceiver::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MqttFrameReceiver.offsetsAndSize,
    qt_meta_data_MqttFrameReceiver,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MqttFrameReceiver_t
, QtPrivate::TypeAndForceComplete<MqttFrameReceiver, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>


>,
    nullptr
} };


const QMetaObject *MqttFrameReceiver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MqttFrameReceiver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MqttFrameReceiver.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MqttFrameReceiver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MqttFrameReceiver::frame_toproess(QByteArray _t1, qint64 _t2, qint64 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
