#include "videoprocessor.h"
#include "ui_videoprocessor.h"

VideoProcessor::VideoProcessor(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VideoProcessor)
{
	ui->setupUi(this);
}

void VideoProcessor::setExecutor(ExecuteSequence *esP)
{
	es = esP;
    QObject::connect(es, SIGNAL(incrementProgress(float, float)), this, SLOT(incrementProgress(float, float)));
    QObject::connect(es, SIGNAL(sequenceDone()), this, SLOT(exportDone()));
    QObject::connect(this, SIGNAL(batchCancelled()), es, SLOT(cancelRun()));
}

VideoProcessor::~VideoProcessor()
{
	delete ui;
}

void VideoProcessor::incrementProgress(float el, float est)
{
    ui->progressBar->setValue((int)est);
}

void VideoProcessor::exportDone()
{
    ui->pickDOT->setEnabled(true);
    ui->checkBox->setEnabled(true);
    this->done(1);
}

void VideoProcessor::on_pickDOT_clicked()
{

	QString filename = QFileDialog::getSaveFileName(this,
                                       tr("Save File as.."), QDir::homePath(),
                                        tr("Avi files(*.avi)"));
	QByteArray ba = filename.toLocal8Bit();
	const char *c_str = ba.data();
    std::string filenameString(c_str);
    es->setAviFilename(filenameString);
    size_t index = 0;
    index = filenameString.find("avi", index);
    filenameString.replace(index, 3, "dot");
    es->setDotFilename(filenameString);
    filenameString.replace(index, 3, "csv");
    es->setCsvFilename(filenameString);
    ui->goButton->setEnabled(true);
    QString fname(filenameString.c_str());
    ui->filename->setText(fname);
    ui->filename->setWordWrap(true);
}

void VideoProcessor::on_goButton_clicked()
{
	ui->goButton->setEnabled(false);
    ui->pickDOT->setEnabled(false);
    ui->checkBox->setEnabled(false);
    ui->pushButton->setEnabled(true);
    QFuture<void> future = QtConcurrent::run(es, &ExecuteSequence::run);
}

void VideoProcessor::on_pushButton_clicked()
{
    ui->pickDOT->setEnabled(true);
    ui->checkBox->setEnabled(true);
    emit batchCancelled();
    this->done(0);
}

void VideoProcessor::on_checkBox_clicked(bool checked)
{
    es->setSaveFrames(checked);
}
