
#include "StdAfx.h"
#include "WindowsThread.h"

CWindowsThread::CWindowsThread():
	m_handle(NULL)
{
}

void CWindowsThread::Start(void* (*func)(void*), void* param)
{
	m_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE )func, param, 0, NULL);
}

void CWindowsThread::Stop()
{
	if (m_handle)
	{
		TerminateThread(m_handle, 0);
		CloseHandle(m_handle);
		m_handle = NULL;
	}
}

CWindowsThread::~CWindowsThread()
{
	if (m_handle)
	{
		CloseHandle(m_handle);
	}
}