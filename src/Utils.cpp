
#include "stdafx.h"
#include "Utils.h"
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"
#ifndef __unix__
#include "../3party/curl/curl.h"
#else
#include <curl.h>
#endif
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <stdio.h>

std::string IntToStr(const int a)
{
	return boost::lexical_cast<std::string>(a);
}

std::string Int64ToStr(const long long a)
{
	return boost::lexical_cast<std::string>(a);
}

int StrToInt(const std::string str, bool& res)
{
	int a = 0;
	try
	{
		a = boost::lexical_cast<int>(str);
		res = true;
	}
	catch (...)
	{
		res = false;
	}
	return a;
}

long long StrToInt64(const std::string str, bool& res)
{
	long long a = 0;
	try
	{
		a = boost::lexical_cast<long long>(str);
		res = true;
	}
	catch (...)
	{
		res = false;
	}
	return a;
}

std::string FloatToStr(const float a)
{	   
	return boost::lexical_cast<std::string>(a);
}

std::string UrlEncode(const std::string &value) 
{
	CURL* curl = curl_easy_init();
	char* output = curl_easy_escape(curl, value.c_str(), value.size());
	std::string res = output;

#ifdef __unix__
	int pos = res.find_first_of(".");
	while (pos != std::string::npos)
	{
		res[pos] = '%';
		res.insert(pos + 1, "2E");
		pos = res.find_first_of(".");
	}
#endif

	curl_free(output);
	curl_easy_cleanup(curl);	

    return res;
}

std::string GetGUID()
{
	boost::uuids::uuid uuid = boost::uuids::random_generator()();	
	return boost::lexical_cast<std::string>(uuid);
}

long long GetUnixTimeInMS()
{
	boost::posix_time::ptime pt;
	static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	boost::posix_time::time_duration diff(pt - epoch);
	return (diff.ticks() / diff.ticks_per_second());	
}

long long GetUnixTimeInMS(const std::string str)
{
	try
	{
		boost::posix_time::ptime pt(boost::posix_time::time_from_string(str));
		static boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
		boost::posix_time::time_duration diff(pt - epoch);
		return (diff.ticks() / diff.ticks_per_second());	
	}
	catch (std::exception e)
	{
		return 0;
	}	
}

std::string GetPatchedJSON(std::ostringstream& buf)
{
	std::string res = buf.str();
	int pos1 = res.find_first_of("[");
	int pos2 = res.find_last_of("]");
	if ((pos1 != std::string::npos) && (pos2 != std::string::npos))
	{
		res = res.substr(pos1, pos2 - pos1 + 1);
		res.erase(std::remove(res.begin(), res.end(), '\n'), res.end());
		return res;
	}
	else
	{
		return "";
	}
}

std::string StringArrayToJSON(const char** const strArray, const unsigned int count)
{
	if (!strArray)
		return "";

	boost::property_tree::ptree pt;
	
	for (unsigned int i = 0; i < count; i++)
	if (strArray[i])
	{				
		boost::property_tree::ptree child;
		child.put("", strArray[i]);
		pt.push_back( std::make_pair("", child));
	}	
	
	std::ostringstream buf;

	boost::property_tree::ptree pt2;
	pt2.add_child("__temp__", pt);

	boost::property_tree::write_json(buf, pt2);
	return GetPatchedJSON(buf); 	
}

std::string GetSHA1(const std::string str)
{
	boost::uuids::detail::sha1 s;		
	s.process_bytes(str.c_str(), str.size());
	unsigned int hashDigest[5];
	s.get_digest(hashDigest);
	char hashStr[21];
	hashStr[20] = 0;
	memcpy(hashStr, hashDigest, 20);
	return hashStr;
}

std::string ForceDotAsSeparatorForDecimalPoint(std::string value)
{
	std::string res = value;

	for (unsigned int i = 0; i < res.size(); i++)
	if (res[i] == ',')
		res[i] = '.';
	
	return res;
}

std::vector<std::string> Explode(std::string const& s, const char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(token);
    }

    return result;
}

struct xmMaskItem
{
	unsigned chMask;
	unsigned uShift;
	unsigned uSize;
	unsigned wcMax;
};
static const xmMaskItem xm_utf8string_pMaskItem[] =
{
	{ 0x80,  0*6, 1, 0x7F,   },   // 1 byte sequence
	{ 0xC0,  1*6, 2, 0x7FF,  },   // 2 byte sequence
	{ 0xE0,  2*6, 3, 0xFFFF, },   // 3 byte sequence
	{ 0xF0,  3*6, 4, 0x1FFFFF, }, // 4 byte sequence
};

void swcs2utf8(
               const wchar_t* pwszWC, // pointer to unicode string that is converterd
               char* pbszUTF8, // pointer to UTF8 buffer string is converterd to
               unsigned uMaxLength, //!< max length of UTF8 buffer that can receivie characters
               const wchar_t* pwszEnd //!< pointer to end of unicode string
               )
{
    char* pbszMax;
    unsigned uShift;
    const xmMaskItem* pMaskItem;
    
    pbszMax = pbszUTF8 + uMaxLength;
    
    while( (*pwszWC != L'\0') && (pwszWC != pwszEnd) && (pbszUTF8 < pbszMax) )
    {
        if( *pwszWC < 0x80 )
        {
            *pbszUTF8 = (char)*pwszWC;
            pbszUTF8++;
        }
        else
        {
            for( pMaskItem = &xm_utf8string_pMaskItem[1]; pMaskItem->chMask; pMaskItem++ )
            {
                if( *pwszWC < pMaskItem->wcMax ) break;
            }
            
            // Check size for buffer
            if( pbszUTF8 > (pbszMax - pMaskItem->uSize) ) break;
            
            uShift = pMaskItem->uShift;
            *pbszUTF8 = (char)(pMaskItem->chMask | (*pwszWC >> uShift));
            pbszUTF8++;
            while( uShift > 0 )
            {
                uShift -= 6;
                *pbszUTF8 = (char)(0x80 | (*pwszWC >> uShift)) & 0xBF;
                pbszUTF8++;
            }
        }
        pwszWC++;
    }
    
    *pbszUTF8 = '\0';
}



std::string wchar_to_utf8_hex(wchar_t ch)
{
	// http://stackoverflow.com/questions/3300025/how-do-i-html-url-encode-a-stdwstring-containing-unicode-characters
	std::string output;
	const int UTF8_STR_SIZE = 100;
	char utf8[UTF8_STR_SIZE];
	{
		swcs2utf8(&ch, &utf8[0], UTF8_STR_SIZE, &ch + 1);
		for (char *p = &utf8[0]; *p; *p++) 
		{
			char oneHex[5];
#ifdef __unix__
			snprintf(oneHex, sizeof(oneHex), "%%%02.2X", (unsigned char)*p);
#else
			sprintf_s(oneHex, sizeof(oneHex), "%%%02.2X", (unsigned char)*p);
#endif
			output.append(oneHex);
		}
	}	

	return output;
}

std::string UrlEncodeW(const wchar_t* const input)
{
	std::string escaped;
	size_t len = wcslen(input);
	for (size_t x = 0; x < len; x++) 
	{
		wchar_t c = input[x];
		if (!(c >= L'A' && c <= L'Z') && !(c >= L'a' && c <= L'z')
			&& !(c >= L'0' && c <= L'9') && c != L'.' && c != L'_'
			&& c != L'-' && c != '~')
		{	
			escaped += wchar_to_utf8_hex(c);			
		}
		else
			escaped += (char)c;
	}

	return escaped;
}
