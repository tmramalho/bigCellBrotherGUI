#-------------------------------------------------
#
# Project created by QtCreator 2012-10-31T10:11:05
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = BigCellBrothergui
TEMPLATE = app

include(./lib/lib.pri)
include(./views/views.pri)
include(./controllers/controllers.pri)

unix|win32|macx: LIBS += -L/opt/local/lib/ -lopencv_core -lopencv_highgui \
                         -lopencv_imgproc -lopencv_photo -lopencv_ml -ltiff -lsvm \
                         -lboost_serialization-mt

INCLUDEPATH += /opt/local/include
DEPENDPATH += /opt/local/include

RESOURCES += \
    assets/icons.qrc

QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_LFLAGS += -stdlib=libc++
QMAKE_INFO_PLIST = /Users/tiago/Library/Qt/5.2.1/clang_64/mkspecs/macx-clang/Info.plist.app

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
