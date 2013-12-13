#include "filecontainer.h"

FileContainer::FileContainer()
{
    isHDR = false;
    loaded = false;
}


FileContainer::~FileContainer()
{
}
void FileContainer::changeVMin(int vm)
{
    vMin = vm;
    emit paramsChanged();
}

void FileContainer::changeVMax(int vm)
{
    vMax = vm;
    emit paramsChanged();
}

int FileContainer::getVMax() const
{
    return vMax;
}

void FileContainer::setVMax(const int &value)
{
    vMax = value;
}

int FileContainer::getVMin() const
{
    return vMin;
}

void FileContainer::setVMin(const int &value)
{
    vMin = value;
}

bool FileContainer::getIsHDR() const
{
    return isHDR;
}

