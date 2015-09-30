
//#include "stdafx.h"
#include "../src/PiwikTracker.h"
#include "../src/Utils.h"

#ifdef __unix__
#define BOOST_TEST_DYN_LINK 1
#endif
#define BOOST_TEST_MODULE testPiwikTracker 
#include <boost/test/unit_test.hpp>
#ifndef __unix
#include "../3party/curl/curl.h"
#else
#include <curl.h>
#endif
#include <algorithm>

#define API_SHORT_URL	"http://asmorkalov.piwik.pro"
#define API_URL			"http://asmorkalov.piwik.pro/piwik.php"

std::string DecodeUrl(std::string url)
{
	CURL* curl = curl_easy_init();
	int len;
	char* output = curl_easy_unescape(curl, url.c_str(), url.size(), &len);
	std::string res = output;
        
	curl_free(output);
	curl_easy_cleanup(curl);		

	res.erase(std::remove(res.begin(), res.end(), ' '), res.end());

	return res;
}

BOOST_AUTO_TEST_CASE(testConstructor)
{
	CPiwikTracker piwikTracker(1, API_URL);
	
	BOOST_CHECK_EQUAL(piwikTracker.getIdSite(), 1);
	BOOST_CHECK_EQUAL(piwikTracker.getBaseUrl(), API_URL);

	CPiwikTracker piwikTracker2(2, API_SHORT_URL);
	
	BOOST_CHECK_EQUAL(piwikTracker2.getIdSite(), 2);
	BOOST_CHECK_EQUAL(piwikTracker2.getBaseUrl(), API_URL);
}
 
BOOST_AUTO_TEST_CASE(testCustomData)
{
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setCustomData("1abcdef2");
	BOOST_CHECK_EQUAL(piwikTracker.getCustomData(), "1abcdef2");
}

BOOST_AUTO_TEST_CASE(testSetGetClearCustomVariables)
{
	CPiwikTracker piwikTracker(1, API_URL);

	char* name = NULL;
	char* value = NULL;

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(0, "abc", "def", "vizit"), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(0, name, value, "vizit"), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(6, "abc", "def", "vizit"), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(6, name, value, "vizit"), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(1, "abc", NULL, NULL), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(1, name, value, NULL), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(2, NULL, "def", NULL), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(2, name, value, NULL), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(3, NULL, NULL, "visit"), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(3, name, value, "visit"), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(4, "abc", "def", NULL), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(4, name, value, NULL), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(5, NULL, "def", "page"), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(5, name, value, "page"), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(1, "abc", NULL, "xyz"), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(1, name, value, "xyz"), false);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(1, "abc", "def", "xyz"), false);
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(1, name, value, "xyz"), false);
	BOOST_CHECK_EQUAL(name == NULL, true);
	BOOST_CHECK_EQUAL(value == NULL, true);

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(1, "abc", "def", "visit"), true);	
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(1, name, value, "visit"), true);
	BOOST_CHECK_EQUAL(name, "abc");
	BOOST_CHECK_EQUAL(value, "def");

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(2, "abc", "def", "page"), true);	
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(2, name, value, "page"), true);
	BOOST_CHECK_EQUAL(name, "abc");
	BOOST_CHECK_EQUAL(value, "def");

	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(3, "abc", "def", "event"), true);	
	BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(3, name, value, "event"), true);
	BOOST_CHECK_EQUAL(name, "abc");
	BOOST_CHECK_EQUAL(value, "def");

	name = NULL;
	value = NULL;
	piwikTracker.clearCustomVariables();
	for (int i = 1; i <= 5; i++)
	{
		BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(i, name, value, "visit"), false);
		BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(i, name, value, "page"), false);
		BOOST_CHECK_EQUAL(piwikTracker.getCustomVariable(i, name, value, "event"), false);
	}
}

BOOST_AUTO_TEST_CASE(testVisitorAndUserId)
{
	CPiwikTracker piwikTracker(1, API_URL);

	BOOST_CHECK_EQUAL(piwikTracker.getUserId(), "");
	BOOST_CHECK(strlen(piwikTracker.getVisitorId()) > 0);
	piwikTracker.setUserId("abcdefgh01234567");
	BOOST_CHECK_EQUAL(piwikTracker.getUserId(), "abcdefgh01234567");

	std::string oldVisitorId = piwikTracker.getVisitorId();
	piwikTracker.setNewVisitorId();
	BOOST_CHECK(strcmp(piwikTracker.getVisitorId(), (char*)oldVisitorId.c_str()) != 0);
}

