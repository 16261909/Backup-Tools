QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filterwidget.cpp \
    main.cpp \
    mainwindow.cpp

LIBS += \

HEADERS += \
    filterwidget.h \
    kernel.h \
    mainwindow.h

QMAKE_CXXFLAGS += \
    -lssl \
    -lcrypt \
    -lcrypto


FORMS += \
    filterwidget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lssl3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lssl3
else:unix: LIBS += -L$$PWD/./ -lssl3

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lssl
else:unix: LIBS += -L$$PWD/./ -lssl

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lssl
else:unix: LIBS += -L$$PWD/./ -lssl

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/libssl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/libssl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/ssl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/ssl.lib
else:unix: PRE_TARGETDEPS += $$PWD/./libssl.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lcrypto
else:unix: LIBS += -L$$PWD/./ -lcrypto

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lcrypto
else:unix: LIBS += -L$$PWD/./ -lcrypto

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/libcrypto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/libcrypto.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/crypto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/crypto.lib
else:unix: PRE_TARGETDEPS += $$PWD/./libcrypto.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lcrypt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lcrypt
else:unix: LIBS += -L$$PWD/./ -lcrypt

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lcrypt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lcrypt
else:unix: LIBS += -L$$PWD/./ -lcrypt

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/libcrypt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/libcrypt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./release/crypt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./debug/crypt.lib
else:unix: PRE_TARGETDEPS += $$PWD/./libcrypt.a
