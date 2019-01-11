#****************************************************************************
#  Copyright (C) 2018 Ken Lindsay
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#*******************************************************************************

QT     += core gui
CONFIG += c++11
# Comment out this line for actual use, otherwise simulated behavior
CONFIG += simulate

#DEFINES += USE_SIX_BYTE # For unsuported six byte functions
DEFINES += DEBUG

greaterThan(QT_MAJOR_VERSION, 4)

TARGET = throttle
TEMPLATE = app
QT += widgets
RESOURCES += resources.qrc

simulate{
    DEFINES += PR3_SIM
    HEADERS += $$PWD/../../LocoNet/src/pr3_io_sim.h \
               $$PWD/../../LocoNet/src/loco_sim.h
    SOURCES += $$PWD/../../LocoNet/src/loco_sim.cpp \
               $$PWD/../../LocoNet/src/pr3_io_sim.cpp

}

SOURCES += main.cpp \
           mainwindow.cpp \
           throttle.cpp \
           config.cpp \
           serial_io_class.cpp \
           get_line_class.cpp \
           $$PWD/../../LocoNet/src/loco_net.cpp \
           $$PWD/../../LocoNet/src/ln_buf.cpp \
           $$PWD/../../LocoNet/src/ln_throttle.cpp \

HEADERS += mainwindow.h \
    throttle.h \
    config.h \
    serial_io_class.h \
    get_line_class.h \
    $$PWD/../../LocoNet/src/loco_net.h \
    $$PWD/../../LocoNet/src/ln_opc.h \
    $$PWD/../../LocoNet/src/ln_buf.h \
    $$PWD/../../LocoNet/src/ln_throttle.h

FORMS += mainwindow.ui

INCLUDEPATH += $$PWD/../../LocoNet/src

DEPENDPATH  += $$PWD/../../LocoNet/src

DISTFILES +=



