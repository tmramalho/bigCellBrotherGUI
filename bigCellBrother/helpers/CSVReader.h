/*
 * CSVReader.h
 *
 *  Created on: Oct 22, 2012
 *      Author: tiago
 */

#ifndef CSVREADER_H_
#define CSVREADER_H_
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

class CSVReader {
public:
	CSVReader();
	~CSVReader();

	static std::vector< std::vector<double> > readValues(std::string filename);
};

#endif /* CSVREADER_H_ */
