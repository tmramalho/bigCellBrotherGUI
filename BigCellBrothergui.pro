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

unix|win32|macx: LIBS += -L/opt/local/lib/ -L$$PWD/../bigCellBrother/Maclib/ -lbigCellBrother -lopencv_core -lopencv_highgui -lopencv_imgproc -ltiff

INCLUDEPATH += $$PWD/../bigCellBrother
DEPENDPATH += $$PWD/../bigCellBrother

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include