BOOST_AUTO_TEST_CASE(testVisitDateTime)
{
	CPiwikTracker piwikTracker(1, API_URL);

	const long long referenceTimeStamp = 1427894055;
	
	BOOST_CHECK(piwikTracker.getTimestamp() != referenceTimeStamp);
	BOOST_CHECK(piwikTracker.setForceVisitDateTime(NULL) == false);
	BOOST_CHECK(piwikTracker.setForceVisitDateTime("abc") == false);	
	BOOST_CHECK(piwikTracker.setForceVisitDateTime("2015-04-01 13:14:15") == true);
	BOOST_CHECK_EQUAL(piwikTracker.getTimestamp(), referenceTimeStamp);	
}

BOOST_AUTO_TEST_CASE(testTrackGoal)
{
	CPiwikTracker piwikTracker(1, API_URL);

	std::string url = piwikTracker.getUrlTrackGoal(1);
	BOOST_CHECK(url.find("idgoal=1") != -1);
	BOOST_CHECK(url.find("revenue=") == -1);

	url = piwikTracker.getUrlTrackGoal(1, 5.5f);
	BOOST_CHECK(url.find("idgoal=1&revenue=5.5") != -1);
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackEcommerceCartUpdate)
{
	CPiwikTracker piwikTracker(1, API_URL);

	std::string url = piwikTracker.getUrlTrackEcommerceCartUpdate(2.2f);	
	BOOST_CHECK(url.find("idgoal=0&revenue=2.2") != -1);
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackEcommerceOrder)
{
	CPiwikTracker piwikTracker(1, API_URL);

	BOOST_CHECK(piwikTracker.getUrlTrackEcommerceOrder(NULL, 1.1f, 2.2f, 3.3f, 4.4f, 5.5f) == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackEcommerceOrder(L"", 1.1f, 2.2f, 3.3f, 4.4f, 5.5f) == NULL);

	std::string url = piwikTracker.getUrlTrackEcommerceOrder(L"order1", 1.1f, 2.2f, 3.311f, 4.4f, 5.5f);	
	BOOST_CHECK(url.find("&idgoal=0&revenue=1.1") != -1);
	BOOST_CHECK(url.find("&ec_st=2.2") != -1);
	BOOST_CHECK(url.find("&ec_tx=3.31") != -1);
	BOOST_CHECK(url.find("&ec_sh=4.4") != -1);
	BOOST_CHECK(url.find("&ec_dt=5.5") != -1);
	BOOST_CHECK(url.find("&ec_id=order1") != -1);

	BOOST_CHECK(piwikTracker.addEcommerceItem(NULL) == false);
	BOOST_CHECK(piwikTracker.addEcommerceItem(NULL, "name1", "cat1", 5.0f, 2) == false);		

	BOOST_CHECK(piwikTracker.addEcommerceItem("sku1", "name1", "cat1", 5.0f, 2) == true);		
	url = piwikTracker.getUrlTrackEcommerceOrder(L"order1", 1.1f, 2.2f, 3.311f, 4.4f, 5.5f);
	url = DecodeUrl(url);
	BOOST_CHECK(url.find(DecodeUrl("&ec_items=%5B%7B%22sku%22%3A%22sku1%22%2C%22name%22%3A%22name1%22%2C%22category%22%3A%22cat1%22%2C%22price%22%3A%225%22%2C%22quantity%22%3A%222%22%7D%5D")) != -1);
	BOOST_CHECK(url.find("&idgoal=0&revenue=1.1") != -1);
	BOOST_CHECK(url.find("&ec_st=2.2") != -1);
	BOOST_CHECK(url.find("&ec_tx=3.31") != -1);
	BOOST_CHECK(url.find("&ec_sh=4.4") != -1);
	BOOST_CHECK(url.find("&ec_dt=5.5") != -1);
	BOOST_CHECK(url.find("&ec_id=order1") != -1);

	BOOST_CHECK(piwikTracker.addEcommerceItem("sku1", "", "", 7.0f, 3) == true);		
	url = piwikTracker.getUrlTrackEcommerceOrder(L"order1", 1.1f, 2.2f, 3.311f, 4.4f, 5.5f);
	url = DecodeUrl(url);	
	BOOST_CHECK(url.find(DecodeUrl("&ec_items=%5B%7B%22sku%22%3A%22sku1%22%2C%22name%22%3A%22%22%2C%22category%22%3A%22%22%2C%22price%22%3A%227%22%2C%22quantity%22%3A%223%22%7D%5D")) != -1);
	BOOST_CHECK(url.find("&idgoal=0&revenue=1.1") != -1);
	BOOST_CHECK(url.find("&ec_st=2.2") != -1);
	BOOST_CHECK(url.find("&ec_tx=3.31") != -1);
	BOOST_CHECK(url.find("&ec_sh=4.4") != -1);
	BOOST_CHECK(url.find("&ec_dt=5.5") != -1);
	BOOST_CHECK(url.find("&ec_id=order1") != -1);	
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackAction)
{
	CPiwikTracker piwikTracker(1, API_URL);

	BOOST_CHECK(piwikTracker.getUrlTrackAction(NULL, NULL) == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(NULL, "") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(L"", NULL) == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(L"", "") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(NULL, "a") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(L"", "a") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(L"a", NULL) == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(L"a", "") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackAction(L"a", "b") == NULL);

	std::string url = piwikTracker.getUrlTrackAction(L"yandex.ru", "link");
	BOOST_CHECK(url.find("link=yandex.ru") != -1);

	url = piwikTracker.getUrlTrackAction(L"level2.zip", "download");
	BOOST_CHECK(url.find("download=level2.zip") != -1);	
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackSiteSearch)
{
	CPiwikTracker piwikTracker(1, API_URL);

	BOOST_CHECK(piwikTracker.getUrlTrackSiteSearch(NULL, L"cat1", 10) == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackSiteSearch(L"keyword1", L"cat1", 10) != NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackSiteSearch(L"keyword1", NULL, 10) != NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackSiteSearch(L"keyword1", L"", 10) != NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackSiteSearch(L"keyword1", L"cat1", 0) != NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackSiteSearch(L"keyword1", L"cat1", -1) != NULL);

	std::string url = piwikTracker.getUrlTrackSiteSearch(L"keyword2");
	BOOST_CHECK(url.find("search=keyword2") != -1);
	BOOST_CHECK(url.find("&search_cat=") == -1);
	BOOST_CHECK(url.find("&search_count=") == -1);	
				
	url = piwikTracker.getUrlTrackSiteSearch(L"keyword1", L"cat1", 10);
	BOOST_CHECK(url.find("search=keyword1&search_cat=cat1&search_count=10") != -1);	
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackContentInteraction)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentInteraction(NULL, NULL, L"a", L"b") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentInteraction(L"", NULL, L"a", L"b") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentInteraction(NULL, L"", L"a", L"b") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentInteraction(L"", L"", L"a", L"b") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentInteraction(L"a", L"", L"c", L"d") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentInteraction(L"", L"b", L"c", L"d") == NULL);

	std::string url = piwikTracker.getUrlTrackContentInteraction(L"inter", L"nam", NULL, NULL);
	BOOST_CHECK(url.find("&c_i=inter&c_n=nam") != -1);
	BOOST_CHECK(url.find("&c_p=") == -1);
	BOOST_CHECK(url.find("&c_t=") == -1);
	url = piwikTracker.getUrlTrackContentInteraction(L"inter", L"nam", L"pc2", L"ct1");
	BOOST_CHECK(url.find("&c_i=inter&c_n=nam&c_p=pc2&c_t=ct1") != -1);
}
 
