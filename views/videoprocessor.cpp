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
                                        tr("Avi files(*.avi)"));
	QByteArray ba = filename.toLocal8Bit();
	const char *c_str = ba.data();
    std::string filenameString(c_str);
    es->setAviFilename(filenameString);
    size_t index = 0;
    index = filenameString.find("avi", index);
    filenameString.replace(index, 3, "csv");
    es->setCsvFilename(filenameString);
    filenameString.replace(index, 3, "dot");
    es->setDotFilename(filenameString);
}

void VideoProcessor::on_goButton_clicked()
{
	ui->goButton->setEnabled(false);
    QFuture<void> future = QtConcurrent::run(es, &ExecuteSequence::run);
}
