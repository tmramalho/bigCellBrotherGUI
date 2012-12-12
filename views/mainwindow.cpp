#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	opCtr = new OperationsController;

	videoBox = NULL;
	videoBoxFluorescence = NULL;

	imageLabel = new PictureLabel();
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);
	ui->scrollArea->setWidget(imageLabel);

	//set up operations pipeline
	QObject::connect(opCtr, SIGNAL(operationDone(QImage)), this, SLOT(updatePreview(QImage)));
	int n = 0;
	QListWidgetItem *qlw;
	qlw = new QListWidgetItem(tr("Load Image"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(-1));

	qlw = new QListWidgetItem(tr("Crop Image"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));

	CropImage *ci = new CropImage();
	ui->stackedWidget->addWidget(ci);
	QObject::connect(opCtr, SIGNAL(newBounds(int,int,int,int)), ci, SLOT(updateBounds(int,int,int,int)));

	CropImageOp *cio = new CropImageOp();
	cio->controller = opCtr;
	ci->bindToOp(cio);
	opCtr->operationPipeline.push_back(cio);

	qlw = new QListWidgetItem(tr("Improve Image"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));

	ImproveImage *ii = new ImproveImage();
	ui->stackedWidget->addWidget(ii);

	ImproveImageOp *iio = new ImproveImageOp();
	iio->controller = opCtr;
	ii->bindToOp(iio);
	opCtr->operationPipeline.push_back(iio);

	qlw = new QListWidgetItem(tr("Threshold"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));

	Threshold *th = new Threshold();
	ui->stackedWidget->addWidget(th);

	ThresholdOp *tho = new ThresholdOp();
	tho->controller = opCtr;
	th->bindToOp(tho);
	opCtr->operationPipeline.push_back(tho);

	qlw = new QListWidgetItem(tr("Create Markers"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));

	CreateMarkers *cm = new CreateMarkers();
	ui->stackedWidget->addWidget(cm);

	CreateMarkersOp *cmo = new CreateMarkersOp();
	cmo->controller = opCtr;
	cm->bindToOp(cmo);
	opCtr->operationPipeline.push_back(cmo);

	qlw = new QListWidgetItem(tr("Watershed"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));

	Watershed *ws = new Watershed();
	ui->stackedWidget->addWidget(ws);

	WatershedOp *wso = new WatershedOp();
	wso->controller = opCtr;
	ws->bindToOp(wso);
	opCtr->operationPipeline.push_back(wso);

	qlw = new QListWidgetItem(tr("Classifier"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));

	ListClassifier *ls = new ListClassifier();
	ui->stackedWidget->addWidget(ls);

	cc = new CreateClassifier;
	cc->controller = opCtr;
	QObject::connect(imageLabel, SIGNAL(labelChanged(int, int, int)), cc, SLOT(cellPicked(int, int, int)));
	QObject::connect(this, SIGNAL(currentFrameChanged(int)), cc, SLOT(frameChanged(int)));
	ls->bindToOp(cc);
	opCtr->operationPipeline.push_back(cc);

	qlw = new QListWidgetItem(tr("Results"), ui->listWidget);
	qlw->setData(Qt::UserRole, QVariant(n++));
	qlw->setHidden(true);

	sp = new ExecuteSequence(opCtr);
	QObject::connect(sp, SIGNAL(sequenceDone()), this, SLOT(sequenceProcessingFinished()));
}

MainWindow::~MainWindow()
{
	delete opCtr;
	delete videoBox;
	delete ui;
}

void MainWindow::openImage() {
	QString fileName = QFileDialog::getOpenFileName(this,
									   tr("Open File"), QDir::homePath());
	if (!fileName.isEmpty()) {
		if(videoBox != NULL) delete videoBox;
		QFileInfo fi(fileName);
		QString ext = fi.suffix();
		if(ext == "avi")
			videoBox = new VideoContainer();
		else if(ext == "tif" || ext == "tiff")
			videoBox = new TiffContainer();
		else {
			QMessageBox::information(this, tr("Image Viewer"),
									 tr("%1 is not a supported format.").arg(fileName));
			return;
		}

		QByteArray ba = fileName.toLocal8Bit();
		const char *c_str = ba.data();
		const std::string filenameString(c_str);
		videoBox->openFile(filenameString);

		if (!videoBox->isLoaded()) {
			QMessageBox::information(this, tr("Image Viewer"),
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
	opCtr->setupPipeline(videoBox->grabFrameNumber(0));
	sp->setFileSource(videoBox);
	scaleFactor = 1.0;

	ui->actionApply->setEnabled(true);
	ui->zoomInAct->setEnabled(true);
	ui->zoomOutAct->setEnabled(true);
	ui->normalSizeAct->setEnabled(true);
	ui->fitToWindowAct->setEnabled(true);
	ui->fitToWindowAct->setChecked(true);
	this->fitToWindow();
	updateActions();

	if (!ui->fitToWindowAct->isChecked())
		imageLabel->adjustSize();
	}
}

void MainWindow::openImageFluorescent()
{
	QString fileName = QFileDialog::getOpenFileName(this,
									   tr("Open File"), QDir::homePath());
	if (!fileName.isEmpty()) {
		if(videoBoxFluorescence != NULL) delete videoBoxFluorescence;
		QFileInfo fi(fileName);
		QString ext = fi.suffix();
		if(ext == "avi")
			videoBoxFluorescence = new VideoContainer();
		else if(ext == "tif" || ext == "tiff")
			videoBoxFluorescence = new TiffContainer();
		else {
			QMessageBox::information(this, tr("Image Viewer"),
									 tr("%1 is not a supported format.").arg(fileName));
			return;
		}

		QByteArray ba = fileName.toLocal8Bit();
		const char *c_str = ba.data();
		const std::string filenameString(c_str);
		videoBoxFluorescence->openFile(filenameString);

		if (!videoBoxFluorescence->isLoaded()) {
			QMessageBox::information(this, tr("Image Viewer"),
									 tr("Cannot load %1.").arg(fileName));
		}

		sp->setFluorFileSource(videoBoxFluorescence);

		return;
	}
}

/*methods related to image viewer*/

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

/* methods related to changing an operation in the pipeline */

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	//change pixmap on preview
	int curOperation = current->data(Qt::UserRole).toInt();
	ui->stackedWidget->setCurrentIndex(curOperation+1);
	opCtr->updateSelectedOperationPreview(curOperation);
	if(curOperation == 6)
		sp->debugSequence(currentFrame);
}

void MainWindow::updateFrameNumberDisplay()
{
	ui->frameNumber->setText(tr("%1 / %2").arg(videoBox->getCurFrame()+1).arg(videoBox->getNumFrames()));
}

void MainWindow::displayExecutionDialog()
{
	VideoProcessor *execution = new VideoProcessor();
	execution->setExecutor(sp);
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
	opCtr->resetPipeline(videoBox->grabFrameNumber(value));
	opCtr->updateSelectedOperationPreview(-1);
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
