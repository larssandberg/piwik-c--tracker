
#include "stdafx.h"
#include "CURLHttpRequest.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef __unix__ 
#include "../3party/curl/curl.h"
#else
#include <curl.h>
#endif
#include "Utils.h"
 
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	SMemoryStruct *mem = (SMemoryStruct *)userp;
 
	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) 
	{
		return 0;
	}
 
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
 
	return realsize;
}

CCURLHttpRequest::CCURLHttpRequest():
	m_lastErrorCode(HTTP_REQUEST_OK),
	m_lastHttpCode(0),
	m_timeout(0)
{
	curl_global_init(CURL_GLOBAL_ALL);	
}

int CCURLHttpRequest::Make(const std::string& url)
{ 
	CURL* session = curl_easy_init();
	curl_easy_setopt(session, CURLOPT_URL, url.c_str());
	curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(session, CURLOPT_WRITEDATA, (void *)&m_buffer);
	curl_easy_setopt(session, CURLOPT_USERAGENT, (m_userAgent.size() == 0) ? "libcurl-agent/1.0" : m_userAgent.c_str());  
	if (m_timeout != 0)
	{
		curl_easy_setopt(session, CURLOPT_TIMEOUT, m_timeout);
	}
	if (m_acceptLanguage.size() > 0)
	{
		struct curl_slist *list = NULL;
		std::string langStr = "Accept-Language: " + m_acceptLanguage;
		list = curl_slist_append(list, langStr.c_str());
		curl_easy_setopt(session, CURLOPT_HTTPHEADER, list);		
	}
	CURLcode res = curl_easy_perform(session);
	m_lastErrorCode = (int)res;
	m_lastHttpCode = 0;
	curl_easy_getinfo (session, CURLINFO_RESPONSE_CODE, &m_lastHttpCode);
	curl_easy_cleanup(session);

	if (res != CURLE_OK) 
	{
		return HTTP_REQUEST_INTERNAL_ERROR;	
	}
	else if ((m_lastHttpCode != 200) && (m_lastHttpCode != 204))
	{
		return HTTP_REQUEST_HTTP_ERROR_CODE;
	}	
	else 
	{
		return HTTP_REQUEST_OK;        
	}  
}

std::string CCURLHttpRequest::GetContent()const
{	
	return m_buffer.memory;
}

std::string CCURLHttpRequest::GetErrorAsString()const
{
	if (m_lastHttpCode != 200)
	{
		return "HTTP Status Code: " + IntToStr(m_lastHttpCode);
	}

	return curl_easy_strerror((CURLcode)m_lastErrorCode);
}

void CCURLHttpRequest::SetRequestTimeout(unsigned int timeout)
{
	m_timeout = timeout;
}

void CCURLHttpRequest::SetUserAgent(std::string agent)
{
	m_userAgent = agent;
}

void CCURLHttpRequest::SetAcceptLanguage(std::string lang)
{
	m_acceptLanguage = lang;
}

CCURLHttpRequest::~CCURLHttpRequest()
{
	curl_global_cleanup();
}
