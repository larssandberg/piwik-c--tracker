
#pragma once

class ICriticalSection
{
public:
	virtual void Enter() = 0;
	virtual void Leave() = 0;

	virtual ~ICriticalSection() {};
};