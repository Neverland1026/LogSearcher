QT += quick
//QT += core5compat

SOURCES += \
        LogAnalysis/LogUtils.cpp \
        LogLoad/LogLoaderThread.cpp \
        LogModel/ChartDataModel.cpp \
        LogModel/LogModel.cpp \
        LogSearcher.cpp \
        main.cpp

QT += quick quickcontrols2 charts
QT += charts

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

HEADERS += \
        LogAnalysis/LogUtils.h \
        LogLoad/LogLoaderThread.h \
        LogModel/ChartDataModel.h \
        LogModel/LogModel.h \
        LogSearcher.h \
        LogUltraSearchTool.h
