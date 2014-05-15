#include "picturelabel.h"

PictureLabel::PictureLabel()
{
	scaleFactor = 1;
}

void PictureLabel::mousePressEvent(QMouseEvent *ev)
{
    std::cout << "Press!" << std::endl;
	QPoint pos = ev->pos();
	Qt::MouseButton bt = ev->button();
	//x and y are exchanged because of opencv matrix
    int i = pos.y()/scaleFactor;
    int j = pos.x()/scaleFactor;
    emit labelChanged(i, j, (int)bt);
    ev->accept();
}

void PictureLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    ev->accept();
}
