
#include "stdafx.h"
#include "PiwikTracker.h"
#include "Utils.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

std::string _SEcommerceItem::ArrayToJSON(std::vector<_SEcommerceItem>& items)
{
	boost::property_tree::ptree pt;
	
	std::vector<_SEcommerceItem>::iterator it = items.begin();
	std::vector<_SEcommerceItem>::iterator itEnd = items.end();
	for ( ; it != itEnd; it++)
	{
		boost::property_tree::ptree ptItem;
		ptItem.put("sku", (*it).sku);
		ptItem.put("name", (*it).name);
		ptItem.put("category", (*it).category);
		ptItem.put("price", ForceDotAsSeparatorForDecimalPoint((char*)FloatToStr((*it).price).c_str()));
		ptItem.put("quantity", (*it).quantity);

		pt.push_back( std::make_pair("", ptItem));
	}	

	boost::property_tree::ptree pt2;
	pt2.add_child("__temp__", pt);
	
	std::ostringstream buf; 

	boost::property_tree::write_json(buf, pt2);
	return GetPatchedJSON(buf);
}
