#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T10:11:05
#
#-------------------------------------------------

QT       += core gui

TARGET = BigCellBrothergui
TEMPLATE = app

include(./views/views.pri)
include(./controllers/controllers.pri)
include(./lib/lib.pri)

unix|win32|macx: LIBS += -L/opt/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_photo -lopencv_ml -ltiff -lsvm

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include
