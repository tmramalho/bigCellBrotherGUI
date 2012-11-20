#include "picturelabel.h"

PictureLabel::PictureLabel()
{
	scaleFactor = 1;
}

void PictureLabel::mousePressEvent(QMouseEvent *ev)
{
	QPoint pos = ev->pos();//this->mapFromParent(ev->pos());
	std::cout << pos.x()/scaleFactor << " " << pos.y()/scaleFactor << std::endl;
}
