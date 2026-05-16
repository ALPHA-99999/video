/****************************************************************************
** Meta object code from reading C++ file 'videoglwidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "videoglwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videoglwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VideoGLWidget_t {
    const uint offsetsAndSize[24];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_VideoGLWidget_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_VideoGLWidget_t qt_meta_stringdata_VideoGLWidget = {
    {
QT_MOC_LITERAL(0, 13), // "VideoGLWidget"
QT_MOC_LITERAL(14, 14), // "framePresented"
QT_MOC_LITERAL(29, 0), // ""
QT_MOC_LITERAL(30, 14), // "udpFirstRecvMs"
QT_MOC_LITERAL(45, 14), // "udpAssembledMs"
QT_MOC_LITERAL(60, 13), // "decodeStartMs"
QT_MOC_LITERAL(74, 12), // "decodeDoneMs"
QT_MOC_LITERAL(87, 10), // "uiSubmitMs"
QT_MOC_LITERAL(98, 12), // "renderDoneMs"
QT_MOC_LITERAL(111, 11), // "submitFrame"
QT_MOC_LITERAL(123, 10), // "VideoFrame"
QT_MOC_LITERAL(134, 5) // "frame"

    },
    "VideoGLWidget\0framePresented\0\0"
    "udpFirstRecvMs\0udpAssembledMs\0"
    "decodeStartMs\0decodeDoneMs\0uiSubmitMs\0"
    "renderDoneMs\0submitFrame\0VideoFrame\0"
    "frame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoGLWidget[] = {

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
       1,    6,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    1,   39,    2, 0x0a,    8 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong,    3,    4,    5,    6,    7,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,

       0        // eod
};

void VideoGLWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VideoGLWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->framePresented((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[6]))); break;
        case 1: _t->submitFrame((*reinterpret_cast< std::add_pointer_t<VideoFrame>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< VideoFrame >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VideoGLWidget::*)(qint64 , qint64 , qint64 , qint64 , qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoGLWidget::framePresented)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject VideoGLWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_VideoGLWidget.offsetsAndSize,
    qt_meta_data_VideoGLWidget,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_VideoGLWidget_t
, QtPrivate::TypeAndForceComplete<VideoGLWidget, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>, QtPrivate::TypeAndForceComplete<qint64, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const VideoFrame &, std::false_type>


>,
    nullptr
} };


const QMetaObject *VideoGLWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoGLWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VideoGLWidget.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions"))
        return static_cast< QOpenGLFunctions*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int VideoGLWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void VideoGLWidget::framePresented(qint64 _t1, qint64 _t2, qint64 _t3, qint64 _t4, qint64 _t5, qint64 _t6)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
