
#pragma once

#define REQUEST_STATUS_REQUEST_NOT_FOUND		-1
#define REQUEST_STATUS_OK						0
#define REQUEST_STATUS_IN_QUEUE					1
#define REQUEST_STATUS_FAILED					2

class IPiwikTracker
{
public:
	virtual bool setPageCharset(const char* const charset = "") = 0;
	virtual bool setUrl(const char* const url) = 0;
	virtual bool setUrlReferrer(const char* const url) = 0;
	virtual void setGenerationTime(const unsigned int timeMs) = 0;
	virtual bool setCustomVariable(const int id, const char* const name, const char* const value, const char* const scope = "visit") = 0;
	virtual bool getCustomVariable(const int id, char*& name, char*& value, const char* const scope = "visit") = 0;
	virtual void clearCustomVariables() = 0;
	virtual void setNewVisitorId() = 0;
	virtual void setIdSite(unsigned int _idSite) = 0;
	virtual bool setBrowserLanguage(const char* const _acceptLanguage) = 0;
	virtual bool setUserAgent(const char* const _userAgent) = 0;
	virtual bool setCountry(const wchar_t* const _country) = 0;
	virtual bool setRegion(const wchar_t* const _region) = 0;
	virtual bool setCity(const wchar_t* const _city) = 0;
	virtual bool setLatitude(const char* const _lat) = 0;
	virtual bool setLongitude(const char* const __long) = 0;
	virtual void enableBulkTracking() = 0;
	virtual bool enableCookies(const char* const path) = 0;
	virtual void disableSendImageResponse() = 0;
	virtual int doTrackPageView(const wchar_t* const documentTitle) = 0;
	virtual int doTrackEvent(const wchar_t* const category, const wchar_t* const action, const wchar_t* const name = NULL, const char* const value = NULL) = 0;
	virtual int doTrackContentImpression(const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL) = 0;
	virtual int doTrackContentInteraction(const wchar_t* const interaction, const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL) = 0;
	virtual int doTrackSiteSearch(const wchar_t* const keyword, const wchar_t* const category = L"", const int countResults = -1) = 0;
	virtual int doTrackGoal(const int idGoal, const float revenue = 0.0f) = 0;
	virtual int doTrackAction(const wchar_t* const actionUrl, const char* const actionType) = 0;
	virtual int doTrackEcommerceOrder(const wchar_t* const orderId, const float grandTotal, const float subTotal = 0.0, const float tax = 0.0,
		const float shipping = 0.0, const float discount = 0.0) = 0;
	virtual bool addEcommerceItem(const char* const sku, const char* const name = "", const char* const category = "", 
		const float price = 0.0f, const int quantity = 1) = 0;
	virtual int doTrackEcommerceCartUpdate(const float grandTotal) = 0;
	virtual int doBulkTrack() = 0;
	virtual char* getUrlTrackPageView(const wchar_t* const documentTitle) = 0;
	virtual char* getUrlTrackEvent(const wchar_t* const category, const wchar_t* const action, const wchar_t* const name = NULL, const char* const value = NULL) = 0;
	virtual char* getUrlTrackContentImpression(const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL) = 0;
	virtual char* getUrlTrackContentInteraction(const wchar_t* const interaction, const wchar_t* const contentName, const wchar_t* const contentPiece = L"Unknown", const wchar_t* const contentTarget = NULL) = 0;
	virtual char* getUrlTrackSiteSearch(const wchar_t* const keyword, const wchar_t* const category = L"", const int countResults = -1) = 0;
	virtual char* getUrlTrackGoal(const int idGoal, const float revenue = 0.0f) = 0;
	virtual char* getUrlTrackAction(const wchar_t* const actionUrl, const char* const actionType) = 0;
	virtual char* getUrlTrackEcommerceOrder(const wchar_t* const orderId, const float grandTotal, const float subTotal = 0.0, 
		const float tax = 0.0, const float shipping = 0.0, const float discount = 0.0) = 0;
	virtual char* getUrlTrackEcommerceCartUpdate(const float grandTotal) = 0;
	virtual bool setForceVisitDateTime(const char* const dateTime) = 0;
	virtual void setForceNewVisit() = 0;
	virtual bool setIp(const char* const _ip) = 0;
	virtual void setUserId(const char* const _userId) = 0;
	virtual char* getVisitorId() = 0;
	virtual char* getUserId()const = 0;
	virtual void deleteCookies() = 0;	
	virtual bool setTokenAuth(const char* const _token_auth) = 0;
	virtual bool setLocalTime(const char* const time) = 0;
	virtual void setResolution(const unsigned int _width, const unsigned int _height) = 0;
	virtual void setBrowserHasCookies(const bool enabled) = 0;
	virtual bool setDebugStringAppend(const char* const str) = 0;
	virtual void setPlugins(const bool flash = false, const bool java = false, const bool director = false, const bool quickTime = false, 
		const bool realPlayer = false, const bool pdf = false, const bool windowsMedia = false, const bool gears = false, 
		const bool silverlight = false) = 0;
	virtual void disableCookieSupport() = 0;
	virtual unsigned int getRequestTimeout()const = 0;
	virtual void setRequestTimeout(const unsigned int timeout) = 0;	
	virtual bool setCustomData(const char* const _customData) = 0;
	virtual char* getCustomData()const = 0;

	virtual void waitForRequest(const unsigned int requestID) = 0;
	virtual int getRequestStatus(const unsigned int requestID) = 0;
	virtual char* getRequestErrorString(const unsigned int requestID) = 0;
	virtual char* getRequestResponse(const unsigned int requestID) = 0;

	virtual ~IPiwikTracker() { };
};