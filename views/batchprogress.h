#ifndef BATCHPROGRESS_H
#define BATCHPROGRESS_H

#include <iostream>
#include <QDialog>

namespace Ui {
class BatchProgress;
}

class BatchProgress : public QDialog
{
    Q_OBJECT

public:
    explicit BatchProgress(QWidget *parent = 0);
    ~BatchProgress();

public slots:
    void incrementProgress(float c, float e);
    void exportDone();

private slots:
    void on_pushButton_clicked();

signals:
    void batchCancelled();

private:
    Ui::BatchProgress *ui;
};

#endif // BATCHPROGRESS_H
