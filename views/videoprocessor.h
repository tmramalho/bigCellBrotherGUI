#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>
#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "executesequence.h"

namespace Ui {
class VideoProcessor;
}

class VideoProcessor : public QDialog
{
	Q_OBJECT

public:
	explicit VideoProcessor(QWidget *parent = 0);
	void setExecutor(ExecuteSequence *esP);
	~VideoProcessor();

public slots:
	void incrementProgress(int c);
    void exportDone();

private slots:
	void on_pickDOT_clicked();

	void on_goButton_clicked();

private:
	Ui::VideoProcessor *ui;
	ExecuteSequence *es;
};

#endif // VIDEOPROCESSOR_H
