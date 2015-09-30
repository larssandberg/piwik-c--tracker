
#pragma once

#include <string>
#include <stdlib.h>
#include "IHttpRequest.h"

typedef struct _SMemoryStruct 
{
	char* memory;
	size_t size;

	_SMemoryStruct()
	{
		memory = (char*)malloc(1);
		size = 0;
	}

	~_SMemoryStruct()
	{
		free(memory);
	}
} SMemoryStruct;

class CCURLHttpRequest : public IHttpRequest
{
public:
	CCURLHttpRequest();
	~CCURLHttpRequest();

	int Make(const std::string& url);
	std::string GetContent()const;
	std::string GetErrorAsString()const;

	void SetRequestTimeout(unsigned int timeout);
	void SetUserAgent(std::string agent);
	void SetAcceptLanguage(std::string lang);

private:
	std::string m_url;
	std::string m_userAgent;
	std::string m_acceptLanguage;
	int m_lastErrorCode;
	int m_lastHttpCode;
	unsigned int m_timeout;
	SMemoryStruct m_buffer;
};