QT += core gui network opengl openglwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    MQTT.cpp \
    command.pb.cc \
    decoder.cpp \
    main.cpp \
    udpreceiver.cpp \
    videoglwidget.cpp \
    widget.cpp

HEADERS += \
    MQTT.h \
    command.pb.h \
    decoder.h \
    udpreceiver.h \
    videoframe.h \
    videoglwidget.h \
    widget.h

FORMS += \
    widget.ui

# Local machine paths (optional override file, not tracked)
exists(local_paths.pri) {
    include(local_paths.pri)
} else {
    warning("local_paths.pri not found. Using default local paths in untitled.pro.")
}

isEmpty(FFMPEG_ROOT) {
    FFMPEG_ROOT = D:/GitHub/vcpkg/packages/ffmpeg_x64-windows
    message("FFMPEG_ROOT default: $$FFMPEG_ROOT")
}

isEmpty(PROTOBUF_ROOT) {
    PROTOBUF_ROOT = D:/GitHub/vcpkg/packages/protobuf_x64-windows
    message("PROTOBUF_ROOT default: $$PROTOBUF_ROOT")
}

isEmpty(ABSEIL_ROOT) {
    ABSEIL_ROOT = D:/GitHub/vcpkg/packages/abseil_x64-windows
    message("ABSEIL_ROOT default: $$ABSEIL_ROOT")
}

isEmpty(QMQTT_LIB_ROOT) {
    QMQTT_LIB_ROOT = $$PWD/lib
    message("QMQTT_LIB_ROOT default: $$QMQTT_LIB_ROOT")
}

INCLUDEPATH += \
    $$FFMPEG_ROOT/include \
    $$ABSEIL_ROOT/include \
    $$PROTOBUF_ROOT/include \
    $$PWD/include

DEPENDPATH += $$PWD/include

# FFmpeg
LIBS += -L$$FFMPEG_ROOT/lib
LIBS += -lavcodec -lavformat -lavutil -lswscale -lswresample

# System libs (Windows)
LIBS += -luser32 -lsecur32 -lbcrypt -lws2_32

# protobuf + abseil
LIBS += "$$PROTOBUF_ROOT/lib/libprotobuf.lib"
LIBS += "$$ABSEIL_ROOT/lib/abseil_dll.lib"

# Qt MQTT (single linkage style)
LIBS += -L$$QMQTT_LIB_ROOT -lQt6Qmqtt

# Default rules for deployment.
win32: target.path = /bin
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Third-party runtime deployment
thirdparty_deployment.path = /bin
thirdparty_deployment.files = $$files($$PROTOBUF_ROOT/bin/*.dll)
thirdparty_deployment.files += $$files($$ABSEIL_ROOT/bin/*.dll)
thirdparty_deployment.files += $$files($$FFMPEG_ROOT/bin/*.dll)
thirdparty_deployment.files += $$files($$QMQTT_LIB_ROOT/*.dll)

INSTALLS += thirdparty_deployment
