#include "BinaryLineReader.h"

BinaryLineReader::BinaryLineReader(std::string FILE_OR_FOLDER)
{
	readBuffer = new char[DEFAULT_BUFFER_SIZE];

	const fs::path path(FILE_OR_FOLDER);
	std::error_code errorCode;

	if (fs::is_directory(path, errorCode)) {
		this->isDirectory = true;
	}

	if (errorCode) {
		std::cerr << "Error when calling is_directory, " << errorCode.message();
		return;
	}

	fileList = new std::vector<fs::path>();

	if (this->isDirectory) {
		for (const fs::path & path : fs::directory_iterator(FILE_OR_FOLDER)) {
			fileList->push_back(path);
		}
	}
	else
	{
		fileList->push_back(path);
	}
}

BinaryLineReader::~BinaryLineReader()
{
	delete[] readBuffer;
	readBuffer = NULL;

	delete[] remainderBuffer;
	remainderBuffer = NULL;

	delete[] returnBuffer;
	returnBuffer = NULL;

	delete fileList;
	fileList = NULL;

	delete input;
	input = NULL;
}

char * BinaryLineReader::readLine()
{
	if (input == NULL) {
		input = getFileStream();

		if (input == NULL) {
			return NULL;
		}
	}

	return getByteLine();
}

std::string BinaryLineReader::getCurrentFile()
{
	return fileList->at(fileList->size() - 1).string();
}

std::fstream * BinaryLineReader::getFileStream()
{
	if (fileList == NULL) {
		return NULL;
	}

	if (fileListIndex == fileList->size()) {
		return NULL;
	}

	if (input != NULL) {
		// Clean up
		input->close();
		delete input;
		input = NULL;
	}

	std::fstream * stream = new std::fstream(fileList->at(fileListIndex), std::ios::in | std::ios::binary);

	if (!input->good()) {
		std::cerr << "Error while opening file: " << fileList->at(fileListIndex).string() << std::endl;
		return NULL;
	}

	std::cout << "Reading " << fileList->at(fileListIndex).string() << std::endl;
	return stream;
}

char * BinaryLineReader::getByteLine()
{
	while ((end = findEndOfLIne()) == -1) {
		// We have reached end of file need to change fstream to next file
		if (readToBuffer() <= 0) {
			break;
		}
	}

	// We reached end of file
	if (end == -1) {
		start = 0;
		delete[] readBuffer;
		readBuffer = new char[DEFAULT_BUFFER_SIZE];
		input = getFileStream();	// Cleanup done in method

		if (input == NULL) {
			return NULL;
		}

		return getByteLine();
	}

	int destPosition = 0;
	if (remainderBuffer != NULL) {
		// We have data from previous buffer
		returnBuffer = new char[sizeof(remainderBuffer) + (end - start) + 1];
		memcpy(returnBuffer, remainderBuffer, sizeof(remainderBuffer));
		delete[] remainderBuffer;
		remainderBuffer = NULL;
	}
	else {
		returnBuffer = new char[end - start + 1];
	}

	memcpy(returnBuffer, readBuffer + (start * sizeof(char)), end - start + 1);
	returnBuffer[sizeof(returnBuffer) - 1] = NEW_LINE;

	if (readBuffer[end] == CARRIAGE_RETURN) {
		start = end + 2;
		lastStartIncrease = 2;
	}
	else {
		start = end + 1;
		lastStartIncrease = 1;
	}

	// Empty line go to next non empty line
	if (sizeof(returnBuffer) == 1 && returnBuffer[0] == NEW_LINE) {
		// We have multiple empty lines skip to non-empty line
		if (start >= sizeof(readBuffer) || readBuffer[start] == NEW_LINE || readBuffer[start] == CARRIAGE_RETURN) {
			int possibleNewStart = 0;
			skipRemainder = true;

			// Read from buffer until we find the first non-newline character
			while ((possibleNewStart = findFirstNotEndOfLine()) == -1)
			{
				// We have reached end of file need to change fstream to next file
				if (readToBuffer() <= 0) {
					break;
				}
			}

			skipRemainder = false;

			// Setup new fstream
			if (possibleNewStart == -1) {
				start = 0;
				delete[] readBuffer;
				readBuffer = new char[DEFAULT_BUFFER_SIZE];
				input = getFileStream();

				if (input == NULL) {
					return NULL;
				}

				return getByteLine();
			}
			else {
				start = possibleNewStart;

				return getByteLine();
			}
		}

		return getByteLine();
	}

	return returnBuffer;
}

int BinaryLineReader::findEndOfLIne()
{
	for (int i = start; i < readBufferSize; i++) {
		if (readBuffer[i] == CARRIAGE_RETURN || readBuffer[i] == NEW_LINE) {
			return i;
		}
	}

	// Failure
	return -1;
}

int BinaryLineReader::findFirstNotEndOfLine()
{
	for (int i = start; i < readBufferSize; i++) {
		if (readBuffer[i] != CARRIAGE_RETURN && readBuffer[i] != NEW_LINE) {
			return i;
		}
	}

	//Failure
	return -1;
}

int BinaryLineReader::readToBuffer()
{
	int count = 0;

	// First read into buffer
	if (sizeof(readBuffer) == DEFAULT_BUFFER_SIZE && readBufferSize == DEFAULT_BUFFER_SIZE && start == 0) {
		input->read(readBuffer, readBufferSize);
		count = input->gcount();

		if (count < readBufferSize) {
			readBufferSize = count;
		}
	}
	else {
		// We have a buffer that contains data but still has space for more data
		if (readBufferSize < sizeof(readBuffer) && sizeof(readBuffer) == DEFAULT_BUFFER_SIZE) {
			input->read(readBuffer + readBufferSize * sizeof(char), sizeof(readBuffer) - readBufferSize);
			count = input->gcount();
		}
		else {
			// We need to read more data
			if (readBufferSize - start > 0) {
				if (!skipRemainder) {
					remainderBuffer = new char[readBufferSize - start];
					memcpy(remainderBuffer, readBuffer + (start * sizeof(char)), readBufferSize - start);
				}
				start = 0;
			}
			else {
				if (readBufferSize - start < 0 && lastStartIncrease == 2) {
					start = 1;
				}
				else
				{
					start = 0;
				}
			}

			delete[] readBuffer;
			readBuffer = new char[DEFAULT_BUFFER_SIZE];
			readBufferSize = 0;
			input->read(readBuffer, DEFAULT_BUFFER_SIZE);
			count = input->gcount();
		}

		if (count >= 0) {
			readBufferSize += count;
		}
	}

	return count;
}


