QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
message(Qt kit version $$QT_VERSION)

CONFIG += c++17
QMAKE_CXXFLAGS += -O3 -march=native -finline-functions -funswitch-loops
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
ROOT      = $$PWD
QT_TG_BOT = $$ROOT/QtTelegramBot
IMGS  = $$ROOT/imgs
ICONS = $$IMGS/icons
message(Root path: $$ROOT)
message(TG bot lib path: $$QT_TG_BOT)
message(Imgs path: $$IMGS)
message(Icons path: $$ICONS)

include($$QT_TG_BOT/QtTelegramBot.pri)

SOURCES += \
    Controller.cpp \
    Database.cpp \
    Settings.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Controller.h \
    Database.h \
    Settings.h \
    mainwindow.h \
    types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

VERSION                  = 0.1
QMAKE_TARGET_COMPANY     =
QMAKE_TARGET_PRODUCT     =
QMAKE_TARGET_DESCRIPTION =
QMAKE_TARGET_COPYRIGHT   =

win32:RC_ICONS += $$ICONS/telegram_64px.ico

RESOURCES += \
    rc.qrc
