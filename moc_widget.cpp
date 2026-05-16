/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "widget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Widget_t {
    const uint offsetsAndSize[42];
    char stringdata0[290];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 6), // "Widget"
QT_MOC_LITERAL(7, 18), // "onMqttLoginClicked"
QT_MOC_LITERAL(26, 0), // ""
QT_MOC_LITERAL(27, 19), // "onMqttLogoutClicked"
QT_MOC_LITERAL(47, 15), // "onMqttConnected"
QT_MOC_LITERAL(63, 18), // "onMqttDisconnected"
QT_MOC_LITERAL(82, 11), // "onMqttError"
QT_MOC_LITERAL(94, 18), // "QMQTT::ClientError"
QT_MOC_LITERAL(113, 5), // "error"
QT_MOC_LITERAL(119, 18), // "onMqttLoginTimeout"
QT_MOC_LITERAL(138, 17), // "onUdpDecodedFrame"
QT_MOC_LITERAL(156, 10), // "VideoFrame"
QT_MOC_LITERAL(167, 5), // "frame"
QT_MOC_LITERAL(173, 18), // "onMqttDecodedFrame"
QT_MOC_LITERAL(192, 16), // "onFramePresented"
QT_MOC_LITERAL(209, 14), // "udpFirstRecvMs"
QT_MOC_LITERAL(224, 14), // "udpAssembledMs"
QT_MOC_LITERAL(239, 13), // "decodeStartMs"
QT_MOC_LITERAL(253, 12), // "decodeDoneMs"
QT_MOC_LITERAL(266, 10), // "uiSubmitMs"
QT_MOC_LITERAL(277, 12) // "renderDoneMs"

    },
    "Widget\0onMqttLoginClicked\0\0"
    "onMqttLogoutClicked\0onMqttConnected\0"
    "onMqttDisconnected\0onMqttError\0"
    "QMQTT::ClientError\0error\0onMqttLoginTimeout\0"
    "onUdpDecodedFrame\0VideoFrame\0frame\0"
    "onMqttDecodedFrame\0onFramePresented\0"
    "udpFirstRecvMs\0udpAssembledMs\0"
    "decodeStartMs\0decodeDoneMs\0uiSubmitMs\0"
    "renderDoneMs"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x08,    1 /* Private */,
       3,    0,   69,    2, 0x08,    2 /* Private */,
       4,    0,   70,    2, 0x08,    3 /* Private */,
       5,    0,   71,    2, 0x08,    4 /* Private */,
       6,    1,   72,    2, 0x08,    5 /* Private */,
       9,    0,   75,    2, 0x08,    7 /* Private */,
      10,    1,   76,    2, 0x08,    8 /* Private */,
      13,    1,   79,    2, 0x08,   10 /* Private */,
      14,    6,   82,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong,   15,   16,   17,   18,   19,   20,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Widget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMqttLoginClicked(); break;
        case 1: _t->onMqttLogoutClicked(); break;
        case 2: _t->onMqttConnected(); break;
        case 3: _t->onMqttDisconnected(); break;
        case 4: _t->onMqttError((*reinterpret_cast< std::add_pointer_t<QMQTT::ClientError>>(_a[1]))); break;
        case 5: _t->onMqttLoginTimeout(); break;
        case 6: _t->onUdpDecodedFrame((*reinterpret_cast< std::add_pointer_t<VideoFrame>>(_a[1]))); break;
        case 7: _t->onMqttDecodedFrame((*reinterpret_cast< std::add_pointer_t<VideoFrame>>(_a[1]))); break;
        case 8: _t->onFramePresented((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[6]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QMQTT::ClientError >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< VideoFrame >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< VideoFrame >(); break;
            }
            break;
        }
    }
}

const QMetaObject Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Widget.offsetsAndSize,
    qt_meta_data_Widget,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Widget_t
, QtPrivate::TypeAndForceComplete<Widget, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QMQTT::ClientError, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const VideoFrame &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const VideoFrame &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>


>,
    nullptr
} };


const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Widget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
