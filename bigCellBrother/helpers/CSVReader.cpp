/*
 * CSVReader.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: tiago
 */

#include "CSVReader.h"

CSVReader::CSVReader() {

}

CSVReader::~CSVReader() {
}

std::vector<std::vector<double> > CSVReader::readValues(std::string filename) {
	std::ifstream dataFile(filename.c_str());
	std::string line;
	std::vector<std::vector<double> > result;
	while(std::getline(dataFile, line)) {
		std::stringstream linestream(line);
		std::string cell;
		std::vector<double> values;
		while(std::getline(linestream, cell, ',')) {
			values.push_back(atof(cell.c_str()));
		}
		result.push_back(values);
	}

	return result;
}


