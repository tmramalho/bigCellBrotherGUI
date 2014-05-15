#include "batchapply.h"
#include "ui_batchapply.h"

BatchApply::BatchApply(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchApply)
{
    ui->setupUi(this);
    batch_ready = 0;
    bp = new BatchProgress();
}

BatchApply::~BatchApply()
{
    delete ui;
}

void BatchApply::setExecutor(ExecuteSequence *esP)
{
    es = esP;
    QObject::connect(es, SIGNAL(incrementProgress(float, float)), bp, SLOT(incrementProgress(float, float)));
    QObject::connect(es, SIGNAL(sequenceDone()), bp, SLOT(exportDone()));
    QObject::connect(bp, SIGNAL(batchCancelled()), es, SLOT(cancelRun()));
}

void BatchApply::on_patterns_textChanged(const QString &patterns)
{
    //get list of files
    QStringList patternList = patterns.split(";");
    files.clear();
    for (int i = 0; i < patternList.size(); ++i) {
        QStringList singlePattern;
        singlePattern << patternList.at(i);
        QFileInfoList matchingFiles = b_dir.entryInfoList(singlePattern, QDir::Files, QDir::Name);
        files.push_back(matchingFiles);
    }

    //sanity check
    int numStacks = files.at(0).count();
    bool stacksMatch = 1;
    for (int j = 1; j < files.size(); ++j) {
        if(files.at(j).count() != numStacks) stacksMatch = 0;
    }

    //create description
    if (numStacks == 0) {
        ui->actionText->setHtml("<h1>Nothing found :(</h1>");
        batch_ready = 0;
    } else {
        QString description;
        QTextStream in(&description);
        in << "<h1>Found some files!</h1>";
        if(!stacksMatch) {
            in << "Number of stacks doesn't match.";
            in << "Maybe there is something wrong?<br/>";
        }
        for (int i = 0; i < numStacks; ++i) {
            in << "<h3>Stack " << i << "</h3>";
            for (int j = 0; j < files.size(); ++j) {
                if(j == 0) in << "<b>Brightfield:</b> ";
                else in << "<b>Fluorescence:</b> ";
                if(i < files.at(j).count()) in << files.at(j).at(i).fileName() << "<br/>";
                else in << "file missing! <br/>";
            }
        }
        ui->actionText->setHtml(description);
        batch_ready = 1;
    }
}

void BatchApply::on_buttonBox_accepted()
{
    bp->show();
    QFuture<void> future = QtConcurrent::run(es, &ExecuteSequence::batchRun, files);
}

void BatchApply::on_checkBox_clicked(bool checked)
{
    es->setSaveFrames(checked);
}
