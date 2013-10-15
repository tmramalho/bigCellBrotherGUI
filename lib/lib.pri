INCLUDEPATH += $$PWD
SOURCES +=  $$PWD/classifier/CellClassifier.cpp \
    $$PWD/classifier/NaiveBayes.cpp \
    $$PWD/classifier/SVMachine.cpp \
    $$PWD/files/CSVReader.cpp \
    $$PWD/files/filecontainer.cpp \
    $$PWD/files/tiffcontainer.cpp \
    $$PWD/files/videocontainer.cpp \
    $$PWD/helpers/CellCont.cpp \
    $$PWD/helpers/Histogram1D.cpp \
    $$PWD/helpers/PictureVis.cpp \
    $$PWD/processors/ImageProcessor.cpp \
    $$PWD/processors/ImageSegmentor.cpp \
    $$PWD/processors/ScientificProcessor.cpp
    

HEADERS  += $$PWD/classifier/CellClassifier.h \
    $$PWD/classifier/NaiveBayes.h \
    $$PWD/classifier/SVMachine.h \
    $$PWD/files/CSVReader.h \
    $$PWD/files/filecontainer.h \
    $$PWD/files/tiffcontainer.h \
    $$PWD/files/videocontainer.h \
    $$PWD/helpers/CellCont.h \
    $$PWD/helpers/markersCont.h \
    $$PWD/helpers/Histogram1D.h \
    $$PWD/helpers/PictureVis.h \
    $$PWD/processors/ImageProcessor.h \
    $$PWD/processors/ImageSegmentor.h \
    $$PWD/processors/ScientificProcessor.h