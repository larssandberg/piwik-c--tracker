
#pragma once

#include "IThread.h"
#include <windows.h>

class CWindowsThread : public IThread
{
public:
	CWindowsThread();
	~CWindowsThread();

	void Start(void* (*func)(void*), void* param);
	void Stop();

private:
	HANDLE m_handle;
};