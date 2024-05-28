# Add common source files here
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
           Pub/global.h \
           Pub/os_header.h \
           Pub/ss_pub.h \
    $$PWD/global_new.h

SOURCES += \
           Pub/global.cpp \
           Pub/ss_pub.cpp \
    $$PWD/global_new.cpp
