
#pragma once

#include <string>
#include <map>

typedef struct _SCookieValue
{
	std::string value;
	long long timeStamp;

	_SCookieValue(std::string _value, long long _timeStamp)
	{
		timeStamp = _timeStamp;
		value = _value;
	}

	_SCookieValue(const _SCookieValue& other)
	{
		timeStamp = other.timeStamp;
		value = other.value;
	}
} SCookieValue;

class CCookies
{
public:
	CCookies();
	~CCookies();

	bool SetFileName(const std::string& fileName);

	void Set(const std::string& key, const std::string& value,  const long long timeStamp);
	bool Get(const std::string& key, std::string& value)const;

	bool GetValueByNameMask(const std::string& key, std::string& value)const;

	void DeleteAll();	

private:
	void SaveAll();
	bool ReadBinaryString(FILE* fl, std::string& res);
	bool WriteBinaryString(FILE* fl, const std::string& str);
	void SetInMemory(const std::string& key, const std::string& value, long long timeStamp);

	std::string m_fileName;
	std::map<std::string, SCookieValue> m_values;
};