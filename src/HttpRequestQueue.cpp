
#include "stdafx.h"
#include "HttpRequestQueue.h"
#include "CURLHttpRequest.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

void* ThreadProc(void* param)
{
	CHttpRequestQueue* queue = (CHttpRequestQueue*)param;
	if (!queue)
		return NULL;

	while (queue->IsWorking())
	{
		if (!queue->ProcessRequest())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
	}
	return NULL;
}

CHttpRequestQueue::CHttpRequestQueue()
{
	m_httpRequest = new CCURLHttpRequest();
	m_isWorking = true;

	m_thread = boost::thread(&ThreadProc, this);
}

void CHttpRequestQueue::SetRequestTimeout(unsigned int timeout)
{
	m_httpRequest->SetRequestTimeout(timeout);
}

void CHttpRequestQueue::SetUserAgent(std::string agent)
{
	m_httpRequest->SetUserAgent(agent);
}
	
void CHttpRequestQueue::SetAcceptLanguage(std::string lang)
{
	m_httpRequest->SetAcceptLanguage(lang);
}

unsigned int CHttpRequestQueue::AddRequest(const std::string& url)
{
	boost::lock_guard<boost::recursive_mutex> lock(m_queueMutex);

	SHttpRequestDesc* desc = new SHttpRequestDesc(url);
	m_requests.push_back(desc);
		
	m_queue.push_back(desc);

	return (m_requests.size() - 1);
}

int CHttpRequestQueue::GetRequestStatus(const unsigned int requestID)
{
	boost::lock_guard<boost::recursive_mutex> lock(m_queueMutex);

	if (requestID < m_requests.size())
	{
		return m_requests[requestID]->status;
	}

	return REQUEST_STATUS_REQUEST_NOT_FOUND;
}

char* CHttpRequestQueue::GetRequestErrorString(const unsigned int requestID)
{
	boost::lock_guard<boost::recursive_mutex> lock(m_queueMutex);

	if (requestID < m_requests.size())
	{
		return (char*)m_requests[requestID]->error.c_str();
	}

	return NULL;
}

char* CHttpRequestQueue::GetRequestResponse(const unsigned int requestID)
{
	boost::lock_guard<boost::recursive_mutex> lock(m_queueMutex);

	if (requestID < m_requests.size())
	{
		return (char*)m_requests[requestID]->response.c_str();
	}

	return NULL;
}

bool CHttpRequestQueue::ProcessRequest()
{
	boost::lock_guard<boost::recursive_mutex> lock(m_workMutex);	
	SHttpRequestDesc* desc = NULL;
	m_queueMutex.lock();
	if (m_queue.size() > 0)
	{
		desc = m_queue[0];
		m_queue.erase(m_queue.begin());
	}
	m_queueMutex.unlock();

	if (!desc)
	{		
		return false;
	}

	bool isOk = (m_httpRequest->Make(desc->url) == HTTP_REQUEST_OK);
	desc->status = isOk ? REQUEST_STATUS_OK : REQUEST_STATUS_FAILED;
	desc->error = !isOk ? m_httpRequest->GetErrorAsString() : "";
	desc->response = isOk ? m_httpRequest->GetContent() : "";
	
	return true;
}

bool CHttpRequestQueue::IsWorking()const
{
	return m_isWorking;
}

CHttpRequestQueue::~CHttpRequestQueue()
{
	m_isWorking = false;
	boost::lock_guard<boost::recursive_mutex> lock(m_workMutex);	
	
	try
	{
		m_thread.interrupt();	
		m_thread.join();
	}
	catch (...)
	{
	}

	std::vector<SHttpRequestDesc *>::iterator it = m_requests.begin();
	std::vector<SHttpRequestDesc *>::iterator itEnd = m_requests.end();

	for ( ; it != itEnd; it++)
	{
		delete (*it);
	}
	
	if (m_httpRequest)
	{
		delete m_httpRequest;
	}
}
