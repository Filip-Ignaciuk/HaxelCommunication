#include <fstream>
#include <map>


static class TextViewer
{
private:
	static std::string m_fileName;
	static std::fstream m_file;
public:
	static const enum readType { Normal, Config };
	static std::string ReadFile(std::string _path, readType _readType)
	{
		m_file.open(_path);
	}
};