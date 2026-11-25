QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    entidadbase.cpp \
    escenabase.cpp \
    escenanivel1.cpp \
    escenanivel2.cpp \
    fisicas.cpp \
    main.cpp \
    mainwindow.cpp \
    spiderman.cpp \
    escenario.cpp \
    spritesheet.cpp

HEADERS += \
    entidadbase.h \
    escenabase.h \
    escenanivel1.h \
    escenanivel2.h \
    fisicas.h \
    mainwindow.h \
    spiderman.h \
    escenario.h \
    spritesheet.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
RESOURCES += \
    recursos.qrc
