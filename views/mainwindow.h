#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "files/videocontainer.h"
#include "files/tiffcontainer.h"
#include "operationscontroller.h"
#include "improveimage.h"
#include "cropimage.h"
#include "improveimageop.h"
#include "cropimageop.h"
#include "threshold.h"
#include "thresholdop.h"
#include "createmarkers.h"
#include "createmarkersop.h"
#include "watershed.h"
#include "watershedop.h"
#include "executesequence.h"
#include "videoprocessor.h"
#include "createclassifier.h"
#include "picturelabel.h"
#include "listclassifier.h"
#include "parameterloader.h"
#include <iostream>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QVariant>
#include <QMainWindow>
#include <QScrollBar>
#include <QListWidgetItem>

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

	void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

	void on_framePicker_valueChanged(int value);

	void updatePreview(QImage imagePreview);
	void displayExecutionDialog();
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
	FileContainer *videoBoxFluorescence;
	OperationsController *opCtr;
	ExecuteSequence *sp;
	CreateClassifier *cc;
	QLabel *imageLabel;
	int currentFrame;
	ParameterLoader *parameterManager;
};

#endif // MAINWINDOW_H
