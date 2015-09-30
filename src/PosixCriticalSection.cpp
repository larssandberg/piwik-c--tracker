
#include "StdAfx.h"
#include "PosixCriticalSection.h"

#ifdef __unix__

CPosixCriticalSection::CPosixCriticalSection()
{
	pthread_mutex_init(&m_mutex, NULL);
}

void CPosixCriticalSection::Enter()
{
	pthread_mutex_lock(&m_mutex);	
}

void CPosixCriticalSection::Leave()
{
	pthread_mutex_unlock(&m_mutex);
}

CPosixCriticalSection::~CPosixCriticalSection()
{
	 pthread_mutex_destroy(&m_mutex);
}

#endif