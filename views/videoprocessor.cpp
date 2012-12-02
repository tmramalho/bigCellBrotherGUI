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
	QObject::connect(es, SIGNAL(incrementProgress(int)), this, SLOT(incrementProgress(int)));
}

VideoProcessor::~VideoProcessor()
{
	delete ui;
}

void VideoProcessor::incrementProgress(int c)
{
	ui->progressBar->setValue(c);
}

void VideoProcessor::on_pickDOT_clicked()
{

	QString filename = QFileDialog::getSaveFileName(this,
									   tr("Save File as.."), QDir::homePath(),
										tr("Dot files(*.dot)"));
	QByteArray ba = filename.toLocal8Bit();
	const char *c_str = ba.data();
	const std::string filenameString(c_str);
	ui->dotFileLabel->setText(tr("DOT file %1").arg(filename));
	es->setDotFilename(filenameString);
}

void VideoProcessor::on_pickCSV_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this,
									   tr("Save File as.."), QDir::homePath(),
										tr("CSV files(*.csv)"));
	QByteArray ba = filename.toLocal8Bit();
	const char *c_str = ba.data();
	const std::string filenameString(c_str);
	ui->dotFileLabel->setText(tr("CSV file %1").arg(filename));
	es->setCsvFilename(filenameString);
}

void VideoProcessor::on_goButton_clicked()
{
	ui->goButton->setEnabled(false);
	es->run();
}

void VideoProcessor::on_spinBox_valueChanged(int arg1)
{
	es->setDistanceCutoff(arg1);
}

void VideoProcessor::on_pickAVI_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this,
									   tr("Save File as.."), QDir::homePath(),
										tr("AVI files(*.avi)"));
	QByteArray ba = filename.toLocal8Bit();
	const char *c_str = ba.data();
	const std::string filenameString(c_str);
	ui->dotFileLabel->setText(tr("Video file %1").arg(filename));
	es->setAviFilename(filenameString);
}
