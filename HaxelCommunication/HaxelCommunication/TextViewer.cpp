#include "TextViewer.h"


TextViewer::TextViewer()
{

}

std::map<std::string, bool> TextViewer::ReadFile(std::string _path)
{
	TextViewer::m_file.open(_path, std::ios::in);

	if (TextViewer::m_file.is_open())
	{
		std::string line;

		
		while (std::getline(TextViewer::m_file, line))
		{
			std::string key;
			std::string sValue;
			bool isEqual = false;
			int size = line.size();
			int i = 0;
			while (size)
			{
				if (line[i] == '=')
				{
					isEqual = true;
					size--;
					i++;
					continue;
				}

				if (!isEqual)
				{
					key.push_back(line[i]);
				}
				else
				{
					sValue.push_back(line[i]);
				}
				size--;
				i++;
			}

			// We can save ints as bools as values will never exceed above 255
			bool value = std::stoi(sValue);
			TextViewer::m_content.emplace(key, value);


		}
	}
	TextViewer::m_file.close();
	return m_content;
}

