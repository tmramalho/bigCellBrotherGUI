#ifndef PARAMETERLOADER_H
#define PARAMETERLOADER_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include <QObject>

class ParameterLoader: public QObject
{
	Q_OBJECT
public:
	ParameterLoader();

	void readParametersFromFile(std::string filename);
	void saveParametersToFile(std::string filename);
	double getNamedParameter(std::string name) { return parameters.at(name); }
	void setNamedParameter(std::string name, double par) { parameters[name] = par; }

signals:
	void parametersRead();

private:
	std::map <std::string, double> parameters;
};

#endif // PARAMETERLOADER_H
