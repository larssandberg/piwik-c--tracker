
#include "stdafx.h"
#include "PiwikTracker.h"

extern "C" IPiwikTracker* createPiwikTracker(unsigned int idSite, char* apiUrl)
{
	if (!apiUrl)
	{
		return NULL;
	}

	return new CPiwikTracker(idSite, apiUrl);
}

extern "C" void deletePiwikTracker(IPiwikTracker* piwikTracker)
{
	if (piwikTracker)
	{
		delete piwikTracker;
	}
}
