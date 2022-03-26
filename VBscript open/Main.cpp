#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>

#include "cls.h"
#include "resource.h"

void ExecuteCMD(std::string command, std::string vbsPath) {

	if (command == "cls") {
		ClearScreen();
		return;
	}

	
	//load & lock resource
	HRSRC vbsResource = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN1), L"bin");
	unsigned int vbsResourceSize = SizeofResource(NULL, vbsResource);
	HGLOBAL vbsResourceData = LoadResource(NULL, vbsResource);
	LPVOID vbsData = LockResource(vbsResourceData);

	//replace cmd with our desired command
	std::string toWrite = (char*)vbsData;
	toWrite.insert(toWrite.find_first_of('\"', 0) + 1, command);

	//free resource
	FreeResource(vbsResource);

	//write vbscript file
	std::ofstream file(vbsPath, std::ios::binary);
	file.write(toWrite.c_str(), vbsResourceSize + command.length());
	file.close();

	//open vbscript
	ShellExecuteA(NULL, "open", vbsPath.c_str(), NULL, NULL, SW_SHOW);
	Sleep(500);

	//get output txt path
	std::string stringBuffer = vbsPath;
	std::string vbsOutputPath = stringBuffer.erase(stringBuffer.find_last_of('\\'), stringBuffer.length());
	vbsOutputPath.append("\\m.txt");

	//Check if txt file exists (only exists if command is valid cmd command)
	std::ifstream checkOutput(vbsOutputPath);
	if (checkOutput) {
		
		std::cout << "Please be patient, this might take a while.\n\n";

		//might cause memory leak? check if txt is empty, only continue when its not
		while (1) {			
			std::ifstream checkOutput(vbsOutputPath);

			if ((checkOutput.peek() == std::ifstream::traits_type::eof()) == 0)
				break;

			Sleep(10);
		} 
	}
	//command wasnt recognized, output file wasnt made
	else {
		std::cout << "\n\'" << command << "\' is not recognized as an internal or external command,\noperable program or batch file.\n";
		remove(vbsPath.c_str());
		checkOutput.close();
		return;
	}

	checkOutput.close();

	//gather output from created txt file
	std::ifstream rfile(vbsOutputPath);
	std::ostringstream ss;
	ss << rfile.rdbuf();
	rfile.close();
	std::cout << ss.str();

	//delete vbscript & output.txt
	remove(vbsPath.c_str());
	remove(vbsOutputPath.c_str());

}

int main() {

	//get current path
	char currentPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, currentPath);

	std::string vbsPath = currentPath;
	vbsPath.append("\\t.vbs");

	std::cout << "\nPseudo-command prompt by Gert Niewenhuis\n";
	std::cout << '\n' << currentPath << '>';

	//buffer for user input
	char buffer[MAX_PATH];

	while (1) {

		//get user input
		std::cin.getline(buffer, MAX_PATH);
		
		//execute command if not empty
		if (buffer[0] != '\0')
			ExecuteCMD(buffer, vbsPath);

		//new line
		std::cout << '\n' << currentPath << '>';

		Sleep(1);
	}

	return 0;
}
