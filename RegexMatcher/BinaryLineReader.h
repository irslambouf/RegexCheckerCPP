#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include "MemoryMapped.h"
#include <cstdio>

namespace fs = std::filesystem;

class BinaryLineReader {
private:
	bool isDirectory = false;
	int fileListIndex = 0;
	std::vector<fs::path> * fileList = NULL;
	std::fstream * input = NULL;
	
	const int DEFAULT_BUFFER_SIZE = 4096;
	const int NEW_LINE = 0x0A;					// \n
	const int CARRIAGE_RETURN = 0x0D;			// \r

	int start = 0;
	int end = 0;
	
	int lastStartIncrease = 0;
	bool skipRemainder = false;

	char * readBuffer = NULL;
	int readBufferSize = DEFAULT_BUFFER_SIZE;	// 4096
	// Assumption: no line > 4096

	char * remainderBuffer = NULL;
	int remainderBufferSize = NULL;

	char * returnBuffer = NULL;					// Size is stored in argument passed to readLine

	std::fstream * getFileStream();
	char * getByteLine(int& length);
	int findEndOfLIne();
	int findFirstNotEndOfLine();
	int readToBuffer();
public:
	BinaryLineReader(std::string FILE_OR_FOLDER);
	~BinaryLineReader();
	char * readLine(int& length);
	std::string getCurrentFile();
};