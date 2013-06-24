#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
    parameterManager = new ParameterLoader;
    opCtr.setPM(parameterManager);
    QObject::connect(parameterManager, SIGNAL(parametersRead()), &opCtr, SLOT(parametersUpdated()));

	videoBox = NULL;
	videoBoxFluorescence = NULL;

    imageLabel = new PictureLabel();
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);
    ui->scrollArea->setWidget(imageLabel);

    //create qt objects
    QListWidgetItem *qlw;
    CropImage *ci = new CropImage();
    ImproveImage *ii = new ImproveImage();
    Threshold *th = new Threshold();
    CreateMarkers *cm = new CreateMarkers();
    Watershed *ws = new Watershed();
    ListClassifier *ls = new ListClassifier();

    //add qt widgets to stackedwidget
    ui->stackedWidget->addWidget(ci);
    ui->stackedWidget->addWidget(ii);
    ui->stackedWidget->addWidget(th);
    ui->stackedWidget->addWidget(cm);
    ui->stackedWidget->addWidget(ws);
    ui->stackedWidget->addWidget(ls);

    //create operation classes
    CropImageOp *cio = new CropImageOp(&opCtr);
    ImproveImageOp *iio = new ImproveImageOp(&opCtr);
    ThresholdOp *tho = new ThresholdOp(&opCtr);
    CreateMarkersOp *cmo = new CreateMarkersOp(&opCtr);
    WatershedOp *wso = new WatershedOp(&opCtr);
    cc = new CreateClassifier(&opCtr);
    sp = new ExecuteSequence(&opCtr);
    execution = new VideoProcessor();

    //bind operations to widgets
    ci->bindToOp(cio);
    ii->bindToOp(iio);
    th->bindToOp(tho);
    cm->bindToOp(cmo);
    ws->bindToOp(wso);
    ls->bindToOp(cc);
    execution->setExecutor(sp);

    //define operation names
    std::string liStr( "Load Image" );
    std::string crStr( "Crop Image" );
    std::string iiStr( "Improve Image" );
    std::string thStr( "Threshold" );
    std::string cmStr( "Create Markers" );
    std::string wsStr( "Watershed" );
    std::string lsStr( "Classifier" );
    std::string rsStr( "Results" );

    //addoperations to opController
    opCtr.addOperation(crStr, cio);
    opCtr.addOperation(iiStr, iio);
    opCtr.addOperation(thStr, tho);
    opCtr.addOperation(cmStr, cmo);
    opCtr.addOperation(wsStr, wso);
    opCtr.addOperation(lsStr, cc);

    //create items for the operation list
    qlw = new QListWidgetItem(tr("Load Image"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(liStr.c_str())));

	qlw = new QListWidgetItem(tr("Crop Image"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(crStr.c_str())));

	qlw = new QListWidgetItem(tr("Improve Image"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(iiStr.c_str())));

	qlw = new QListWidgetItem(tr("Threshold"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(thStr.c_str())));

	qlw = new QListWidgetItem(tr("Create Markers"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(cmStr.c_str())));

	qlw = new QListWidgetItem(tr("Watershed"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(wsStr.c_str())));

	qlw = new QListWidgetItem(tr("Classifier"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(lsStr.c_str())));

	qlw = new QListWidgetItem(tr("Results"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(QString(rsStr.c_str())));
	qlw->setHidden(true);

    //signals and slots
    QObject::connect(&opCtr, SIGNAL(operationDone(QImage)), this, SLOT(updatePreview(QImage)));
    QObject::connect(imageLabel, SIGNAL(labelChanged(int, int, int)), cc, SLOT(cellPicked(int, int, int)));
    QObject::connect(this, SIGNAL(currentFrameChanged(int)), cc, SLOT(frameChanged(int)));
    QObject::connect(&opCtr, SIGNAL(newBounds(int,int,int,int)), ci, SLOT(updateBounds(int,int,int,int)));
    QObject::connect(sp, SIGNAL(sequenceDone()), this, SLOT(sequenceProcessingFinished()));
}

