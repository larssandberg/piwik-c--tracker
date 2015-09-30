
#include "stdafx.h"
#include "../../src/HttpRequestQueue.h"
#include "windows.h"
  
int _tmain(int argc, _TCHAR* argv[])
{
	/*CCURLHttpRequest _req;
	IHttpRequest* req = &_req;

	int error = req->Make("http://asmorkalov.piwik.pro/piwik.php?idsite=1&rec=1&url=demo");
	int size = req->GetContent().size();*/

	CHttpRequestQueue queue;
	unsigned int requestId = queue.AddRequest("http://asmorkalov.piwik.pro/piwik.php?idsite=1&rec=1&url=demo");
	unsigned int request2Id = queue.AddRequest("http://asmorkalov.piwik.pro/piwik.php?idsite=1&rec=1&url=demo2");
	Sleep(10000);
		
	printf("%d\n\n", queue.GetRequestStatus(requestId));
	printf("%s", (char*)queue.GetRequestErrorString(requestId).c_str());
	printf("%d\n\n", queue.GetRequestStatus(request2Id));
	printf("%s", (char*)queue.GetRequestErrorString(request2Id).c_str());
 
	return 0;
}

