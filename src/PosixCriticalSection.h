#pragma once

#ifdef __unix__

#include "ICriticalSection.h"
#include <pthread.h>

class CPosixCriticalSection : public ICriticalSection
{
public:
	CPosixCriticalSection();
	~CPosixCriticalSection();

	void Enter();
	void Leave();

private:
	pthread_mutex_t m_mutex;
};

#endif