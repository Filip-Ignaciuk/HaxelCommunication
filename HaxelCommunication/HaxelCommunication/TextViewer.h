#pragma once

#include <fstream>
#include <map>
#include <string>

static class TextViewer
{
private:
	std::map<std::string, bool> m_content;
	std::fstream m_file;
public:
	std::map<std::string, bool> ReadFile(std::string _path);
	TextViewer();
};