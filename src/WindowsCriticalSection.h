
#pragma once

#include <windows.h>
#include "ICriticalSection.h"

class CWindowsCriticalSection : public ICriticalSection
{
public:
	CWindowsCriticalSection();
	~CWindowsCriticalSection();

	void Enter();
	void Leave();

private:
	CRITICAL_SECTION m_cs;
};