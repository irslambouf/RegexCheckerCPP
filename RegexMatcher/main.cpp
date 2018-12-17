#include <iostream>
#include <string>
#include <regex>
#include "BinaryLineReader.h"

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

	std::fstream fileOrFolder(argv[2], std::ios::binary | std::ios::in);

	if (fileOrFolder.is_open() && fileOrFolder.good()) {
		std::cout << "[+] Input is good - " << argv[2] << std::endl;
		fileOrFolder.close();
	}

	BinaryLineReader * reader = new BinaryLineReader("C:\\pwraw\\Exploit.in\\exploit.in\\Exploit.in");

	char * line;
	int length;
	while ((line = reader->readLine(length)) != NULL)
	{
		//std::cout << std::string(line, length);
	}


	system("pause");
}