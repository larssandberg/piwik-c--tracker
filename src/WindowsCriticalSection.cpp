
#include "StdAfx.h"
#include "WindowsCriticalSection.h"

CWindowsCriticalSection::CWindowsCriticalSection()
{
	InitializeCriticalSection(&m_cs);
}

void CWindowsCriticalSection::Enter()
{
	EnterCriticalSection(&m_cs);
}

void CWindowsCriticalSection::Leave()
{
	LeaveCriticalSection(&m_cs);
}

CWindowsCriticalSection::~CWindowsCriticalSection()
{
	DeleteCriticalSection(&m_cs);
}