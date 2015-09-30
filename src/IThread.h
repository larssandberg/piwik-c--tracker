
#pragma once

class IThread
{
public:
	virtual ~IThread() {};

	virtual void Start(void* (*func)(void*), void* param) = 0;
	virtual void Stop() = 0;
};