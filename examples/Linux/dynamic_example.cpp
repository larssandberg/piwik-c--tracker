
#include <stdio.h>
#include "../../src/IPiwikTracker.h"
#include <dlfcn.h>
#include "unistd.h"
#include <string>

int main()
{
	char path_[1000];
	getcwd(path_, sizeof(path_));
	std::string path = path_;
	path += "/libpiwikdynamic.so";
	void* dll = dlopen((char*)path.c_str(), RTLD_LAZY);
	if (dll)
	{
		typedef IPiwikTracker*(*createPiwikTracker_t)(unsigned int idSite, char* apiUrl);
		createPiwikTracker_t createPiwikTracker = (createPiwikTracker_t)dlsym(dll, "createPiwikTracker");
		typedef void(*deletePiwikTracker_t)(IPiwikTracker* tracker); 
		deletePiwikTracker_t deletePiwikTracker = (deletePiwikTracker_t)dlsym(dll, "deletePiwikTracker");

		if ((!createPiwikTracker) || (!deletePiwikTracker))
		{
			printf("no functions in dll");
			dlclose(dll);
			return -1;
		}

		IPiwikTracker* piwikTracker = createPiwikTracker(1, "http://asmorkalov.piwik.pro");	
		if (!piwikTracker)
		{
			printf("error");
			dlclose(dll);
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
		dlclose(dll);
		printf("ended\n");
		return 0;
	}
	else
	{
		printf("load library error %s\n", dlerror());
		return -1;
	}
}
