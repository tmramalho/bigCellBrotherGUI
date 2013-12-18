#ifndef LISTOPENFILES_H
#define LISTOPENFILES_H

#include <QDialog>
#include <controllers/executesequence.h>

namespace Ui {
class ListOpenFiles;
}

class ListOpenFiles : public QDialog
{
    Q_OBJECT
    
public:
    explicit ListOpenFiles(QWidget *parent = 0);
    ~ListOpenFiles();

    void setOp(ExecuteSequence *_ex);

public slots:
    void addBrightfield(std::string name);
    void addFluorescence(std::string name);
    
private:
    Ui::ListOpenFiles *ui;
    ExecuteSequence *ex;
};

#endif // LISTOPENFILES_H
