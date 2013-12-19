#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T10:11:05
#
#-------------------------------------------------

QT       += core gui

TARGET = BigCellBrothergui
TEMPLATE = app

include(./lib/lib.pri)
include(./views/views.pri)
include(./controllers/controllers.pri)

unix|win32|macx: LIBS += -L/opt/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_photo -lopencv_ml -ltiff -lsvm

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include

RESOURCES += \
    assets/icons.qrc