BOOST_AUTO_TEST_CASE(testGetUrlTrackContentImpression)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentImpression(NULL, NULL, NULL) == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentImpression(NULL, L"a", L"b") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackContentImpression(L"", L"a", L"b") == NULL);

	std::string url = piwikTracker.getUrlTrackContentImpression(L"name1");
	BOOST_CHECK(url.find("&c_n=name1") != -1);
	BOOST_CHECK(url.find("&c_p=Unknown") != -1);
	BOOST_CHECK(url.find("&c_t=") == -1);

	url = piwikTracker.getUrlTrackContentImpression(L"name1", L"piece1", L"target1");
	BOOST_CHECK(url.find("&c_n=name1&c_p=piece1&c_t=target1") != -1);
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackEvent)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.getUrlTrackEvent(NULL, NULL, L"a", "1.1") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackEvent(NULL, L"", L"a", "1.1") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackEvent(L"", NULL, L"a", "1.1") == NULL);
	BOOST_CHECK(piwikTracker.getUrlTrackEvent(L"", L"", L"a", "1.1") == NULL);
	
	std::string url = piwikTracker.getUrlTrackEvent(L"cat1", L"act1");
	BOOST_CHECK(url.find("&e_c=cat1&e_a=act1") != -1);
	BOOST_CHECK(url.find("&e_n=") == -1);
	BOOST_CHECK(url.find("&e_v=") == -1);

	url = piwikTracker.getUrlTrackEvent(L"cat1", L"act1", L"nam1", "1,1");
	BOOST_CHECK(url.find("&e_c=cat1&e_a=act1&e_n=nam1&e_v=1.1") != -1);
}

