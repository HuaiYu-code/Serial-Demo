#-------------------------------------------------
#
# Project created by QtCreator 2017-05-23T21:24:35
#
#-------------------------------------------------

QT       += core gui serialport widgets winextras multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Newline_assistant
TEMPLATE = app


include(Pub/pub.pri)
INCLUDEPATH += Gui
INCLUDEPATH += assistant/libs/ophelia
INCLUDEPATH += $$(BOOST_DIR)
INCLUDEPATH += assistant/libs/hht_helper/include
INCLUDEPATH += assistant/libs/WTL10_8280/Include
LIBS += -L$$(BOOST_DIR)/lib64-msvc-14.1/ -llibboost_date_time-vc141-mt-gd-x64-1_70
LIBS += -L$$PWD/assistant/libs/hht_helper/lib/ -ldesktop_app_manager
LIBS += -LC:/win/boost_1_70_0/lib64-msvc-14.1/ -llibboost_date_time-vc141-mt-gd-x64-1_70
TR_EXCLUDE = $$(BOOST_DIR)\*

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
win32: QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS_RELEASE -= -Zc:strictStrings
win32: QMAKE_CFLAGS -= -Zc:strictStrings
win32: QMAKE_CXXFLAGS -= -Zc:strictStrings

###注:国内与国外版本，只需修改以下这个宏定义即可###
DEFINES += HHT_CHINA   #国内 115200bps
#DEFINES += HHT_CHINA_MCU=1 #国内MCU 19200bps Version

#############################################################################
##目前对应的版本，即 HHT_RELEASE，HHT_GERMANY
###以下是各个版本的描述###
## HHT_RELEASE 对应对外发布版本，
## HHT_GERMANY 对应德国固件发布版本,
#############################################################################
#############################################################################
###注：编译不同版本时，只需修改以下这个宏定义即可###
DEFINES += HHT_GERMANY UNICODE
#############################################################################

SOURCES += main.cpp\
    assistant/src/NetWork.cpp \
    assistant/src/WindowsThemeColorApi.cpp \
    assistant/src/command.cpp \
    assistant/src/commandParser.cpp \
    assistant/src/ioservice.cpp \
    assistant/src/settings.cpp \
    assistant/src/windows_performance_util.cpp \
    assistant/src/windows_sys_info.cpp \
    assistant/src/windows_theme_color.cpp \
    assistant/src/wmi_util.cpp \
        maindialog.cpp \
    Helper/iconhelper.cpp \
    Gui/aboutdialog.cpp \
    Gui/frmmessagebox.cpp \
    nlistwidget.cpp \
    rollinglable.cpp \
    syncwidget.cpp \
    trashwidget.cpp \
    Gui/hintdialog.cpp \
    maskwidget.cpp \
    Gui/uploadwidget.cpp \
    Gui/montagedialog.cpp \
    Helper/fontscaleratio.cpp \
    uploadsucwidget.cpp \
    singleapplication.cpp

HEADERS  += maindialog.h \
    assistant/libs/ophelia/network/basicUdpServer.hpp \
    assistant/src/assistantHostFinder.hpp \
    hhtheader.h \
    Helper/hhthelper.h \
    Helper/iconhelper.h \
    Gui/aboutdialog.h \
    Gui/frmmessagebox.h \
    nlistwidget.h \
    rollinglable.h \
    syncwidget.h \
    trashwidget.h \
    Gui/hintdialog.h \
    maskwidget.h \
    Gui/uploadwidget.h \
    Gui/montagedialog.h \
    Helper/fontscaleratio.h \
    uploadsucwidget.h \
    assistant/src/settings.hpp \
    assistant/src/command_code.hpp \
    assistant/src/hitevisionAssistantService.hpp \
    singleapplication.h

FORMS    += maindialog.ui \
    Gui/aboutdialog.ui \
    Gui/frmmessagebox.ui \
    Gui/hintdialog.ui \
    Gui/uploadwidget.ui \
    Gui/montagedialog.ui \

TRANSLATIONS += lang_Indonesia.ts


RESOURCES += \
    resource.qrc

contains(DEFINES, HHT_CHINA){
    RC_FILE += newline-2nd.rc#app_ch.rc
    message("-->>CHINA")
}else{
    RC_FILE += app.rc
    message("-->>OVERSEAS")
}
DISTFILES += \
    Resource/images/alert.png \
    Resource/images/warning.png \
    lang_Svenska.qm

#LIBS += qtmaind.lib \
#        shell32.lib \
#        User32.lib \
