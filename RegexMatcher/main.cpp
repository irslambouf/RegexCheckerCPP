#include <iostream>
#include <string>
#include <regex>

#include "BinaryMappedReader.h"

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cerr << "ERROR: Failed to provide enough arguments." << std::endl;
		std::cerr << "Usage: RegexMatcher.exe [regex expression] [in:filename] [out:filename]" << std::endl;
		std::cerr << "Exiting..." << std::endl;
		return -1;
	}
	
	try {
		std::regex regex = std::regex(argv[1]);
		std::cout << "[+] Input regex is good - " << argv[1] << std::endl;
	}
	catch (std::regex_error& e) {
		std::cerr << "ERROR: Invalid regex input" << std::endl;
		std::cerr << "Regex expression below:" << std::endl;
		std::cerr << argv[1] << std::endl;
		std::cerr << "Exiting..." << std::endl;
		return -1;
	}

	/*std::fstream fileOrFolder("C:\\pwraw\\Exploit.in\\exploit.in\\Exploit.in", std::ios::binary | std::ios::in);
	bool isDirectory;
	std::vector<std::filesystem::path> fileList;
	std::filesystem::path p;

	if (fileOrFolder.is_open() && fileOrFolder.good()) {
		std::cout << "[+] Input is good - " << argv[2] << std::endl;
		fileOrFolder.close();

		p = std::filesystem::path(argv[2]);
		std::error_code errorCode;

		if (std::filesystem::is_directory(p, errorCode)) {
			isDirectory = true;
		}

		if (errorCode) {
			std::cerr << "Error when calling is_directory, " << errorCode.message();
			std::cerr << "Exiting..." << std::endl;
			return -1;
		}
	}
	else
	{
		std::cerr << "Error while trying to read input file or folder" << std::endl;
		std::cerr << argv[2] << std::endl;
		std::cerr << "Exiting..." << std::endl;
		return -1;
	}*/
	/*bool isDirectory = true;
	std::vector<std::filesystem::path> fileList;
	std::filesystem::path p;
	fileList = std::vector<std::filesystem::path>();
	if (isDirectory) {
		std::cout << "[+] Input is folder, listing files:" << std::endl;
		for (const std::filesystem::path & path : std::filesystem::directory_iterator(argv[2])) {
			fileList.push_back(path);
			std::cout << "- " << path.string() << std::endl;
		}
	}
	else
	{
		std::cout << "[+] Input is file, " << p.string() << std::endl;
		fileList.push_back(p);
	}*/
	
	BinaryMappedReader reader = BinaryMappedReader(argv[2]);
	char * line;
	uint64_t count;
	uint64_t size;
	while ((line = reader.readLine(size)) != NULL) {
		
	}

	//BinaryLineReader * reader = new BinaryLineReader("C:\\pwraw\\Exploit.in\\exploit.in\\Exploit.in");

	//char * line;
	//int length;
	//while ((line = reader->readLine(length)) != NULL)
	//{
	//	//std::cout << std::string(line, length);
	//}


	system("pause");
}