MainWindow::~MainWindow()
{
    if(videoBox != NULL) {
        videoBox->closeFile();
        delete videoBox;
    }
    if(videoBoxFluorescence != NULL) {
        videoBoxFluorescence->closeFile();
        delete videoBoxFluorescence;
    }
    delete ui;
    delete sp;
    delete cc;
    delete parameterManager;
}

void MainWindow::openImage() {
	QString fileName = QFileDialog::getOpenFileName(this,
									   tr("Open File"), QDir::homePath());
	if (!fileName.isEmpty()) {
        if(videoBox != NULL) {
            videoBox->closeFile();
            delete videoBox;
        }
		QFileInfo fi(fileName);
		QString ext = fi.suffix();
		if(ext == "avi")
            videoBox = new VideoContainer();
		else if(ext == "tif" || ext == "tiff")
            videoBox = new TiffContainer();
		else {
            QMessageBox::information(this, tr("Big Cell Brother"),
									 tr("%1 is not a supported format.").arg(fileName));
			return;
		}

		QByteArray ba = fileName.toLocal8Bit();
		const char *c_str = ba.data();
		const std::string filenameString(c_str);
        videoBox->openFile(filenameString);

        if (!videoBox->isLoaded()) {
            QMessageBox::information(this, tr("Big Cell Brother"),
									 tr("Cannot load %1.").arg(fileName));
		return;
	}

	ui->framePicker->setEnabled(true);
	ui->framePicker->setMinimum(0);
    ui->framePicker->setMaximum(videoBox->getNumFrames()-1);
	ui->framePicker->setTickPosition(QSlider::TicksBelow);
	ui->framePicker->setSingleStep(1);
    ui->framePicker->setTickInterval(videoBox->getNumFrames()/10);
	updateFrameNumberDisplay();

	on_framePicker_valueChanged(0);
    opCtr.setupPipeline(videoBox->grabFrameNumber(0));
	sp->setFileSource(videoBox);
	scaleFactor = 1.0;

	ui->actionApply->setEnabled(true);
	ui->zoomInAct->setEnabled(true);
	ui->zoomOutAct->setEnabled(true);
	ui->normalSizeAct->setEnabled(true);
	ui->fitToWindowAct->setEnabled(true);
	//ui->fitToWindowAct->setChecked(true);
	//this->fitToWindow();
	updateActions();

	if (!ui->fitToWindowAct->isChecked())
		imageLabel->adjustSize();
	}
}

/***********
 *Open files
 **********/

void MainWindow::openImageFluorescent()
{
	QString fileName = QFileDialog::getOpenFileName(this,
									   tr("Open File"), QDir::homePath());
	if (!fileName.isEmpty()) {
        if(videoBoxFluorescence != NULL) {
            videoBoxFluorescence->closeFile();
            delete videoBoxFluorescence;
        }
        QFileInfo fi(fileName);
		QString ext = fi.suffix();
		if(ext == "avi")
            videoBoxFluorescence = new VideoContainer();
		else if(ext == "tif" || ext == "tiff")
            videoBoxFluorescence = new TiffContainer();
		else {
            QMessageBox::information(this, tr("Big Cell Brother"),
									 tr("%1 is not a supported format.").arg(fileName));
			return;
		}

		QByteArray ba = fileName.toLocal8Bit();
		const char *c_str = ba.data();
		const std::string filenameString(c_str);
        videoBoxFluorescence->openFile(filenameString);

        if (!videoBoxFluorescence->isLoaded()) {
            QMessageBox::information(this, tr("Big Cell Brother"),
									 tr("Cannot load %1.").arg(fileName));
        } else {
            QMessageBox::information(this, tr("Big Cell Brother"),
                                     tr("Successfully loaded %1. The fluorescence values will show up upon export").arg(fileName));
        }

		sp->setFluorFileSource(videoBoxFluorescence);

		return;
	}
}