BOOST_AUTO_TEST_CASE(testGetUrlTrackPageView)
{
	CPiwikTracker piwikTracker(1, API_URL);
	
	std::string url = piwikTracker.getUrlTrackPageView(NULL);
	BOOST_CHECK(url.find("&action_name=") == -1);

	url = piwikTracker.getUrlTrackPageView(L"Main Menu");
	BOOST_CHECK(url.find("&action_name=Main%20Menu") != -1);
} 

BOOST_AUTO_TEST_CASE(testMakingRequestsWithInvalidParameters)
{
	CPiwikTracker piwikTracker(1, API_URL);
	
	BOOST_CHECK(piwikTracker.doTrackAction(NULL, NULL) == 0);
	BOOST_CHECK(piwikTracker.doTrackAction(NULL, "") == 0);
	BOOST_CHECK(piwikTracker.doTrackAction(L"", "") == 0);
	BOOST_CHECK(piwikTracker.doTrackAction(L"", NULL) == 0);

	BOOST_CHECK(piwikTracker.doTrackSiteSearch(NULL, L"cat1", 10) == 0);
	BOOST_CHECK(piwikTracker.doTrackSiteSearch(L"", L"cat1", 10) == 0);

	BOOST_CHECK(piwikTracker.doTrackContentInteraction(NULL, NULL, L"a", L"b") == 0);
	BOOST_CHECK(piwikTracker.doTrackContentInteraction(NULL, L"", L"a", L"b") == 0);
	BOOST_CHECK(piwikTracker.doTrackContentInteraction(L"", NULL, L"a", L"b") == 0);
	BOOST_CHECK(piwikTracker.doTrackContentInteraction(L"", L"", L"a", L"b") == 0);

	BOOST_CHECK(piwikTracker.doTrackContentImpression(NULL, L"a", L"b") == 0);
	BOOST_CHECK(piwikTracker.doTrackContentImpression(L"", L"a", L"b") == 0);

	BOOST_CHECK(piwikTracker.doTrackEvent(NULL, NULL, L"a", "1.1") == 0);
	BOOST_CHECK(piwikTracker.doTrackEvent(L"", NULL, L"a", "1.1") == 0);
	BOOST_CHECK(piwikTracker.doTrackEvent(NULL, L"", L"a", "1.1") == 0);
	BOOST_CHECK(piwikTracker.doTrackEvent(L"", L"", L"a", "1.1") == 0);
}

BOOST_AUTO_TEST_CASE(testUsingCustomVariablesInRequests)
{
	CPiwikTracker piwikTracker(1, API_URL);
	
	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(1, "a 1", "b 2", "visit"), true);	
	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(2, "c 3", "d 4", "visit"), true);	
	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(2, "e 5", "g 6", "page"), true);	
	BOOST_CHECK_EQUAL(piwikTracker.setCustomVariable(3, "f 7", "h 8", "event"), true);
	std::string url = piwikTracker.getUrlTrackPageView(NULL);
	url = DecodeUrl(url);
	BOOST_CHECK(url.find(DecodeUrl("&_cvar=%5B%5B%22a%201%22%2C%22b%202%22%5D%2C%5B%22c%203%22%2C%22d%204%22%5D%2C%5B%22%22%2C%22%22%5D%2C%5B%22%22%2C%22%22%5D%2C%5B%22%22%2C%22%22%5D%5D")) != -1);
	BOOST_CHECK(url.find(DecodeUrl("&cvar=%5B%5B%22%22%2C%22%22%5D%2C%5B%22e%205%22%2C%22g%206%22%5D%2C%5B%22%22%2C%22%22%5D%2C%5B%22%22%2C%22%22%5D%2C%5B%22%22%2C%22%22%5D%5D")) != -1);
	BOOST_CHECK(url.find(DecodeUrl("&e_cvar=%5B%5B%22%22%2C%22%22%5D%2C%5B%22%22%2C%22%22%5D%2C%5B%22f%207%22%2C%22h%208%22%5D%2C%5B%22%22%2C%22%22%5D%2C%5B%22%22%2C%22%22%5D%5D")) != -1);
}

