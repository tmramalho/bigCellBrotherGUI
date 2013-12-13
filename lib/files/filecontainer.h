#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#include <QObject>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class FileContainer : public QObject
{
    Q_OBJECT

public:
	FileContainer();
	virtual void openFile(std::string filename)=0;
	virtual void closeFile()=0;
	virtual cv::Mat grabFrameNumber(int frameNum)=0;
    virtual ~FileContainer()=0;

	bool isLoaded() { return loaded; }
	int getNumFrames() { return numFrames; }
	int getCurFrame() { return curFrame; }
	cv::Size getFrameSize() { return frameSize; }


    int getVMin() const;
    void setVMin(const int &value);

    int getVMax() const;
    void setVMax(const int &value);

    bool getIsHDR() const;

signals:
    void paramsChanged();

public slots:
    void changeVMin(int vm);
    void changeVMax(int vm);

protected:
	bool loaded;
    bool isHDR;
	int numFrames;
	int curFrame;
    int vMin, vMax;
	cv::Size frameSize;
};

#endif // FILECONTAINER_H
