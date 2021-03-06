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

    imageLabel = new PictureLabel();
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);
    ui->scrollArea->setWidget(imageLabel);

    //create qt objects
    ci = new CropImage();
    ii = new ImproveImage();
    th = new Threshold();
    cm = new CreateMarkers();
    ws = new Watershed();
    ls = new ListClassifier();
    li = new LoadImage();
    lo = new ListOpenFiles();

    //add qt widgets to stackedwidget
    ui->stackedWidget->addWidget(ci);
    ui->stackedWidget->addWidget(ii);
    ui->stackedWidget->addWidget(th);
    ui->stackedWidget->addWidget(cm);
    ui->stackedWidget->addWidget(ws);
    ui->stackedWidget->addWidget(ls);
    bIndex = ui->stackedWidget->addWidget(li);

    //create operation classes
    CropImageOp *cio = new CropImageOp(&opCtr);
    ImproveImageOp *iio = new ImproveImageOp(&opCtr);
    ThresholdOp *tho = new ThresholdOp(&opCtr);
    CreateMarkersOp *cmo = new CreateMarkersOp(&opCtr);
    WatershedOp *wso = new WatershedOp(&opCtr);
    cc = new CreateClassifier(&opCtr);
    sp = new ExecuteSequence(&opCtr);
    execution = new VideoProcessor();
    batch = new BatchApply();
    mt = new ManualTracker(sp, &opCtr);
    genTree = new GenerateTrees(mt);

    //bind operations to widgets
    li->bindToOps();
    ci->bindToOp(cio);
    ii->bindToOp(iio);
    th->bindToOp(tho);
    cm->bindToOp(cmo);
    ws->bindToOp(wso);
    ls->bindToOp(cc);
    execution->setExecutor(sp);
    batch->setExecutor(sp);
    lo->setOp(sp);

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

    //parameters signals
    QObject::connect(parameterManager, SIGNAL(parametersUpdated(std::map<std::string,double>)),
                     ci, SLOT(updateParameters(std::map<std::string,double>)));
    QObject::connect(parameterManager, SIGNAL(parametersUpdated(std::map<std::string,double>)),
                     ii, SLOT(updateParameters(std::map<std::string,double>)));
    QObject::connect(parameterManager, SIGNAL(parametersUpdated(std::map<std::string,double>)),
                     th, SLOT(updateParameters(std::map<std::string,double>)));
    QObject::connect(parameterManager, SIGNAL(parametersUpdated(std::map<std::string,double>)),
                     cm, SLOT(updateParameters(std::map<std::string,double>)));
    QObject::connect(parameterManager, SIGNAL(parametersUpdated(std::map<std::string,double>)),
                     ws, SLOT(updateParameters(std::map<std::string,double>)));
}

MainWindow::~MainWindow()
{
    if(videoBox != NULL) {
        videoBox->closeFile();
        delete videoBox;
    }
    delete ui;
    delete sp;
    delete cc;
    delete lo;
    delete execution;
    delete parameterManager;
}

void MainWindow::openImage() {
    QString fileName = QFileDialog::getOpenFileName(this,
									   tr("Open File"), QDir::homePath());

	if (!fileName.isEmpty()) {
        if(videoBox != NULL) {
            QObject::disconnect(videoBox, SIGNAL(paramsChanged()), this, SLOT(resetPipeline()));
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

        QObject::connect(videoBox, SIGNAL(paramsChanged()), this, SLOT(resetPipeline()));
        li->bindToFile(videoBox);
        ui->framePicker->setEnabled(true);
        ui->framePicker->setMinimum(0);
        ui->framePicker->setMaximum(videoBox->getNumFrames()-1);
        ui->framePicker->setTickPosition(QSlider::TicksBelow);
        ui->framePicker->setSingleStep(1);
        ui->framePicker->setTickInterval(videoBox->getNumFrames()/10);
        genTree->updatePicker(videoBox->getNumFrames()-1);
        updateFrameNumberDisplay();

        on_framePicker_valueChanged(0);
        opCtr.setupPipeline(videoBox->grabFrameNumber(0));
        sp->setFileSource(videoBox);
        mt->setFileSource(videoBox);
        scaleFactor = 1.0;

        if(videoBox->getIsHDR()) ui->stackedWidget->setCurrentIndex(bIndex);
        ui->actionApply->setEnabled(true);
        ui->actionBatch_apply->setEnabled(true);
        ui->zoomInAct->setEnabled(true);
        ui->zoomOutAct->setEnabled(true);
        ui->normalSizeAct->setEnabled(true);
        ui->fitToWindowAct->setEnabled(true);
        //ui->fitToWindowAct->setChecked(true);
        //this->fitToWindow();
        updateActions();

        batch->setDirectory(fi.absoluteDir());

        if (!ui->fitToWindowAct->isChecked())
            imageLabel->adjustSize();
	}
}

/***********
 *Open files
 **********/

void MainWindow::openImageFluorescent()
{
    FileContainer* videoBoxFluorescence;
	QString fileName = QFileDialog::getOpenFileName(this,
									   tr("Open File"), QDir::homePath());
	if (!fileName.isEmpty()) {
        QFileInfo fi(fileName);
		QString ext = fi.suffix();
        if(ext == "avi") {
            videoBoxFluorescence = new VideoContainer();
        } else if(ext == "tif" || ext == "tiff") {
            videoBoxFluorescence = new TiffContainer(false);
        } else {
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
        }

		sp->setFluorFileSource(videoBoxFluorescence);

		return;
	}
}

void MainWindow::loadParameters()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::homePath(),
                                    tr("PXM files(*.pxm)"));
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
    int op = opCtr.getStepOrder(curOperation);
    if(videoBox->getIsHDR() && op == 0) op = bIndex;
    ui->stackedWidget->setCurrentIndex(op);
    if(curOperation == "Classifier" && ui->fitToWindowAct->isChecked()) {
        opCtr.setOperationState(curOperation);
		ui->fitToWindowAct->trigger();
    }
    if(curOperation == "Results") {
        opCtr.setOperationState(curOperation);
		sp->debugSequence(currentFrame);
    } else
        opCtr.updateSelectedOperationPreview(curOperation);
}

void MainWindow::updateFrameNumberDisplay()
{
    ui->frameNumber->setText(tr("%1 / %2").arg(videoBox->getCurFrame()+1).arg(videoBox->getNumFrames()));
}

void MainWindow::displayExecutionDialog()
{
    execution->exec();
}

void MainWindow::displayOpenFiles()
{
    lo->show();
}

void MainWindow::displayBatch()
{
    batch->exec();
}

void MainWindow::sequenceProcessingFinished()
{
	int nItems = ui->listWidget->count();
	QListWidgetItem *qli = ui->listWidget->item(nItems-1);
    qli->setHidden(false);
    ui->actionTracking->setEnabled(true);
}

void MainWindow::displayTrackingEditor()
{
    genTree->exec();
}

void MainWindow::on_framePicker_valueChanged(int value)
{
	ui->listWidget->setCurrentRow(0);
    int op = 0;
    if(videoBox->getIsHDR()) op = bIndex;
    ui->stackedWidget->setCurrentIndex(op);
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

void MainWindow::resetPipeline() {
    int value = ui->framePicker->value();
    on_framePicker_valueChanged(value);
}

void MainWindow::openSequence()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                       tr("Open sequence file"), QDir::homePath(),
                                        tr("SLR files(*.slr)"));
    QByteArray ba = filename.toLocal8Bit();
    const char *c_str = ba.data();
    const std::string filenameString(c_str);
    sp->importLab(filenameString);
    ui->actionTracking->setEnabled(true);
}
