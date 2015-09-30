
#pragma once

#include <string>
#include <vector>
#include "ICriticalSection.h"
#include "IHttpRequest.h"
#include "IThread.h"
#include "IPiwikTracker.h"
#include <boost/thread/thread.hpp> 
#include <boost/thread/recursive_mutex.hpp>

typedef struct _SHttpRequestDesc
{
	std::string url;
	std::string error;
	std::string response;
	int status;

	_SHttpRequestDesc(std::string _url)
	{
		status = REQUEST_STATUS_IN_QUEUE;
		url = _url;
	}
} SHttpRequestDesc;

class CHttpRequestQueue
{
public:
	CHttpRequestQueue();
	~CHttpRequestQueue();

	unsigned int AddRequest(const std::string& url);

	int GetRequestStatus(const unsigned int requestID);
	char* GetRequestErrorString(const unsigned int requestID);
	char* GetRequestResponse(const unsigned int requestID);

	bool IsWorking()const;
	bool ProcessRequest();

	void SetRequestTimeout(unsigned int timeout);
	void SetUserAgent(std::string agent);
	void SetAcceptLanguage(std::string lang);

private:
	IHttpRequest* m_httpRequest;

	boost::recursive_mutex m_queueMutex;
	boost::recursive_mutex m_workMutex;
		
	boost::thread m_thread;

	volatile bool m_isWorking;

	std::vector<SHttpRequestDesc *> m_requests;
	std::vector<SHttpRequestDesc *> m_queue;
};