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

unix|win32: LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc

unix:!symbian|win32: LIBS += -L$$PWD/../../../../media/sf_tiago/Dropbox/workspace/bigCellBrother/bigCellBrother/Library/ -lbigCellBrother

INCLUDEPATH += $$PWD/../../../../media/sf_tiago/Dropbox/workspace/bigCellBrother/bigCellBrother
DEPENDPATH += $$PWD/../../../../media/sf_tiago/Dropbox/workspace/bigCellBrother/bigCellBrother
