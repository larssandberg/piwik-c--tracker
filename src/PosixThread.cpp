#include "StdAfx.h"
#include "PosixThread.h"

#ifdef __unix__

CPosixThread::CPosixThread():
	m_thread(NULL)
{
}

void CPosixThread::Start(void* (*func)(void*), void* param)
{
	if (pthread_create(&m_thread, NULL, func, param) != 0)
	{
		m_thread = NULL;
	}
}

void CPosixThread::Stop()
{
	if (m_thread)
	{
		pthread_cancel(m_thread);
		void* status;
		pthread_join(m_thread, &status);
	}
}

CPosixThread::~CPosixThread()
{

}

#endif