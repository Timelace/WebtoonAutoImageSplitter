#pragma once

#include <iostream>
#include <vector>

#include "CommonFileDialogApp.h";



class FileSelecter {
public:
	static HRESULT select_file(std::string& input);
	static HRESULT select_folder(std::string& input);
	static HRESULT select_multiple_files(std::vector<std::string>& input);
	static HRESULT select_multiple_folders(std::vector<std::string>& input);
};


