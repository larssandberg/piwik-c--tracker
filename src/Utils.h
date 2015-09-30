
#pragma once

#include <string>
#include <vector>

std::string IntToStr(const int a);
std::string Int64ToStr(const long long a);
std::string FloatToStr(const float a);

int StrToInt(const std::string str, bool& res);
long long StrToInt64(const std::string str, bool& res);

std::string UrlEncode(const std::string &value);
std::string GetGUID();

long long GetUnixTimeInMS();
long long GetUnixTimeInMS(const std::string str);

std::string StringArrayToJSON(const char** const strArray, const unsigned int count);

std::string GetSHA1(const std::string str);

std::string ForceDotAsSeparatorForDecimalPoint(std::string value);

std::vector<std::string> Explode(std::string const& s, const char delim);

std::string GetPatchedJSON(std::ostringstream& buf);

std::string UrlEncodeW(const wchar_t* const input);