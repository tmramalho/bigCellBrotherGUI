#include "batchprogress.h"
#include "ui_batchprogress.h"

BatchProgress::BatchProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchProgress)
{
    ui->setupUi(this);
}

BatchProgress::~BatchProgress()
{
    delete ui;
}

void BatchProgress::incrementProgress(float c, float e)
{
    ui->progressBar->setValue((int) e);
    QString message;
    message.sprintf("Roughly %.0f seconds remain...", c);
    ui->timeRemaining->setText(message);
}

void BatchProgress::exportDone()
{
    this->done(1);
}

void BatchProgress::on_pushButton_clicked()
{
    emit batchCancelled();
    this->done(0);
}
