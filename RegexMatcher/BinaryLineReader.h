#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

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
	int readBufferSize = DEFAULT_BUFFER_SIZE;	// 4096
	int lastStartIncrease = 0;
	bool skipRemainder = false;

	char * readBuffer = NULL;
	char * remainderBuffer = NULL;
	char * returnBuffer = NULL;

	std::fstream * getFileStream();
	char * getByteLine();
	int findEndOfLIne();
	int findFirstNotEndOfLine();
	int readToBuffer();
public:
	BinaryLineReader(std::string FILE_OR_FOLDER);
	~BinaryLineReader();
	char * readLine();
	std::string getCurrentFile();
};