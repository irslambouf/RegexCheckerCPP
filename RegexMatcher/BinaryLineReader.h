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
	std::vector<fs::path> * fileList;
	std::fstream * input;
	
	const int DEFAULT_BUFFER_SIZE = 4096;
	const int NEW_LINE = 0x0A;
	const int CARRIAGE_RETURN = 0x0D;

	int start = 0;
	int end = 0;
	int readBufferSize = DEFAULT_BUFFER_SIZE;
	int lastStartIncrease = 0;
	bool skipRemainder = false;

	unsigned char * readBuffer;
	unsigned char * remainderBuffer;

public:
	BinaryLineReader(std::string FILE_OR_FOLDER);
	~BinaryLineReader();
};