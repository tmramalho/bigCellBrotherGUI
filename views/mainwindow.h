#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lib/files/videocontainer.h"
#include "lib/files/tiffcontainer.h"
#include "operationscontroller.h"
#include "improveimage.h"
#include "cropimage.h"
#include "improveimageop.h"
#include "cropimageop.h"
#include "threshold.h"
#include "listopenfiles.h"
#include "thresholdop.h"
#include "createmarkers.h"
#include "createmarkersop.h"
#include "watershed.h"
#include "loadimage.h"
#include "watershedop.h"
#include "executesequence.h"
#include "videoprocessor.h"
#include "createclassifier.h"
#include "picturelabel.h"
#include "listclassifier.h"
#include "parameterloader.h"
#include <iostream>
#include <QtWidgets/QFileDialog>
#include <QDir>
#include <QtWidgets/QMessageBox>
#include <QVariant>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

signals:
	void currentFrameChanged(int);

private slots:
	void openImage();
	void openImageFluorescent();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void fitToWindow();
	void loadParameters();
	void saveParameters();
    void resetPipeline();

	void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

	void on_framePicker_valueChanged(int value);

	void updatePreview(QImage imagePreview);
	void displayExecutionDialog();
    void displayOpenFiles();
	void sequenceProcessingFinished();

private:
	void updateActions();
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	void updateFrameNumberDisplay();

private:
	Ui::MainWindow *ui;
	double scaleFactor;
    FileContainer *videoBox;
    OperationsController opCtr;
    ExecuteSequence *sp;
    CreateClassifier *cc;
    QLabel *imageLabel;
    int currentFrame;
    int bIndex;
    ParameterLoader *parameterManager;
    VideoProcessor *execution;
    QListWidgetItem *qlw;
    CropImage *ci;
    ImproveImage *ii;
    Threshold *th;
    CreateMarkers *cm;
    Watershed *ws;
    ListClassifier *ls;
    LoadImage *li;
    ListOpenFiles *lo;
};

#endif // MAINWINDOW_H
