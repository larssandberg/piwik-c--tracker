#pragma once

#include "HttpRequestQueue.h"
#include <string>
#include <vector>
#include <map>
#include "Cookies.h"
#include "IPiwikTracker.h"
#include "CustomVar.h"
#include <boost/shared_ptr.hpp>

#define MAX_ATTRIBS 4

typedef struct _SEcommerceItem
{
	std::string sku;
	std::string name;
	std::string category;
	float price;
	int quantity;

	_SEcommerceItem(const char* const _sku, const char* const _name, const char* const _category, const float _price, const int _quantity)
	{
		sku = _sku;
		if (_name)
		{
			name = _name;
		}
		if (_category)
		{
			category = _category;
		}
		price = _price;
		quantity = _quantity;
	}

	_SEcommerceItem(const _SEcommerceItem& other)
	{
		sku = other.sku;
		name = other.name;
		category = other.category;
		price = other.price;
		quantity = other.quantity;
	}

	static std::string ArrayToJSON(std::vector<_SEcommerceItem>& items);
} SEcommerceItem;

class CPiwikTracker : public IPiwikTracker
{
public:
	CPiwikTracker(const unsigned int idSite, const std::string apiUrl = "");
	~CPiwikTracker();

	void setNewVisitorId();
	bool setPageCharset(const char* const charset = "");
	bool setCustomVariable(const int id, const char* const name, const char* const value, const char* const scope = "visit");
	bool setUrl(const char* const url);
	bool setUrlReferrer(const char* const url);
	void setGenerationTime(const unsigned int timeMs);
	bool getCustomVariable(const int id, char*& name, char*& value, const char* const scope = "visit");
	void clearCustomVariables();

	bool setAttributionInfo(const char* const jsonEncoded);

	void setIdSite(unsigned int _idSite);
	bool setBrowserLanguage(const char* const _acceptLanguage);
	bool setUserAgent(const char* const _userAgent);
	bool setCountry(const wchar_t* const _country);
	bool setRegion(const wchar_t* const _region);
	bool setCity(const wchar_t* const _city);
	bool setLatitude(const char* const _lat);
	bool setLongitude(const char* const __long);
	void enableBulkTracking();

	bool enableCookies(const char* const path);

	void disableSendImageResponse();

	int doTrackPageView(const wchar_t* const documentTitle);
	int doTrackEvent(const wchar_t* const category, const wchar_t* const action, const wchar_t* const name = NULL, const char* const value = NULL);
	int doTrackContentImpression(const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL);
	int doTrackContentInteraction(const wchar_t* const interaction, const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL);
	int doTrackSiteSearch(const wchar_t* const keyword, const wchar_t* const category = L"", const int countResults = -1);
	int doTrackGoal(const int idGoal, const float revenue = 0.0f);
	int doTrackAction(const wchar_t* const actionUrl, const char* const actionType);
	int doTrackEcommerceOrder(const wchar_t* const orderId, const float grandTotal, const float subTotal = 0.0, const float tax = 0.0,
		const float shipping = 0.0, const float discount = 0.0);

	bool addEcommerceItem(const char* const sku, const char* const name = "", const char* const category = "", 
		const float price = 0.0f, const int quantity = 1);
	int doTrackEcommerceCartUpdate(const float grandTotal);
	int doBulkTrack();

	char* getUrlTrackPageView(const wchar_t* const documentTitle);
	char* getUrlTrackEvent(const wchar_t* const category, const wchar_t* const action, const wchar_t* const name = NULL, const char* const value = NULL);
	char* getUrlTrackContentImpression(const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL);
	char* getUrlTrackContentInteraction(const wchar_t* const interaction, const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL);
	char* getUrlTrackSiteSearch(const wchar_t* const keyword, const wchar_t* const category = L"", const int countResults = -1);
	char* getUrlTrackGoal(const int idGoal, const float revenue = 0.0f);
	char* getUrlTrackAction(const wchar_t* const actionUrl, const char* const actionType);

	char* getUrlTrackEcommerceOrder(const wchar_t* const orderId, const float grandTotal, const float subTotal = 0.0, 
		const float tax = 0.0, const float shipping = 0.0, const float discount = 0.0);
	char* getUrlTrackEcommerceCartUpdate(const float grandTotal);

