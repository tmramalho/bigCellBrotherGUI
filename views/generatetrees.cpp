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
    QObject::connect(mt, SIGNAL(newChildFrame(QImage)), this, SLOT(updateChildFrame(QImage)));
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
    std::cout << "View changed prev frame" << std::endl;
    if(value >= nextFrame) {
        setNextFrame(value);
    }
    setCurrentFrame(value);
}

void GenerateTrees::on_nextSlider_valueChanged(int value)
{
    if (value == nextFrame) return;
    std::cout << "View changed next frame" << std::endl;
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
    mt->changeParentImage(value);
}

void GenerateTrees::setNextFrame(int value)
{
    mt->changeChildImage(value);
}

void GenerateTrees::showEvent(QShowEvent *ev)
{
    setCurrentFrame(0);
    if(maxFrames > 0) setNextFrame(1);
    else setNextFrame(0);
}

void GenerateTrees::updateParentFrame(QImage frame)
{
    curFrame = mt->getParentFrameNumber();
    ui->curSlider->setValue(curFrame);
    ui->curNumber->setValue(curFrame);
    curLabel->clear();
    curLabel->setPixmap(QPixmap::fromImage(frame));
    curLabel->setMinimumSize(frame.width(), frame.height());
}

void GenerateTrees::updateChildFrame(QImage frame)
{
    nextFrame = mt->getChildFrameNumber();
    ui->nextSlider->setValue(nextFrame);
    ui->nextNumber->setValue(nextFrame);
    nextLabel->clear();
    nextLabel->setPixmap(QPixmap::fromImage(frame));
    nextLabel->setMinimumSize(frame.width(), frame.height());
}

void GenerateTrees::on_nextFrameButton_clicked()
{
    if(curFrame < maxFrames && nextFrame < maxFrames) {
        mt->setChildAsParent();
    }
}

void GenerateTrees::on_prevFrameButton_clicked()
{
    if(curFrame > 0 && nextFrame > 0) {
        mt->setParentAsChild();
    }
}

void GenerateTrees::on_saveButton_clicked()
{

}
