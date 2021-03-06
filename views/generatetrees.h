#ifndef GENERATETREES_H
#define GENERATETREES_H

#include <QDialog>
#include <QScrollBar>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include "picturelabel.h"
#include "controllers/manualtracker.h"

namespace Ui {
class GenerateTrees;
}

class GenerateTrees : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateTrees(ManualTracker *_mt, QWidget *parent = 0);
    ~GenerateTrees();
    void updatePicker(int max);
    void setCurrentFrame(int value);
    void setNextFrame(int value);

    void showEvent(QShowEvent *ev);

public slots:
    void updateParentFrame(QImage frame);
    void updateChildFrame(QImage frame);
    void finishExport();

private slots:
    void on_curSlider_valueChanged(int value);
    void on_nextSlider_valueChanged(int value);
    void syncHorizontalSlider(int value);
    void syncVerticalSlider(int value);

    void on_nextFrameButton_clicked();
    void on_prevFrameButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::GenerateTrees *ui;
    int curFrame;
    int nextFrame;
    int maxFrames;
    ManualTracker *mt;
    PictureLabel *curLabel;
    PictureLabel *nextLabel;
};

#endif // GENERATETREES_H
