#ifndef LOADIMAGE_H
#define LOADIMAGE_H

#include <QDialog>
#include <lib/files/filecontainer.h>

namespace Ui {
class LoadImage;
}

class LoadImage : public QDialog
{
	Q_OBJECT

public:
    explicit LoadImage(QWidget *parent = 0);
    ~LoadImage();
    void bindToFile(FileContainer *fi);
    void bindToOps();

private:
    Ui::LoadImage *ui;
    FileContainer *file;
};

#endif // LOADIMAGE_H
