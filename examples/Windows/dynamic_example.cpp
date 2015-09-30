// piwik_dynamic_example.cpp : Defines the entry point for the console application.
//

#include "../../src/stdafx.h"
#include "../../src/IPiwikTracker.h"
#include <tchar.h>
#include "windows.h"

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE dll = LoadLibrary(L"piwik_dynamic.dll");
	if (dll)
	{
		typedef IPiwikTracker*(*createPiwikTracker_t)(unsigned int idSite, char* apiUrl);
		createPiwikTracker_t createPiwikTracker = (createPiwikTracker_t)GetProcAddress(dll, "createPiwikTracker");
		typedef void(*deletePiwikTracker_t)(IPiwikTracker* tracker); 
		deletePiwikTracker_t deletePiwikTracker = (deletePiwikTracker_t)GetProcAddress(dll, "deletePiwikTracker");

		if ((!createPiwikTracker) || (!deletePiwikTracker))
		{
			printf("no functions in dll");
			FreeLibrary(dll);
			return -1;
		}

		IPiwikTracker* piwikTracker = createPiwikTracker(1, "http://asmorkalov.piwik.pro");	
		if (!piwikTracker)
		{
			printf("error");
			FreeLibrary(dll);
			return -1;
		}
		unsigned int requestId = piwikTracker->doTrackPageView(L"Game Menu");
		piwikTracker->waitForRequest(requestId);
		printf("status of request: %d\n", piwikTracker->getRequestStatus(requestId));
		printf("error: %s\n", piwikTracker->getRequestErrorString(requestId));
		printf("response: %s\n", piwikTracker->getRequestResponse(requestId));

		requestId = piwikTracker->doTrackSiteSearch(L"hello", L"something", 15);
		piwikTracker->waitForRequest(requestId);
		printf("status of request: %d\n", piwikTracker->getRequestStatus(requestId));
		printf("error: %s\n", piwikTracker->getRequestErrorString(requestId));	
		printf("response: %s\n", piwikTracker->getRequestResponse(requestId));

		requestId = piwikTracker->doTrackGoal(1, 15.5f);
		piwikTracker->waitForRequest(requestId);
		printf("status of request: %d\n", piwikTracker->getRequestStatus(requestId));
		printf("error: %s\n", piwikTracker->getRequestErrorString(requestId));	
		printf("response: %s\n", piwikTracker->getRequestResponse(requestId));
		deletePiwikTracker(piwikTracker);
		FreeLibrary(dll);
		printf("ended\n");
		return 0;
	}
	else
	{
		printf("load library error\n");
		return -1;
	}

	return 0;
}

