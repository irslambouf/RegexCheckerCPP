#include "BinaryMappedReader.h"

BinaryMappedReader::BinaryMappedReader(std::string FILE_OR_FOLDER)
{
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
		std::cout << "[+] Input is folder, listing files:" << std::endl;
		for (const fs::path & path : fs::directory_iterator(FILE_OR_FOLDER)) {
			fileList->push_back(path);
			std::cout << "- " << path.string() << std::endl;
		}
	}
	else
	{
		std::cout << "[+] Input is file, " << path.string() << std::endl;
		fileList->push_back(path);
	}
}

BinaryMappedReader::~BinaryMappedReader()
{
	if (fileList != NULL) {
		delete fileList;
		fileList = NULL;
	}

	if (readBuffer != NULL) {
		delete[] readBuffer;
		readBuffer = NULL;
	}

	if (remainderBuffer != NULL) {
		delete[] remainderBuffer;
		remainderBuffer = NULL;
	}

	if (returnBuffer != NULL) {
		delete[] returnBuffer;
		returnBuffer = NULL;
	}

	if (mappedData != NULL) {
		delete mappedData;
		mappedData = NULL;
	}
}

char * BinaryMappedReader::readLine(uint64_t & length)
{
	if (mappedData == NULL) {
		mappedData = getMemoryMappedPointer();

		if (mappedData == NULL) {
			return NULL;
		}
	}

	return getByteLine(length);
}

std::string BinaryMappedReader::getCurrentFile()
{
	if (fileList == NULL) {
		return NULL;
	}

	return fileList->at(fileListIndex - 1).string();
}

MemoryMapped * BinaryMappedReader::getMemoryMappedPointer()
{
	if (fileList == NULL) {
		return NULL;
	}

	if (mappedData != NULL) {
		// Clean up
		mappedData->close();
		delete mappedData;
		mappedData = NULL;
	}

	MemoryMapped * data = new MemoryMapped(fileList->at(fileListIndex++).string(), this->TWO_GIGABYTES, MemoryMapped::SequentialScan);

	if (!data->isValid()) {
		std::cerr << "Error while opening file for memory mapping: " << fileList->at(fileListIndex).string() << std::endl;
		return NULL;
	}

	if (startAt != 0) {
		startAt = 0;
	}

	firstReadOnFile = true;
	std::cout << "Reading " << fileList->at(fileListIndex - 1).string() << std::endl;
	return data;
}

int64_t BinaryMappedReader::findEndOfLine()
{
	if (readBuffer == NULL) {
		return -1;
	}

	for (uint64_t i = start; i < readBufferSize; i++) {
		if (readBuffer[i] == CARRIAGE_RETURN || readBuffer[i] == NEW_LINE) {
			return i;
		}
	}

	// Failure
	return -1;
}

int64_t BinaryMappedReader::findFirstNotEndOfLine()
{
	if (readBuffer == NULL) {
		return -1;
	}

	for (uint64_t i = start; i < readBufferSize; i++) {
		if (readBuffer[i] != CARRIAGE_RETURN && readBuffer[i] != NEW_LINE) {
			return i;
		}
	}

	//Failure
	return -1;
}

uint64_t BinaryMappedReader::readToBuffer()
{
	uint64_t count = 0;

	if (firstReadOnFile) {
		firstReadOnFile = false;


		readBuffer = (char *)mappedData->getData();
		if (mappedData->size() < TWO_GIGABYTES) {
			readBufferSize = mappedData->size();
			count = readBufferSize;
			startAt = readBufferSize;
		}
		else {
			startAt = TWO_GIGABYTES;
		}
	}
	else {
		// See how many bytes remain unread from file
		size_t remainingBytes = mappedData->size() - startAt;

		if (remainingBytes <= 0) {
			return 0;
		}

		// We need to read more data but buffer is full 
		// Save what remains and read some more
		if (readBufferSize - start > 0) {
			if (!skipRemainder) {
				remainderBuffer = new char[readBufferSize - start];
				memcpy(remainderBuffer, readBuffer + (start * sizeof(char)), readBufferSize - start);
			}
			start = 0;
		}
		else {
			// Make sure we restart at right spot in new buffer
			((readBufferSize - start < 0 && lastStartIncrease == 2) ? start = 1 : start = 0);
		}

		// Adjust size if still to large
		if (remainingBytes > TWO_GIGABYTES) {
			remainingBytes = TWO_GIGABYTES;
		}

		// Remap & read
		mappedData->remap(startAt, remainingBytes);
		readBuffer = (char *)mappedData->getData();

		// Adjust relevant variables
		readBufferSize = remainingBytes;
		count = readBufferSize;
		startAt += readBufferSize;
	}


	return count;
}

char * BinaryMappedReader::getByteLine(uint64_t & length)
{
	// Clean up
	if (returnBuffer != NULL) {
		delete[] returnBuffer;
		returnBuffer = NULL;
	}

	while ((end = findEndOfLine()) == -1) {
		// We have reached end of file need to change fstream to the next file
		if (readToBuffer() <= 0) {
			break;
		}
	}

	// When file does not end with /r or /n
	// i.e. When last line of file is not an empty line
	if (end == -1 && start != 0 && start < readBufferSize) {
		end = readBufferSize;
	}

	// We reached end of file
	if (end == -1) {
		start = 0;
		readBuffer = NULL;
		mappedData = getMemoryMappedPointer();	// Cleanup done in method
		

		if (mappedData == NULL) {
			return NULL;
		}

		return getByteLine(length);
	}

	int destPosition = 0;
	if (remainderBuffer != NULL) {
		// We have data from previous buffer
		returnBuffer = new char[remainderBufferSize + (end - start) + 1];
		memcpy(returnBuffer, remainderBuffer, remainderBufferSize);
		length = remainderBufferSize + (end - start) + 1;

		// Cleanup
		delete[] remainderBuffer;
		remainderBuffer = NULL;
		remainderBufferSize = NULL;
	}
	else {
		returnBuffer = new char[end - start + 1];
		length = end - start + 1;
	}

	memcpy(returnBuffer, readBuffer + (start * sizeof(char)), end - start + 1);

	// Override end of line char to \n
	returnBuffer[length - 1] = NEW_LINE;

	if (readBuffer[end] == CARRIAGE_RETURN) {
		start = end + 2;
		lastStartIncrease = 2;
	}
	else {
		start = end + 1;
		lastStartIncrease = 1;
	}

	// Empty line -> go to next non empty line
	if (length == 1 && returnBuffer[0] == NEW_LINE) {
		// We have multiple empty lines skip to non-empty line
		if (start >= TWO_GIGABYTES || readBuffer[start] == NEW_LINE || readBuffer[start] == CARRIAGE_RETURN) {
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
				mappedData = getMemoryMappedPointer();

				if (mappedData == NULL) {
					return NULL;
				}

				return getByteLine(length);
			}
			else {
				start = possibleNewStart;

				return getByteLine(length);
			}
		}

		return getByteLine(length);
	}

	return returnBuffer;
}