BOOST_AUTO_TEST_CASE(testSetEcommerceView)
{
	CPiwikTracker piwikTracker(1, API_URL);

	BOOST_CHECK(piwikTracker.setEcommerceView(NULL, NULL, "cat", 1.1f) == false);
	BOOST_CHECK(piwikTracker.setEcommerceView(NULL, "", "cat", 1.1f) == false);
	BOOST_CHECK(piwikTracker.setEcommerceView("", NULL, "cat", 1.1f) == false);
	BOOST_CHECK(piwikTracker.setEcommerceView("", "", "cat", 1.1f) == false);

	piwikTracker.setEcommerceView("item1", "subitem 3", "cat", 1.1f);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	url = DecodeUrl(url);
	BOOST_CHECK(url.find(DecodeUrl("&cvar=%5B%5B%22%22%2C%22%22%5D%2C%5B%22%5Fpkp%22%2C%221%2E10000002%22%5D%2C%5B%22%5Fpks%22%2C%22item1%22%5D%2C%5B%22%5Fpkn%22%2C%22subitem%203%22%5D%2C%5B%22%5Fpkc%22%2C%22cat%22%5D%5D")) != -1);

	std::string catName = "cat";
	const char* catNamePtr = catName.c_str();
	const char** const categories = &catNamePtr;
	piwikTracker.setEcommerceView("item1", "subitem 3", categories, 1, 1.1f);

	url = piwikTracker.getUrlTrackPageView(NULL);	
	url = DecodeUrl(url);
	std::string ref = DecodeUrl("&cvar=%5B%5B%22%22%2C%22%22%5D%2C%5B%22%5Fpkp%22%2C%221%2E10000002%22%5D%2C%5B%22%5Fpks%22%2C%22item1%22%5D%2C%5B%22%5Fpkn%22%2C%22subitem%203%22%5D%2C%5B%22%5Fpkc%22%2C%22%5B%5C%22cat%5C%22%5D%22%5D%5D");
	BOOST_CHECK(url.find(ref) != -1);
}

BOOST_AUTO_TEST_CASE(testSetPageCharset)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setPageCharset(NULL) == false);
	BOOST_CHECK(piwikTracker.setPageCharset("cp1251") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&cs=cp1251") != -1);
}

BOOST_AUTO_TEST_CASE(testSetUrl)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setUrl(NULL) == false);
	BOOST_CHECK(piwikTracker.setUrl("http://www.games.com") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&url=http%3A%2F%2Fwww%2Egames%2Ecom") != -1);
}

BOOST_AUTO_TEST_CASE(testSetUrlReferrer)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setUrlReferrer(NULL) == false);
	BOOST_CHECK(piwikTracker.setUrlReferrer("http://www.gdc.com") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&urlref=http%3A%2F%2Fwww%2Egdc%2Ecom") != -1);
}

BOOST_AUTO_TEST_CASE(testSetGenerationTime)
{
	CPiwikTracker piwikTracker(1, API_URL);	
	piwikTracker.setGenerationTime(10);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&gt_ms=10") != -1);
}

BOOST_AUTO_TEST_CASE(testSetIdSite)
{
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setIdSite(2);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("?idsite=2&") != -1);
	BOOST_CHECK(piwikTracker.getIdSite() == 2);
}

BOOST_AUTO_TEST_CASE(testSetCountry)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setCountry(NULL) == false);
	BOOST_CHECK(piwikTracker.setCountry(L"France") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&country=France") != -1);
}

