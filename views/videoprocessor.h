#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QDialog>
#include <QFileDialog>
#include <QDir>
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
	
private slots:
	void on_pickDOT_clicked();

	void on_pickCSV_clicked();

	void on_goButton_clicked();

	void on_spinBox_valueChanged(int arg1);

	void on_pickAVI_clicked();

private:
	Ui::VideoProcessor *ui;
	ExecuteSequence *es;
};

#endif // VIDEOPROCESSOR_H
