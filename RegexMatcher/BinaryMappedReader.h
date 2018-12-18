#include <cstdio>
#include <vector>
#include <filesystem>
#include <iostream>
#include "MemoryMapped.h"

namespace fs = std::filesystem;

class BinaryMappedReader {
private:
	bool isDirectory = false;
	int fileListIndex = 0;
	std::vector<fs::path> * fileList = NULL;

	const size_t TWO_GIGABYTES = 2 << 30;
	const int NEW_LINE = 0x0A;					// \n
	const int CARRIAGE_RETURN = 0x0D;			// \r

	uint64_t start = 0;			// For buffer data
	int64_t end = 0;
	uint64_t startAt = 0;		// For mapped data

	int lastStartIncrease = 0;
	bool skipRemainder = false;
	bool firstReadOnFile = true;
	
	char * readBuffer = NULL;
	uint64_t readBufferSize = TWO_GIGABYTES;

	char * remainderBuffer = NULL;
	uint64_t remainderBufferSize = NULL;

	char * returnBuffer = NULL;
	MemoryMapped * mappedData = NULL;
	
	MemoryMapped * getMemoryMappedPointer();
	int64_t findEndOfLine();
	int64_t findFirstNotEndOfLine();
	uint64_t readToBuffer();
	char * getByteLine(uint64_t & length);
public:
	BinaryMappedReader(std::string FILE_OR_FOLDER);
	~BinaryMappedReader();
	char * readLine(uint64_t& length);
	std::string getCurrentFile();
};