BOOST_AUTO_TEST_CASE(testSetRegion)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setRegion(NULL) == false);
	BOOST_CHECK(piwikTracker.setRegion(L"Scotland") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&region=Scotland") != -1);
}

BOOST_AUTO_TEST_CASE(testSetCity)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setCity(NULL) == false);
	BOOST_CHECK(piwikTracker.setCity(L"New York") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&city=New%20York") != -1);
}

BOOST_AUTO_TEST_CASE(testSetLatitude)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setLatitude(NULL) == false); 
	BOOST_CHECK(piwikTracker.setLatitude("22.456") == true); 

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&lat=22%2E456") != -1);	
}

BOOST_AUTO_TEST_CASE(testSetLongitude)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setLongitude(NULL) == false);
	BOOST_CHECK(piwikTracker.setLongitude("22.456") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&long=22%2E456") != -1);
}

BOOST_AUTO_TEST_CASE(testSetForceNewVisit)
{
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setForceNewVisit();

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&new_visit=1") != -1);
}

BOOST_AUTO_TEST_CASE(testSetIp)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setIp(NULL) == false);
	BOOST_CHECK(piwikTracker.setIp("1.2.3.4") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&cip=1.2.3.4") != -1);
}

BOOST_AUTO_TEST_CASE(testSetLocalTime)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setLocalTime(NULL) == false);
	BOOST_CHECK(piwikTracker.setLocalTime("") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12-34-45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:34:45:") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:34:") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:34") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:34:450") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:340:45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("120:34:45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:34:-50") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("12:-34:45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("-12:34:45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("24:34:45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("23:60:45") == false);
	BOOST_CHECK(piwikTracker.setLocalTime("23:59:60") == false);
	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&h=") == std::string::npos);
	BOOST_CHECK(url.find("&m=") == std::string::npos);
	BOOST_CHECK(url.find("&s=") == std::string::npos);

	BOOST_CHECK(piwikTracker.setLocalTime("12:34:45") == true);
	url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&h=12&m=34&s=45") != std::string::npos);

	BOOST_CHECK(piwikTracker.setLocalTime("1:4:5") == true);
	url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&h=1&m=4&s=5") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetResolution)
{
	// no tests with incorrect parameters because all parameters are unsigned int	
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setResolution(1024, 768);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&res=1024x768") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetBrowserHasCookies)
{
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setBrowserHasCookies(true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&cookie=true") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetDebugStringAppend)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setDebugStringAppend(NULL) == false);
	BOOST_CHECK(piwikTracker.setDebugStringAppend("supertest=1") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&supertest=1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetPlugins)
{
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setPlugins(true, false, true, false, true, false, true, false, true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&fla=1&java=0&dir=1&qt=0&realp=1&pdf=0&wma=1&gears=0&ag=1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetCustomData)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setCustomData(NULL) == false);
	BOOST_CHECK(piwikTracker.setCustomData("some custom data") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&data=some%20custom%20data") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetTokenAuth)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setTokenAuth(NULL) == false);
	BOOST_CHECK(piwikTracker.setTokenAuth("abc") == true);

	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&token_auth=abc") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(testSetAndGetRequestTimeout)
{
	CPiwikTracker piwikTracker(1, API_URL);
	piwikTracker.setRequestTimeout(10);
	BOOST_CHECK(piwikTracker.getRequestTimeout() == 10);	
	piwikTracker.setRequestTimeout(0);
	BOOST_CHECK(piwikTracker.getRequestTimeout() == 0);
}

BOOST_AUTO_TEST_CASE(testSetAttributionInfo)
{
	CPiwikTracker piwikTracker(1, API_URL);
	BOOST_CHECK(piwikTracker.setAttributionInfo("[\"str1\",\"str2\",\"str3\",\"str4\"]") == true);
	std::string url = piwikTracker.getUrlTrackPageView(NULL);	
	BOOST_CHECK(url.find("&_rcn=str1") != std::string::npos);
	BOOST_CHECK(url.find("&_rck=str2") != std::string::npos);
	BOOST_CHECK(url.find("&_refts=str3") != std::string::npos);
	BOOST_CHECK(url.find("&_ref=str4") != std::string::npos);	
}

BOOST_AUTO_TEST_CASE(testUrlEncodeW)
{
	std::string escaped = UrlEncodeW(L"#English text#");
	BOOST_CHECK(escaped == "%23English%20text%23");
}
