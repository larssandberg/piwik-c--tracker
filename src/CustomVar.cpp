
#include "stdafx.h"
#include "CustomVar.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "Utils.h"

CCustomVars::CCustomVars()
{
}

bool CCustomVars::Set(const int index, const std::string _name, const std::string _value)
{
	if ((index >= 1) && (index <= MAX_SLOTS))
	{
		m_items[index].Set(_name, _value);
		return true;
	}

	return false;
}

bool CCustomVars::Get(const int index, char*& _name, char*& _value)
{
	if ((index >= 1) && (index <= MAX_SLOTS))
	{
		if (!m_items[index].isUsed)
		{
			return false;
		}

		_name = (char*)m_items[index].name.c_str();
		_value = (char*)m_items[index].value.c_str();
		return true;
	}

	return false;
}

void CCustomVars::Clear()
{
	for (unsigned int i = 1; i <= MAX_SLOTS; i++)
	{
		m_items[i].Clear();
	}
}

std::string CCustomVars::ToJSON()
{
	boost::property_tree::ptree pt;
	
	for (unsigned int i = 1; i <= MAX_SLOTS; i++)
	{
		boost::property_tree::ptree ptItem;

		ptItem.push_back(std::make_pair("", boost::property_tree::ptree(m_items[i].name)));
		ptItem.push_back(std::make_pair("", boost::property_tree::ptree(m_items[i].value)));
		
		pt.push_back( std::make_pair("", ptItem));
	}	
	
	std::ostringstream buf; 

	boost::property_tree::ptree pt2;
	pt2.add_child("__temp__", pt);

	boost::property_tree::write_json(buf, pt2);	 
	return GetPatchedJSON(buf);
}
	
bool CCustomVars::IsEmptyArray()const
{
	for (unsigned int i = 0; i < MAX_SLOTS; i++)
	{		
		if (m_items[i].isUsed)
		{
			return false;
		}		
	}

	return true;
}

bool CCustomVars::FromJSON(const std::string json)
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(json, pt);
	int i = 0;

	BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt)
	{
		std::vector<std::string> oneItem;
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, v.second)
		{
			oneItem.push_back(v2.second.data());
		}
		m_items[i].name = oneItem[0];
		m_items[i].value = oneItem[1];
		m_items[i].isUsed = ((m_items[i].name.size() > 0) || (m_items[i].value.size() > 0));
		i++;
		if (i == MAX_SLOTS + 1)
		{
			break;
		}
    }	

	return true;
}

CCustomVars::~CCustomVars()
{
}
