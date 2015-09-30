
#pragma once

#include <string>

#define MAX_SLOTS	5

typedef struct _SCustomVar
{
	std::string name;
	std::string value;
	bool isUsed;

	_SCustomVar()
	{
		isUsed = false;
	}

	void Set(const std::string _name, const std::string _value)
	{
		isUsed = true;
		name = _name;
		value = _value;
	}

	void Clear()
	{
		isUsed = false;
		name = "";
		value = "";
	}

	
} SCustomVar;

class CCustomVars
{
public:
	CCustomVars();
	~CCustomVars();

	bool Set(const int index, const std::string _name, const std::string _value);
	bool Get(const int index, char*& _name, char*& _value);
	void Clear();

	std::string ToJSON();
	bool IsEmptyArray()const;
	bool FromJSON(const std::string json);

private:
	SCustomVar m_items[MAX_SLOTS + 1];
};