void MainWindow::loadParameters()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                       tr("Open File"), QDir::homePath());
    if (!fileName.isEmpty()) {
        QByteArray ba = fileName.toLocal8Bit();
        const char *c_str = ba.data();
        const std::string filenameString(c_str);
        parameterManager->readParametersFromFile(filenameString);
    }
}

void MainWindow::saveParameters()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                       tr("Save File as.."), QDir::homePath(),
                                        tr("PXM files(*.pxm)"));
    QByteArray ba = filename.toLocal8Bit();
    const char *c_str = ba.data();
    const std::string filenameString(c_str);
    parameterManager->saveParametersToFile(filenameString);
}

/*********************************
 * methods related to image viewer
 *********************************/

void MainWindow::zoomIn()
 {
	 scaleImage(1.25);
 }

 void MainWindow::zoomOut()
 {
	 scaleImage(0.8);
 }

 void MainWindow::normalSize()
 {
	 imageLabel->adjustSize();
	 scaleFactor = 1.0;
	 PictureLabel *il = static_cast<PictureLabel *>(imageLabel);
	 il->scaleFactor = scaleFactor;
 }

 void MainWindow::fitToWindow()
 {
	 bool fitToWindow = ui->fitToWindowAct->isChecked();
	 ui->scrollArea->setWidgetResizable(fitToWindow);
	 if (!fitToWindow) {
		 normalSize();
	 }
	 updateActions();
 }

void MainWindow::updateActions()
{
	ui->zoomInAct->setEnabled(!ui->fitToWindowAct->isChecked());
	ui->zoomOutAct->setEnabled(!ui->fitToWindowAct->isChecked());
	ui->normalSizeAct->setEnabled(!ui->fitToWindowAct->isChecked());
}

void MainWindow::scaleImage(double factor)
{
	Q_ASSERT(imageLabel->pixmap());
	scaleFactor *= factor;
	PictureLabel *il = static_cast<PictureLabel *>(imageLabel);
	il->scaleFactor = scaleFactor;
	imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

	adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);

	ui->zoomInAct->setEnabled(scaleFactor < 3.0);
	ui->zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value()
					 + ((factor - 1) * scrollBar->pageStep()/2)));
}

/******************************************************************
 * methods related to changing an operation in the pipeline
******************************************************************/

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	//change pixmap on preview
	QString qoperation = current->data(Qt::UserRole).toString();
	QByteArray ba = qoperation.toLocal8Bit();
	const char *c_str = ba.data();
	std::string curOperation(c_str);
    //std::cout << curOperation << std::endl;
    ui->stackedWidget->setCurrentIndex(opCtr.getStepOrder(curOperation));
	if(curOperation == "Classifier" && ui->fitToWindowAct->isChecked())
		ui->fitToWindowAct->trigger();
	if(curOperation == "Results")
		sp->debugSequence(currentFrame);
    else
        opCtr.updateSelectedOperationPreview(curOperation);
}

void MainWindow::updateFrameNumberDisplay()
{
    ui->frameNumber->setText(tr("%1 / %2").arg(videoBox->getCurFrame()+1).arg(videoBox->getNumFrames()));
}

void MainWindow::displayExecutionDialog()
{
    execution->show();
}

void MainWindow::sequenceProcessingFinished()
{
	int nItems = ui->listWidget->count();
	QListWidgetItem *qli = ui->listWidget->item(nItems-1);
	qli->setHidden(false);
}

void MainWindow::on_framePicker_valueChanged(int value)
{
	ui->listWidget->setCurrentRow(0);
	ui->stackedWidget->setCurrentIndex(0);
    opCtr.resetPipeline(videoBox->grabFrameNumber(value));
    opCtr.updateSelectedOperationPreview("Load Image");
	currentFrame = value;
	updateFrameNumberDisplay();
	emit currentFrameChanged(value);
}

void MainWindow::updatePreview(QImage imagePreview)
{
	imageLabel->setScaledContents(false);
	imageLabel->clear();
	imageLabel->setPixmap(QPixmap::fromImage(imagePreview));
	imageLabel->setScaledContents(true);
	imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());
}
