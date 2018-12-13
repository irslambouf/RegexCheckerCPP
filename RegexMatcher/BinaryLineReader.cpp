#include "BinaryLineReader.h"

BinaryLineReader::BinaryLineReader(std::string FILE_OR_FOLDER)
{
	readBuffer = new unsigned char[DEFAULT_BUFFER_SIZE];
	remainderBuffer = NULL;

	const fs::path path(FILE_OR_FOLDER);
	std::error_code errorCode;

	if (fs::is_directory(path, errorCode)) {
		this->isDirectory = true;
	}

	if (errorCode) {
		std::cerr << "Error when calling is_directory, " << errorCode.message();
	}

	if (this->isDirectory) {
		fileList = new std::vector<fs::path>();
		for (const fs::path & path : fs::directory_iterator(FILE_OR_FOLDER)) {
			std::cout << path << std::endl;
		}
	}
}

BinaryLineReader::~BinaryLineReader()
{
	delete[] readBuffer;
	readBuffer = NULL;

	delete[] remainderBuffer;
	remainderBuffer = NULL;
}
