#include "parameterloader.h"

ParameterLoader::ParameterLoader()
{
	//defaults
	parameters["ccw"] = 10;
	parameters["cch"] = 80;
	parameters["xb"]  = 0;
	parameters["xe"]  = 100;
	parameters["yb"]  = 0;
	parameters["ye"]  = 100;
    parameters["dns"] = 3;
	parameters["iibw"]  = 0;
	parameters["iism"]  = 0;
	parameters["iidr"]  = 0;
	parameters["tto"]  = 0;
    parameters["ttw"]  = 27;
    parameters["ttt"]  = 255;
	parameters["ttb"]  = 0;
	parameters["wsz"]  = 3;
	parameters["wsm"]  = 1;
    parameters["wsst"]  = 3;
	parameters["wsrm"]  = 0;
    parameters["ang"] = 0;
}

void ParameterLoader::readParametersFromFile(std::string filename)
{
	std::ifstream dataFile(filename.c_str());
	std::string line;
	while(std::getline(dataFile, line)) {
		std::stringstream linestream(line);
		std::string name;
		std::getline(linestream, name, ',');
		std::string val;
		std::getline(linestream, val, ',');
		parameters[name] = atof(val.c_str());
	}

	for (std::map<std::string, double>::iterator it=parameters.begin();
		 it!=parameters.end(); ++it) {
        std::cout << it->first << ", " << it->second << std::endl;
	}

	emit parametersRead();
    emit parametersUpdated(parameters);
}

void ParameterLoader::saveParametersToFile(std::string filename)
{
	std::fstream filestr(filename.c_str(), std::fstream::trunc | std::fstream::out);
	for (std::map<std::string, double>::iterator it=parameters.begin();
		 it!=parameters.end(); ++it) {
		filestr << it->first << ", " << it->second << std::endl;
	}
	filestr.close();
}
