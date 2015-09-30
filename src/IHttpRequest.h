
#pragma once

#define HTTP_REQUEST_OK					0
#define HTTP_REQUEST_INTERNAL_ERROR		1
#define HTTP_REQUEST_HTTP_ERROR_CODE	2

class IHttpRequest
{
public:
	virtual int Make(const std::string& url) = 0;
	virtual std::string GetContent()const = 0;
	virtual std::string GetErrorAsString()const = 0;

	virtual void SetRequestTimeout(unsigned int timeout) = 0;
	virtual void SetUserAgent(std::string agent) = 0;
	virtual void SetAcceptLanguage(std::string lang) = 0;

	virtual ~IHttpRequest() {};
};