
#pragma once

#ifdef __unix__

#include <pthread.h>
#include "IThread.h"

class CPosixThread : public IThread
{
public:
	CPosixThread();
	~CPosixThread();

	void Start(void* (*func)(void*), void* param);
	void Stop();

private:
	pthread_t m_thread;
};

#endif