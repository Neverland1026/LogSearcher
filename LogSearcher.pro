QT += quick
//QT += core5compat

SOURCES += \
        LogSearcher.cpp \
        main.cpp

RESOURCES += qml.qrc

RC_FILE = logo.rc

QT += widgets

QT += concurrent

LIBS += -lUser32

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#HEADERS += \
#        CustomImageProvider.h

HEADERS += \
        LogSearcher.h