	char* getVisitorId();
	char* getUserId()const;

	bool setEcommerceView(const char* const sku = "", const char* const name = "", const char* const category = "", const float price = 0.0);
	bool setEcommerceView(const char* const sku, const char* const name, const char** const categories, const int categoryCount, const float price);
	bool setForceVisitDateTime(const char* const dateTime);
	void setForceNewVisit();
	bool setIp(const char* const _ip);
	void setUserId(const char* const _userId);

	void deleteCookies();
	void getAttributionInfo();
	bool setTokenAuth(const char* const _token_auth);
	bool setLocalTime(const char* const time);

	void setResolution(const unsigned int _width, const unsigned int _height);
	void setBrowserHasCookies(const bool enabled);
	bool setDebugStringAppend(const char* const str);
	void setPlugins(const bool flash = false, const bool java = false, const bool director = false, const bool quickTime = false, 
		const bool realPlayer = false, const bool pdf = false, const bool windowsMedia = false, const bool gears = false, 
		const bool silverlight = false);

	void disableCookieSupport();
	unsigned int getRequestTimeout()const;
	void setRequestTimeout(const unsigned int timeout);	

	bool setCustomData(const char* const _customData);
	char* getCustomData()const;

	int getIdSite()const;

	std::string getBaseUrl();

	long long getTimestamp()const;

	void waitForRequest(const unsigned int requestID);
	int getRequestStatus(const unsigned int requestID);
	char* getRequestErrorString(const unsigned int requestID);
	char* getRequestResponse(const unsigned int requestID);

private:
	CHttpRequestQueue m_requestQueue;

private:
	std::string getCookieName(const std::string cookieName);	
	std::string getRequest(const unsigned int idSite);
	int sendRequest(std::string url, std::string method = "GET", void* data = NULL, bool force = false);

	char* getUrlTrackEcommerce(const float grandTotal, const float subTotal = 0.0f, const float tax = 0.0f, 
		const float shipping = 0.0f, const float discount = 0.0f);

	static std::string getUserIdHashed(const std::string id);
	static std::string getCurrentHost();

	bool getCustomVariablesFromCookie(CCustomVars& vars);

	bool loadVisitorIdCookie();

	void setCookie(const std::string cookieName, const std::string cookieValue, const long long cookieTTL);

	void setFirstPartyCookies();

	std::string getCookieMatchingName(const std::string name);

	static std::string forceDotAsSeparatorForDecimalPoint(const char* const value);
	
	std::string forcedDatetime;
    std::string forcedNewVisit;
	std::string customData;
    int hasCookies;
    unsigned int width;
    unsigned int height;
    std::string plugins;
    unsigned int localHour;
    unsigned int localMinute;
    unsigned int localSecond;

	std::string token_auth;
    std::string userAgent;
	boost::shared_ptr<wchar_t> country;
    boost::shared_ptr<wchar_t> region;
    boost::shared_ptr<wchar_t> city;
    std::string lat;
    std::string _long;

	unsigned int generationTime;

    unsigned int idSite;
	std::string urlReferrer;
	std::string pageCharset;
	std::string pageUrl;

	std::string ip;
    std::string acceptLanguage;

	std::string apiUrl;

	unsigned int configVisitorCookieTimeout;         
    unsigned int configSessionCookieTimeout;         
    unsigned int configReferralCookieTimeout;

	std::string userId;
    std::string forcedVisitorId;
    std::string cookieVisitorId;
    std::string randomVisitorId;

	bool configCookiesDisabled;
    std::string configCookiePath;
    std::string configCookieDomain;

    long long currentTs;
    long long createTs;
    unsigned int visitCount;

    long long currentVisitTs;
    long long lastVisitTs;
    long long ecommerceLastOrderTimestamp;
		
	unsigned int requestTimeout;
    bool doBulkRequests;   

	bool sendImageResponse;	

	CCustomVars pageCustomVars;
	CCustomVars eventCustomVars;
    CCustomVars visitorCustomVars;

	std::vector<std::string> storedTrackingActions;

	std::map<std::string, SEcommerceItem> ecommerceItems;

	std::string DEBUG_APPEND_URL;

	CCookies m_cookies;

	std::string url;

	std::string attributionInfo[MAX_ATTRIBS];
};
