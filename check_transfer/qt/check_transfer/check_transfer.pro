QT += core
QT -= gui

CONFIG += c++11

TARGET = check_transfer
DESTDIR = ../../bin
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    xcl.cpp \
    ../shared/exceptinfo.cpp \
    ../shared/table_engine_console.cpp \
    ../shared/tf_testthread.cpp \
    ../shared/ipc.cpp \
    tf_checktransferout.cpp \
    ../shared/xcl2.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../shared

HEADERS += \
    xcl.h \
    ../shared/exceptinfo.h \
    ../shared/table_engine.h \
    ../shared/table_engine_console.h \
    ../shared/term_table.h \
    ../shared/tf_test.h \
    ../shared/tf_testthread.h \
    ../shared/ipc.h \
    tf_checktransferout.h \
    ../shared/utypes.h \
    ../shared/xcl2.hpp
# host compiler global settings
#CXXFLAGS += -DSDX_PLATFORM=$(SDX_PLATFORM) -D__USE_XOPEN2K8 -I/opt/Xilinx/SDx/2017.1/runtime/include/1_2/ -I/opt/Xilinx/SDx/2017.1/Vivado_HLS/include/ -O2 -Wall -c -fmessage-length=0 -std=c++14
#LDFLAGS += -lxilinxopencl -lpthread -lrt -lstdc++ -L/opt/Xilinx/SDx/2017.1/runtime/lib/x86_64

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += /opt/Xilinx/SDx/2017.1/Vivado_HLS/include \
               /opt/Xilinx/SDx/2017.1/runtime/include/1_2/

SDX_PLATFORM = xilinx:adm-pcie-ku3:2ddr:3.3

DEFINES += SDX_PLATFORM=$$SDX_PLATFORM __USE_XOPEN2K8

LIBS += -L/opt/Xilinx/SDx/2017.1/runtime/lib/x86_64
LIBS += -lxilinxopencl -lpthread -lrt -lstdc++

