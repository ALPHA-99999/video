QT += core gui network opengl openglwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

exists($$PWD/local_paths.pri) {
    include($$PWD/local_paths.pri)
}

SOURCES += \
    MQTT.cpp \
    command.pb.cc \
    decoder.cpp \
    main.cpp \
    mqttframereceiver.cpp \
    udpreceiver.cpp \
    videoglwidget.cpp \
    widget.cpp

HEADERS += \
    MQTT.h \
    command.pb.h \
    decoder.h \
    mqttframereceiver.h \
    udpreceiver.h \
    videoframe.h \
    videoglwidget.h \
    widget.h

FORMS += \
    widget.ui

VCPKG_ROOT = $$clean_path(D:/GitHub/vcpkg)

isEmpty(FFMPEG_ROOT): FFMPEG_ROOT = $$clean_path($$VCPKG_ROOT/packages/ffmpeg_x64-windows)
isEmpty(PROTOBUF_ROOT): PROTOBUF_ROOT = $$clean_path($$VCPKG_ROOT/packages/protobuf_x64-windows)
isEmpty(ABSEIL_ROOT): ABSEIL_ROOT = $$clean_path($$VCPKG_ROOT/packages/abseil_x64-windows)
isEmpty(QMQTT_LIB_ROOT): QMQTT_LIB_ROOT = $$clean_path($$PWD/lib)


INCLUDEPATH += \
    $$FFMPEG_ROOT/include \
    $$ABSEIL_ROOT/include \
    $$PROTOBUF_ROOT/include \
    $$PWD/include

DEPENDPATH += $$PWD/include

# FFmpeg
LIBS += -L$$FFMPEG_ROOT/lib
LIBS += -lavcodec -lavformat -lavutil -lswscale -lswresample

# protobuf
LIBS += -L$$PROTOBUF_ROOT/lib
win32: LIBS += "$$PROTOBUF_ROOT/lib/libprotobuf.lib"
unix:!macx: LIBS += -lprotobuf

# abseil (Windows runtime package)
win32: LIBS += "$$ABSEIL_ROOT/lib/abseil_dll.lib"

# System libs (Windows)
win32: LIBS += -luser32 -lsecur32 -lbcrypt -lws2_32

# Qt MQTT (single linkage style)
LIBS += -L$$QMQTT_LIB_ROOT -lQt6Qmqtt

win32 {
    RUNTIME_DLL_TARGET = $$clean_path($$OUT_PWD/$${DESTDIR})

    # Keep the IDE run directory self-contained so Qt Creator can launch the app directly.
    QMAKE_POST_LINK += powershell -NoProfile -ExecutionPolicy Bypass -Command "Copy-Item -Force '$$system_path($$FFMPEG_ROOT)/bin/*.dll' '$$system_path($$RUNTIME_DLL_TARGET)'" $$escape_expand(\n\t)
    QMAKE_POST_LINK += powershell -NoProfile -ExecutionPolicy Bypass -Command "Copy-Item -Force '$$system_path($$PROTOBUF_ROOT)/bin/*.dll' '$$system_path($$RUNTIME_DLL_TARGET)'" $$escape_expand(\n\t)
    QMAKE_POST_LINK += powershell -NoProfile -ExecutionPolicy Bypass -Command "Copy-Item -Force '$$system_path($$ABSEIL_ROOT)/bin/*.dll' '$$system_path($$RUNTIME_DLL_TARGET)'" $$escape_expand(\n\t)
    QMAKE_POST_LINK += powershell -NoProfile -ExecutionPolicy Bypass -Command "Copy-Item -Force '$$system_path($$QMQTT_LIB_ROOT)/*.dll' '$$system_path($$RUNTIME_DLL_TARGET)'" $$escape_expand(\n\t)
}

# Default rules for deployment.
win32: target.path = /bin
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    # Third-party runtime deployment
    thirdparty_deployment.path = /bin
    thirdparty_deployment.files = $$files($$PROTOBUF_ROOT/bin/*.dll)
    thirdparty_deployment.files += $$files($$ABSEIL_ROOT/bin/*.dll)
    thirdparty_deployment.files += $$files($$FFMPEG_ROOT/bin/*.dll)
    thirdparty_deployment.files += $$files($$QMQTT_LIB_ROOT/*.dll)

    INSTALLS += thirdparty_deployment
}
