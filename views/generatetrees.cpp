#include "generatetrees.h"
#include "ui_generatetrees.h"

GenerateTrees::GenerateTrees(ManualTracker *_mt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenerateTrees)
{
    ui->setupUi(this);
    curFrame = 0;
    nextFrame = 0;
    mt = _mt;

    curLabel = new PictureLabel();
    curLabel->setBackgroundRole(QPalette::Dark);
    curLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->curScroll->setWidget(curLabel);

    nextLabel = new PictureLabel();
    curLabel->setBackgroundRole(QPalette::Dark);
    nextLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->nextScroll->setWidget(nextLabel);

    QObject::connect(ui->curNumber, SIGNAL(valueChanged(int)), this, SLOT(on_curSlider_valueChanged(int)));
    QObject::connect(ui->nextNumber, SIGNAL(valueChanged(int)), this, SLOT(on_nextSlider_valueChanged(int)));
    QObject::connect(curLabel, SIGNAL(labelChanged(int, int, int)), mt, SLOT(curCellPicked(int, int, int)));
    QObject::connect(nextLabel, SIGNAL(labelChanged(int, int, int)), mt, SLOT(nextCellPicked(int, int, int)));
    QObject::connect(ui->curScroll->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(syncVerticalSlider(int)));
    QObject::connect(ui->nextScroll->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(syncVerticalSlider(int)));
    QObject::connect(ui->curScroll->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(syncHorizontalSlider(int)));
    QObject::connect(ui->nextScroll->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(syncHorizontalSlider(int)));
    QObject::connect(mt, SIGNAL(newParentFrame(QImage)), this, SLOT(updateParentFrame(QImage)));
}

GenerateTrees::~GenerateTrees()
{
    delete ui;
}

void GenerateTrees::updatePicker(int max)
{
    ui->curSlider->setMaximum(max);
    ui->nextSlider->setMaximum(max);
    ui->curNumber->setMaximum(max);
    ui->nextNumber->setMaximum(max);
    maxFrames = max;
}

void GenerateTrees::on_curSlider_valueChanged(int value)
{
    if (value == curFrame) return;
    if(value >= nextFrame) {
        setNextFrame(value);
    }
    setCurrentFrame(value);
}

void GenerateTrees::on_nextSlider_valueChanged(int value)
{
    if (value == nextFrame) return;
    if (value < curFrame) {
        setCurrentFrame(value);
    }
    setNextFrame(value);
}

void GenerateTrees::syncHorizontalSlider(int value)
{
    ui->curScroll->horizontalScrollBar()->setValue(value);
    ui->nextScroll->horizontalScrollBar()->setValue(value);
}

void GenerateTrees::syncVerticalSlider(int value)
{
    ui->curScroll->verticalScrollBar()->setValue(value);
    ui->nextScroll->verticalScrollBar()->setValue(value);
}

void GenerateTrees::setCurrentFrame(int value)
{
    curFrame = value;
    ui->curSlider->setValue(curFrame);
    ui->curNumber->setValue(curFrame);
    QImage frame = mt->changeParentImage(value);
    curLabel->clear();
    curLabel->setPixmap(QPixmap::fromImage(frame));
    curLabel->setMinimumSize(frame.width(), frame.height());
}

void GenerateTrees::setNextFrame(int value)
{
    nextFrame = value;
    ui->nextSlider->setValue(nextFrame);
    ui->nextNumber->setValue(nextFrame);
    QImage frame = mt->changeChildImage(value);
    nextLabel->clear();
    nextLabel->setPixmap(QPixmap::fromImage(frame));
    nextLabel->setMinimumSize(frame.width(), frame.height());
}

void GenerateTrees::updateParentFrame(QImage frame)
{
    curLabel->clear();
    curLabel->setPixmap(QPixmap::fromImage(frame));
    curLabel->setMinimumSize(frame.width(), frame.height());
}

void GenerateTrees::on_nextFrameButton_clicked()
{
    if(curFrame < maxFrames && nextFrame < maxFrames) {
        setCurrentFrame(nextFrame);
        setNextFrame(nextFrame+1);
    }
}

void GenerateTrees::on_pushButton_clicked()
{
    if(curFrame > 0 && nextFrame > 0) {
        setCurrentFrame(curFrame-1);
        setNextFrame(nextFrame-1);
    }
}
