QT += core gui network opengl openglwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 link_pkgconfig

exists($$PWD/local_paths.pri) {
    include($$PWD/local_paths.pri)
}

isEmpty(QMQTT_ROOT): QMQTT_ROOT = /home/arty/Documents/video_ros2/install

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

RESOURCES += \
    resources.qrc

INCLUDEPATH += \
    $$QMQTT_ROOT/include \
    $$PWD/include

DEPENDPATH += $$PWD/include

unix: PKGCONFIG += libavcodec libavformat libavutil libswscale libswresample protobuf
LIBS += -L$$QMQTT_ROOT/lib -lqmqtt
QMAKE_RPATHDIR += $$QMQTT_ROOT/lib

# Default rules for deployment.
unix: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
