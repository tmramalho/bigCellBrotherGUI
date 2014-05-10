#ifndef PICTURELABEL_H
#define PICTURELABEL_H

#include <QtWidgets/QLabel>
#include <QMouseEvent>
#include <iostream>

class PictureLabel : public QLabel
{
	Q_OBJECT

public:
	PictureLabel();
	double scaleFactor;

signals:
	void labelChanged(int i, int j, int bt);

protected:
	void mousePressEvent ( QMouseEvent * ev );

};

#endif // PICTURELABEL_H
