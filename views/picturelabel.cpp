#include "picturelabel.h"

PictureLabel::PictureLabel()
{
	scaleFactor = 1;
}

void PictureLabel::mousePressEvent(QMouseEvent *ev)
{
	QPoint pos = ev->pos();
	Qt::MouseButton bt = ev->button();
	//x and y are exchanged because of opencv matrix
	emit labelChanged(pos.y()/scaleFactor, pos.x()/scaleFactor, (int)bt);
}
