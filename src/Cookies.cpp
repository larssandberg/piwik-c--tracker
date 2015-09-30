
#include "stdafx.h"
#include "Cookies.h"
#include "Utils.h"

CCookies::CCookies()
{
}

bool CCookies::SetFileName(const std::string& fileName)
{
	m_fileName = fileName;
#ifdef _WIN32
	FILE* fl = NULL;
	errno_t err = fopen_s(&fl, fileName.c_str(), "rb");
	if (err != 0)
		fl = NULL;
#else
	FILE* fl = fopen(fileName.c_str(), "rb");
#endif
	if (!fl)
	{
		return false;
	}

	fseek(fl, 0, 2);
	unsigned int size = ftell(fl);
	fseek(fl, 0, 0);

	while (!feof(fl))
	{
		std::string key, value;
		if (!ReadBinaryString(fl, key))
		{
			fclose(fl);
			return false;
		}
		if (!ReadBinaryString(fl, value))
		{
			fclose(fl);
			return false;
		}
		long long timeStamp = 0;
		if (fread(&timeStamp, 1, sizeof(timeStamp), fl) != sizeof(timeStamp))
		{
			fclose(fl);
			return false;
		}

		if (timeStamp > GetUnixTimeInMS())
		{
			SetInMemory(key, value, timeStamp);
		}
	}
		
	fclose(fl);
	return true;
}

bool CCookies::ReadBinaryString(FILE* fl, std::string& res)
{
	int count;
	if (fread(&count, 1, 4, fl) != 4)
	{
		return false;
	}
	char* str = new char[count + 1];
	str[count] = 0;
	if (fread(str, 1, count, fl) != count)
	{
		delete[] str;
		return false;
	}
	res = str;
	delete[] str;
	return true;
}

bool CCookies::WriteBinaryString(FILE* fl, const std::string& str)
{
	int count = str.size();
	if (fwrite(&count, 1, 4, fl) != 4)
	{
		return false;
	}
	if (fwrite(str.c_str(), 1, count, fl) != count)
	{
		return false;
	}
	return true;
}

void CCookies::Set(const std::string& key, const std::string& value,  const long long timeStamp)
{
	SetInMemory(key, value, timeStamp);
	SaveAll();
}

void CCookies::SetInMemory(const std::string& key, const std::string& value, const long long timeStamp)
{
	std::map<std::string, SCookieValue>::iterator it = m_values.find(key);
	SCookieValue valueObj(value, timeStamp);
	if (it != m_values.end())
	{		
		(*it).second = valueObj;
	}
	else
	{
		m_values.insert(std::map<std::string, SCookieValue>::value_type(key, valueObj));
	}
}

bool CCookies::Get(const std::string& key, std::string& value)const 
{
	std::map<std::string, SCookieValue>::const_iterator it = m_values.find(key);
	if (it != m_values.end())
	{
		value = (*it).second.value;
		return true;
	}
	else
	{
		value = "";
		return false;
	}
}

bool CCookies::GetValueByNameMask(const std::string& key, std::string& value)const
{
	std::map<std::string, SCookieValue>::const_iterator it = m_values.begin();
	std::map<std::string, SCookieValue>::const_iterator itEnd = m_values.end();

	for ( ; it != itEnd; it++)
	if ((*it).first.find(key) != std::string::npos)
	{
		value = (*it).second.value;
		return true;
	}

	return false;
}

void CCookies::SaveAll()
{
#ifdef _WIN32
	FILE* fl = NULL;
	errno_t err = fopen_s(&fl, m_fileName.c_str(), "wb");
	if (err != 0)
		fl = NULL;
#else
	FILE* fl = fopen((char *)m_fileName.c_str(), "wb");
#endif
	if (!fl)
	{
		return;
	}

	bool wasError = false;

	std::map<std::string, SCookieValue>::iterator it = m_values.begin();
	std::map<std::string, SCookieValue>::iterator itEnd = m_values.end();

	for ( ; it != itEnd; it++)
	{
		if (!WriteBinaryString(fl, (*it).first))
		{
			wasError = true;
			break;
		}
		if (!WriteBinaryString(fl, (*it).second.value))
		{
			wasError = true;
			break;
		}
		int size = sizeof((*it).second.timeStamp);
		if (fwrite(&(*it).second.timeStamp, 1, size, fl) != size)
		{
			wasError = true;
			break;
		}
	}

	fclose(fl);

	if (wasError)
	{
		DeleteAll();
	}
}

void CCookies::DeleteAll()
{
	m_values.clear();
	SaveAll();
}

CCookies::~CCookies()
{
}
