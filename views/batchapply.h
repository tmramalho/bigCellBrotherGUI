#ifndef BATCHAPPLY_H
#define BATCHAPPLY_H

#include "controllers/executesequence.h"
#include "batchprogress.h"
#include <iostream>
#include <QDialog>
#include <QDir>
#include <QTextStream>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

namespace Ui {
class BatchApply;
}

class BatchApply : public QDialog
{
    Q_OBJECT

public:
    explicit BatchApply(QWidget *parent = 0);
    ~BatchApply();

    void setDirectory(QDir directory) { b_dir = directory; }

    void setExecutor(ExecuteSequence *esP);

private slots:
    void on_patterns_textChanged(const QString &patterns);

    void on_buttonBox_accepted();

    void on_checkBox_clicked(bool checked);

private:
    Ui::BatchApply *ui;

    QDir b_dir;
    bool batch_ready;
    std::vector<QFileInfoList> files;
    BatchProgress *bp;
    ExecuteSequence *es;
};

#endif // BATCHAPPLY_H
