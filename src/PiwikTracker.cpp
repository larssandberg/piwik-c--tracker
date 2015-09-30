
#include "stdafx.h"
#include "PiwikTracker.h"
#include "Utils.h"
#include <time.h>
#include <stdlib.h>
#include <boost/uuid/sha1.hpp>
#include <algorithm>
#include <string.h>
#ifdef __unix__
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

#define DEFAULT_CHARSET_PARAMETER_VALUES	"utf-8"
#define DEFAULT_COOKIE_PATH					"/"
#define LENGTH_VISITOR_ID					16
#define VERSION								"1"

#define CVAR_INDEX_ECOMMERCE_ITEM_PRICE		2
#define CVAR_INDEX_ECOMMERCE_ITEM_SKU		3
#define CVAR_INDEX_ECOMMERCE_ITEM_NAME		4
#define CVAR_INDEX_ECOMMERCE_ITEM_CATEGORY	5

#define FIRST_PARTY_COOKIES_PREFIX			"_pk_"

CPiwikTracker::CPiwikTracker(unsigned int idSite_, std::string apiUrl_)
{	
	forcedDatetime = "";
	forcedNewVisit = "";
	generationTime = false;    
	customData = "";
	hasCookies = -1;
	width = 0;
	height = 0;
	plugins = "";
	localHour = 0xFF;
	localMinute = 0xFF;
	localSecond = 0xFF;

	idSite = idSite_;
	urlReferrer = "";
	pageCharset = DEFAULT_CHARSET_PARAMETER_VALUES;
	pageUrl = "";
	ip = "";
	std::string acceptLanguage = "";    

	apiUrl = apiUrl_;

	configVisitorCookieTimeout = 33955200;         
	configSessionCookieTimeout = 1800;         
	configReferralCookieTimeout = 15768000;

	userId = "";
    
	setNewVisitorId();

	configCookiesDisabled = false;
	configCookiePath = DEFAULT_COOKIE_PATH;
    
	currentTs = GetUnixTimeInMS();
	createTs = currentTs;
	visitCount = 0;
	currentVisitTs = 0;
	lastVisitTs = 0;
	ecommerceLastOrderTimestamp = 0;

	// Allow debug while blocking the request
	requestTimeout = 600;
	doBulkRequests = false;
    
	sendImageResponse = true;

	srand((unsigned int)time(NULL));

	getCustomVariablesFromCookie(visitorCustomVars);
}

bool CPiwikTracker::setCustomData(const char* const _customData)
{
	if (!_customData)
	{
		return false;
	}

	customData = _customData;
	return true;
}

int CPiwikTracker::getIdSite()const
{
	return idSite;
}

char* CPiwikTracker::getCustomData()const
{
	return (char*)customData.c_str();
}

void CPiwikTracker::setNewVisitorId()
{
	std::string guid = GetGUID();
	guid.erase(std::remove(guid.begin(), guid.end(), '-'), guid.end());
	randomVisitorId = guid.substr(0, LENGTH_VISITOR_ID);
	userId = "";
	forcedVisitorId = "";
	cookieVisitorId = "";
}

bool CPiwikTracker::setPageCharset(const char* const charset)
{
	if (!charset)
	{
		return false;
	}
	
	pageCharset = charset;
	return true;
}

bool CPiwikTracker::setUrl(const char* const url)
{
	if (!url)
	{
		return false;
	}
	
	pageUrl = url;
	return true;
}

bool CPiwikTracker::setUrlReferrer(const char* const url)
{
	if (!url)
	{
		return false;
	}

	urlReferrer = url;
	return true;
}

void CPiwikTracker::setGenerationTime(const unsigned int timeMs)
{
	generationTime = timeMs;
}

    
bool CPiwikTracker::setAttributionInfo(const char* const jsonEncoded)
{
	try
	{
		std::stringstream ss;
		ss << "{\"__temp__\":";        
		ss << jsonEncoded;	
		ss << "}";	
 
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);
 
		int i = 0;
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("__temp__"))
		{
			attributionInfo[i] = v.second.data();
			i++;
			if (i == MAX_ATTRIBS)
			{
				return true;
			}
		}        
	}
	catch (...)
	{
	}

	return false;
}

bool CPiwikTracker::setCustomVariable(const int id, const char* const name, const char* const value, const char* const scope)
{
	if ((id < 1) || (id > MAX_SLOTS)) 
	{
		return false;
	}

	if ((!name) || (!value) || (!scope))
	{
		return false;
	}
        
	if (strcmp(scope, "page") == 0) 
	{
		return pageCustomVars.Set(id, name, value);		
	} 
	else if (strcmp(scope, "event") == 0) 
	{
		return eventCustomVars.Set(id, name, value);
    } 
	else if (strcmp(scope, "visit") == 0) 
	{
		return visitorCustomVars.Set(id, name, value);		
    } 
	
	return false;    
}

bool CPiwikTracker::getCustomVariable(const int id, char*& name, char*& value, const char* const scope)
{
	if ((id < 1) || (id > MAX_SLOTS)) 
	{		
		return false;
	}

	if (!scope)
	{
		return false;
	}

	if (strcmp(scope, "page") == 0) 
	{
		return pageCustomVars.Get(id, name, value);		
	} 
	else if (strcmp(scope, "event") == 0)
	{
		return eventCustomVars.Get(id, name, value);	
    } 
	else if (strcmp(scope, "visit") != 0)
	{
		return false;
    }
     
	bool res =  visitorCustomVars.Get(id, name, value);	
	if (res)
	{
		return true;	
	}

	CCustomVars visitorCustomVarFromCookie;
	if (getCustomVariablesFromCookie(visitorCustomVarFromCookie))
	{
		return visitorCustomVarFromCookie.Get(id, name, value);
	}

	return false;		
}

void CPiwikTracker::clearCustomVariables()
{
	visitorCustomVars.Clear();
	eventCustomVars.Clear();
	pageCustomVars.Clear();	
}

void CPiwikTracker::setIdSite(unsigned int _idSite)
{
	idSite = _idSite;
}

bool CPiwikTracker::setBrowserLanguage(const char* const _acceptLanguage)
{
	if (!_acceptLanguage)
	{
		return false;
	}
	
	acceptLanguage = _acceptLanguage;
	m_requestQueue.SetAcceptLanguage(acceptLanguage);
	return true;
}

bool CPiwikTracker::setUserAgent(const char* const _userAgent)
{
	if (!_userAgent)
	{
		return false;
	}
	
	userAgent = _userAgent;
	m_requestQueue.SetUserAgent(userAgent);
	return true;
}

bool CPiwikTracker::setCountry(const wchar_t* const _country)
{
	if (!_country)
	{
		return false;
	}

#ifdef _WIN32
	country.reset(_wcsdup(_country));
#else
	country.reset(wcsdup(_country));
#endif
	return true;
}

bool CPiwikTracker::setRegion(const wchar_t* const _region)
{
	if (!_region)
	{
		return false;
	}

#ifdef _WIN32
	region.reset(_wcsdup(_region));
#else
	region.reset(wcsdup(_region));
#endif
	return true;
}

bool CPiwikTracker::setCity(const wchar_t* const _city)
{
	if (!_city)
	{
		return false;
	}

#ifdef _WIN32	
	city.reset(_wcsdup(_city));
#else
	city.reset(wcsdup(_city));
#endif
	return true;
}
	    
bool CPiwikTracker::setLatitude(const char* const _lat)
{
	if (!_lat)
	{
		return false;
	}
	
	lat = _lat;
	return true;
}
    
bool CPiwikTracker::setLongitude(const char* const __long)
{
	if (!__long)
	{
		return false;
	}

	_long = __long;
	return true;
}
    
void CPiwikTracker::enableBulkTracking()
{
	// not implemented yet
	// doBulkRequests = true;
}

bool CPiwikTracker::enableCookies(const char* const path)
{
	if (path)
	{
		configCookiesDisabled = false;
		configCookiePath = path;
		
		return m_cookies.SetFileName(configCookiePath);
	}
	else
	{
		return false;
	}
}

void CPiwikTracker::disableSendImageResponse()
{
	sendImageResponse = false;
}
    
std::string CPiwikTracker::getCookieName(std::string cookieName) 
{
    // NOTE: If the cookie name is changed, we must also update the method in piwik.js with the same name.
	std::string hash = GetSHA1(getCurrentHost()  + configCookiePath);
	hash = hash.substr(0, 4);

	return FIRST_PARTY_COOKIES_PREFIX + cookieName + "." + IntToStr(idSite) + "." + hash;
}

int CPiwikTracker::doTrackPageView(const wchar_t* const documentTitle)
{
	std::string url = getUrlTrackPageView(documentTitle);
	return sendRequest(url);	
}
    
int CPiwikTracker::doTrackEvent(const wchar_t* const category, const wchar_t* const action, const wchar_t* const name, const char* const value)
{
	if ((category) && (wcslen(category) > 0) && (action) && (wcslen(action) > 0))
	{
		std::string url = getUrlTrackEvent(category, action, name, value);
		return sendRequest(url);
	}
	else
	{
		return 0;
	}
}

int CPiwikTracker::doTrackContentImpression(const wchar_t* const contentName, const wchar_t* const contentPiece, const wchar_t* const contentTarget)
{
	if ((contentName) && (wcslen(contentName) > 0))
	{
		std::string url = getUrlTrackContentImpression(contentName, contentPiece, contentTarget);
		return sendRequest(url);
	}
	else
	{
		return 0;
	}
}

int CPiwikTracker::doTrackContentInteraction(const wchar_t* const interaction, const wchar_t* const contentName, const wchar_t* const contentPiece, const wchar_t* const contentTarget)
{
	if ((interaction) && (wcslen(interaction) > 0) && (contentName) && (wcslen(contentName) > 0))
	{
		std::string url = getUrlTrackContentInteraction(interaction, contentName, contentPiece, contentTarget);
		return sendRequest(url);
	}
	else
	{
		return 0;
	}
}

int CPiwikTracker::doTrackSiteSearch(const wchar_t* const keyword, const wchar_t* const category, const int countResults)
{
	if ((keyword != NULL) && (wcslen(keyword) > 0))
	{
		std::string url = getUrlTrackSiteSearch(keyword, category, countResults);
		return sendRequest(url);
	}
	else
	{
		return 0;
	}
}

int CPiwikTracker::doTrackGoal(const int idGoal, const float revenue)
{
	std::string url = getUrlTrackGoal(idGoal, revenue);
	return sendRequest(url);
}

int CPiwikTracker::doTrackAction(const wchar_t* const actionUrl, const char* const actionType)
{
	if ((actionUrl) && (wcslen(actionUrl) > 0) && (actionType) && (strlen(actionType) > 0))
	{
		// Referrer could be udpated to be the current URL temporarily (to mimic JS behavior)
		std::string url = getUrlTrackAction(actionUrl, actionType);
		return sendRequest(url);
	}
	else
	{
		return 0;
	}
}

bool CPiwikTracker::addEcommerceItem(const char* const sku, const char* const name, const char* const category, 
	const float price, const int quantity)
{
	if (sku == NULL) 
	{
		return false;
	}	

	SEcommerceItem item(sku, name ? name : "", category ? category : "", price, quantity);
	ecommerceItems.insert(std::map<std::string, SEcommerceItem>::value_type(sku, item));		
	return true;
}

int CPiwikTracker::doTrackEcommerceCartUpdate(const float grandTotal)
{
	std::string url = getUrlTrackEcommerceCartUpdate(grandTotal);
	return sendRequest(url);
}

int CPiwikTracker::doBulkTrack()
{
	// not implemented yet
	return 0;
}

int CPiwikTracker::doTrackEcommerceOrder(const wchar_t* const orderId, const float grandTotal, const float subTotal, 
	const float tax, const float shipping, const float discount)
{
	if (orderId != NULL)
	{
		std::string url = getUrlTrackEcommerceOrder(orderId, grandTotal, subTotal, tax, shipping, discount);
		return sendRequest(url);
	}
	else
	{
		return 0;
	}
}

bool CPiwikTracker::setEcommerceView(const char* const sku, const char* const name, const char** const categories, 
	const int categoryCount, const float price)
{
	if ((categoryCount > 0) && (*categories))
	{
		std::string category = StringArrayToJSON(categories, categoryCount);
		return setEcommerceView(sku, name, (char*)category.c_str(), price);		
	}
	else
	{
		return false;
	}
}

bool CPiwikTracker::setEcommerceView(const char* const sku, const char* const name, const char* const category, const float price)
{
	// On a category page, do not record "Product name not defined"
	if ( ((sku == NULL) || (strlen(sku) == 0)) && ((name == NULL) || (strlen(name) == 0)))
	{
		return false;
	}

	pageCustomVars.Set(CVAR_INDEX_ECOMMERCE_ITEM_CATEGORY, "_pkc", category ? category : "");
	pageCustomVars.Set(CVAR_INDEX_ECOMMERCE_ITEM_PRICE, "_pkp", FloatToStr(price));
    pageCustomVars.Set(CVAR_INDEX_ECOMMERCE_ITEM_SKU, "_pks", sku);
    pageCustomVars.Set(CVAR_INDEX_ECOMMERCE_ITEM_NAME, "_pkn", name);

	return true;
}

bool CPiwikTracker::setForceVisitDateTime(const char* const dateTime)
{
	if (!dateTime)
	{
		return false;
	}
	
	if (GetUnixTimeInMS(dateTime) == 0)
	{
		return false;
	}
	
	forcedDatetime = dateTime;
	return true;
}

void CPiwikTracker::setForceNewVisit()
{
	forcedNewVisit = true;
}

bool CPiwikTracker::setIp(const char* const _ip)
{
	if (!_ip)
	{
		return false;
	}
	
	ip = _ip;
	return true;
}

void CPiwikTracker::setUserId(const char* const _userId)
{
	if ((_userId == NULL) || (strlen(_userId) == 0))
	{
		setNewVisitorId();
		return;
	}

    userId = _userId;
}

std::string CPiwikTracker::getUserIdHashed(std::string id)
{	
	return GetSHA1(id).substr(0, 16);
}

/*CPiwikTracker::setVisitorId($visitorId)
{
	// !!!
	$hexChars = '01234567890abcdefABCDEF';
	if (strlen($visitorId) != self::LENGTH_VISITOR_ID
            || strspn($visitorId, $hexChars) !== strlen($visitorId)) 
	{
            throw new Exception("setVisitorId() expects a "
                . self::LENGTH_VISITOR_ID
                . " characters hexadecimal string (containing only the following: "
                . $hexChars
                . ")");
        }
        $this->forcedVisitorId = $visitorId;
    }*/

char* CPiwikTracker::getVisitorId()
{
	if (userId.size() > 0) 
	{
		return (char*)getUserIdHashed(userId.c_str()).c_str();
	}
	if (loadVisitorIdCookie()) 
	{
		return (char*)cookieVisitorId.c_str();
	}
	return (char*)randomVisitorId.c_str();
}

char* CPiwikTracker::getUserId()const
{
	return (char*)userId.c_str();
}

char* CPiwikTracker::getUrlTrackPageView(const wchar_t* const documentTitle)
{
	url = getRequest(idSite);

	if ((documentTitle) && (wcslen(documentTitle) > 0))
	{
		url += "&action_name=";
		url += UrlEncodeW(documentTitle);
    }
	return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackEvent(const wchar_t* const category, const wchar_t* const action, const wchar_t* const name, 
	const char* const value)
{
	url = getRequest(idSite);
    if ((category == NULL) || (wcslen(category) == 0))
	{
		return NULL;
    }
	if ((action == NULL) || (wcslen(action) == 0))
	{
		return NULL;
	}

	url += "&e_c=" + UrlEncodeW(category);
	url += "&e_a=" + UrlEncodeW(action);

	if ((name != NULL) && (wcslen(name) > 0))
	{
		url += "&e_n=" + UrlEncodeW(name);
	}
	if ((value != NULL) && (strlen(value) > 0))
	{
		std::string valueStr = value;
		valueStr = forceDotAsSeparatorForDecimalPoint((char*)valueStr.c_str());
		url += "&e_v=" + valueStr;
	}
	return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackContentImpression(const wchar_t* const contentName, const wchar_t* const contentPiece, 
	const wchar_t* const contentTarget)
{
	url = getRequest(idSite);

	if ((!contentName) || (wcslen(contentName) == 0))
	{
		return NULL;
	}

	url += "&c_n=" + UrlEncodeW(contentName);

	if ((contentPiece != NULL) && (wcslen(contentPiece) > 0)) 
	{
		url += "&c_p=" + UrlEncodeW(contentPiece);
	}
	if ((contentTarget != NULL) && (wcslen(contentTarget) > 0))
	{
		url += "&c_t=" + UrlEncodeW(contentTarget);
	}

	return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackContentInteraction(const wchar_t* const interaction, const wchar_t* const contentName, 
	const wchar_t* const contentPiece, const wchar_t* const contentTarget)
{
	url = getRequest(idSite);

	if ((interaction == NULL) || (wcslen(interaction) == 0))
	{
		return NULL;
	}

	if ((contentName == NULL) || (wcslen(contentName) == 0))
	{
		return NULL;
	}

	url += "&c_i=" + UrlEncodeW(interaction);
	url += "&c_n=" + UrlEncodeW(contentName);

	if ((contentPiece != NULL) && (wcslen(contentPiece) > 0))
	{
		url += "&c_p=" + UrlEncodeW(contentPiece);
	}
	if ((contentTarget != NULL) && (wcslen(contentTarget) > 0))
	{
		url += "&c_t=" + UrlEncodeW(contentTarget);
	}

	return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackSiteSearch(const wchar_t* const keyword, const wchar_t* const category, const int countResults)
{
	if ((keyword == NULL) || (wcslen(keyword) == 0))
	{
		return NULL;
	}

	url = getRequest(idSite);
	url += "&search=" + UrlEncodeW(keyword);
	if ((category != NULL) && (wcslen(category) > 0))
	{
		url += "&search_cat=" + UrlEncodeW(category);
	}
	if (countResults >= 0) 
	{
		url += "&search_count=" + IntToStr(countResults);
	}
	return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackGoal(int idGoal, float revenue)
{
	url = getRequest(idSite);
	url += "&idgoal=" + IntToStr(idGoal);
	if (revenue != 0.0f) 
	{
		std::string revenueStr = FloatToStr(revenue);
		revenueStr = forceDotAsSeparatorForDecimalPoint((char *)revenueStr.c_str());
		url += "&revenue=" + revenueStr;
	}
	return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackAction(const wchar_t* const actionUrl, const char* const actionType)
{
	if ((actionUrl == NULL) || (wcslen(actionUrl) == 0))
	{
		return NULL;
	}

	if ((actionType == NULL) || (strlen(actionType) == 0))
	{
		return NULL;
	}

	if (strcmp(actionType, "download") != 0)
	if (strcmp(actionType, "link") != 0)
	{
		return NULL;
	}

	url = getRequest(idSite);
	url += "&";
	url += actionType;
	url += '=';
	url += UrlEncodeW(actionUrl);
    return (char*)url.c_str();
}

char* CPiwikTracker::getUrlTrackEcommerceCartUpdate(const float grandTotal)
{
	url = getUrlTrackEcommerce(grandTotal);
	return (char *)url.c_str();
}

char* CPiwikTracker::getUrlTrackEcommerceOrder(const wchar_t* const orderId, const float grandTotal, const float subTotal, 
	const float tax, const float shipping, const float discount)
{
	if ((!orderId) || (wcslen(orderId) == 0))
	{
		return NULL;
	}
	url = getUrlTrackEcommerce(grandTotal, subTotal, tax, shipping, discount);
	url += "&ec_id=" + UrlEncodeW(orderId);
	ecommerceLastOrderTimestamp = getTimestamp();
	return (char *)url.c_str();
}

char* CPiwikTracker::getUrlTrackEcommerce(float grandTotal, float subTotal, float tax, float shipping, float discount)
{
	url = getRequest(idSite);
	url += "&idgoal=0";

	std::string grandTotalStr = forceDotAsSeparatorForDecimalPoint((char*)FloatToStr(grandTotal).c_str());
	url += "&revenue=" + grandTotalStr;

	if (subTotal != 0.0f)
	{
		std::string subTotalStr = forceDotAsSeparatorForDecimalPoint((char*)FloatToStr(subTotal).c_str());
		url += "&ec_st=" + subTotalStr;
	}

	if (tax != 0.0f) 
	{
		std::string taxStr = forceDotAsSeparatorForDecimalPoint((char*)FloatToStr(tax).c_str());
		url += "&ec_tx=" + taxStr;
	}

	if (shipping != 0.0f) 
	{
		std::string shippingStr = forceDotAsSeparatorForDecimalPoint((char*)FloatToStr(shipping).c_str());
		url += "&ec_sh=" + shippingStr;
	}

	if (discount != 0.0f) 
	{
		std::string discountStr = forceDotAsSeparatorForDecimalPoint((char*)FloatToStr(discount).c_str());
		url += "&ec_dt=" + discountStr;
	}  
     
	if (ecommerceItems.size() > 0) 
	{
		// Removing the SKU index in the array before JSON encoding

		std::vector<SEcommerceItem> pureItems;
		std::map<std::string, SEcommerceItem>::iterator it = ecommerceItems.begin();
		std::map<std::string, SEcommerceItem>::iterator itEnd = ecommerceItems.end();

		for ( ; it != itEnd; it++)
		{
			pureItems.push_back((*it).second);
		}

		url += "&ec_items=" + UrlEncode(SEcommerceItem::ArrayToJSON(pureItems));
    }

    ecommerceItems.clear();        
    return (char*)url.c_str();
}

std::string CPiwikTracker::forceDotAsSeparatorForDecimalPoint(const char* const value)
{
	if (value)
	{
		return ForceDotAsSeparatorForDecimalPoint(value);
	}
	else
	{
		return "";
	}
}

std::string CPiwikTracker::getBaseUrl()
{
	if (apiUrl.size() == 0) 
	{
		return "";
	}
	if ((apiUrl.find("/piwik.php") == std::string::npos) && (apiUrl.find("/proxy-piwik.php") == std::string::npos)) 
	{
		apiUrl += "/piwik.php";
	}
	return apiUrl;
}

std::string CPiwikTracker::getRequest(unsigned int idSite)
{
	setFirstPartyCookies();

    std::string url = getBaseUrl();
    url += "?idsite=" + IntToStr(idSite);
	url += "&rec=1";
	url += "&apiv=";
	url += VERSION;
	url += "&r=" + IntToStr((int)(rand() * 32000.0f)%899999 + 100000);

	// Only allowed for Super User, token_auth required,
    url += (ip.size() > 0 ? "&cip=" + ip : "");
	url += (userId.size() > 0 ? "&uid=" + UrlEncode(userId) : "");
	url += (forcedDatetime.size() > 0 ? "&cdt=" + UrlEncode(forcedDatetime) : "");
	url += (forcedNewVisit.size() > 0 ? "&new_visit=1" : "");
	url += (((token_auth.size() > 0) && (!doBulkRequests)) ? "&token_auth=" + UrlEncode(token_auth) : "");
   
	url += "&_idts=" + Int64ToStr(createTs);
	url += "&_idvc="  + IntToStr(visitCount);
	url += (lastVisitTs != 0) ? "&_viewts=" + Int64ToStr(lastVisitTs) : "";
    url += (ecommerceLastOrderTimestamp != 0) ? "&_ects=" + UrlEncode(Int64ToStr(ecommerceLastOrderTimestamp)) : "";

	// These parameters are set by the JS, but optional when using API
	url += (plugins.size() > 0) ? plugins : "";
	url += (localHour != 0xFF && localMinute != 0xFF && localSecond != 0xFF) ? "&h=" + IntToStr(localHour) + "&m=" + IntToStr(localMinute) + "&s=" + IntToStr(localSecond) : "";
	url += ((width > 0) && (height > 0)) ? "&res=" + IntToStr(width) + "x" + IntToStr(height) : "";

	if (hasCookies != -1)
	{
		std::string str = "&cookie=";
		str += ((hasCookies == 1) ? "true" : "false");
		url += str;
	}

	url += (customData.size() > 0) ? "&data=" + UrlEncode(customData) : "";
	if (!visitorCustomVars.IsEmptyArray())
	{
		std::string json = visitorCustomVars.ToJSON();
		url += "&_cvar=" + UrlEncode(json);
	}
	if (!pageCustomVars.IsEmptyArray())
	{
		std::string json = pageCustomVars.ToJSON();
		url += "&cvar=" + UrlEncode(json);
	}
	if (!eventCustomVars.IsEmptyArray())
	{
		std::string json = eventCustomVars.ToJSON();
		url += "&e_cvar=" + UrlEncode(json);
	}
    url += (generationTime != -1) ? "&gt_ms=" + IntToStr(generationTime) : "";
	if (forcedVisitorId.size() > 0) 
	{
		url += "&cid=" + forcedVisitorId;
	}
	else
	{
		url += "&_id=";
		url += getVisitorId();
	}

	// URL parameters
	url += "&url=" + UrlEncode(pageUrl);
	url += "&urlref=" + UrlEncode(urlReferrer);
	url += (((pageCharset.size() > 0) && (pageCharset != DEFAULT_CHARSET_PARAMETER_VALUES)) ? "&cs=" + pageCharset : "");

	// Attribution information, so that Goal conversions are attributed to the right referrer or campaign
	// Campaign name
	url += (attributionInfo[0].size() > 0) ? "&_rcn=" + UrlEncode(attributionInfo[0]) : "";
    // Campaign keyword
	url += (attributionInfo[1].size() > 0) ? "&_rck=" + UrlEncode(attributionInfo[1]) : "";
	// Timestamp at which the referrer was set
	url += (attributionInfo[2].size() > 0) ? "&_refts=" + UrlEncode(attributionInfo[2]) : "";
	// Referrer URL
	url += (attributionInfo[3].size() > 0) ? "&_ref=" + UrlEncode(attributionInfo[3]) : "";

	// custom location info
	url += ((country.get() != NULL) ? "&country=" + UrlEncodeW(country.get()) : "");
    url += ((region.get() != NULL) ? "&region=" + UrlEncodeW(region.get()) : "");
    url += ((city.get() != NULL) ? "&city=" + UrlEncodeW(city.get()) : "");
    url += ((lat.size() > 0) ? "&lat=" + UrlEncode(lat) : "");
    url += ((_long.size() > 0) ? "&long=" + UrlEncode(_long) : "");
    url += (!sendImageResponse ? "&send_image=0" : "");

	if (DEBUG_APPEND_URL.size() > 0)
	{
		url += DEBUG_APPEND_URL;
	}
	            
    // Reset page level custom variables after this page view
	eventCustomVars.Clear();
	pageCustomVars.Clear();
	
	// force new visit only once, user must call again setForceNewVisit()
	forcedNewVisit = "";

	return url;
}

int CPiwikTracker::sendRequest(std::string url, std::string method, void* data, bool force)
{
    // if doing a bulk request, store the url
    if (doBulkRequests && !force) 
	{
		url += (userAgent.size() > 0 ? ("&ua=" + UrlEncode(userAgent)) : "");
        url += (acceptLanguage.size() ? ("&lang=" + UrlEncode(acceptLanguage)) : "");

		storedTrackingActions.push_back(url);

        // Clear custom variables so they don't get copied over to other users in the bulk request
        clearCustomVariables();
        userAgent = "";
        acceptLanguage = "";
		return -1;
	}

	return m_requestQueue.AddRequest(url);              
}

long long CPiwikTracker::getTimestamp()const
{
	if (forcedDatetime.size() == 0)
		return GetUnixTimeInMS();
        
	return GetUnixTimeInMS(forcedDatetime);
}

bool CPiwikTracker::loadVisitorIdCookie()
{
	std::string idCookie = getCookieMatchingName("id");
	if (idCookie == "") 
	{
		return false;
	}

	std::vector<std::string> parts = Explode(idCookie, '.');
	if (parts[0].size() != LENGTH_VISITOR_ID) 
	{
		return false;
	}

	bool res;
	cookieVisitorId = parts[0]; // provides backward compatibility since getVisitorId() didn't change any existing VisitorId value
	createTs = StrToInt64(parts[1], res);
	if (!res) 
	{
		return false;
	}
	visitCount = StrToInt(parts[2], res);
	if (!res) 
	{
		return false;
	}
	currentVisitTs = StrToInt64(parts[3], res);
	if (!res) 
	{
		return false;
	}
	lastVisitTs = StrToInt64(parts[4], res);
	if (!res) 
	{
		return false;
	}
	if ((parts.size() > 5) && (parts[5].size() > 0)) 
	{
		ecommerceLastOrderTimestamp = StrToInt64(parts[5], res);
		if (!res) 
		{
			return false;
		}
	}
	return true;
}

std::string CPiwikTracker::getCurrentHost()
{
	return "unknown";
}

void CPiwikTracker::deleteCookies()
{
	m_cookies.DeleteAll();      
}

bool CPiwikTracker::setTokenAuth(const char* const _token_auth)
{
	if (!_token_auth)
	{
		return false;
	}

	token_auth = _token_auth;
	return true;
}

bool CPiwikTracker::setLocalTime(const char* const time)
{
	if (!time)
	{
		return false;
	}

	std::string timeStr = time;
	int posDelim1 = timeStr.find_first_of(":");    
	int posDelim2 = timeStr.find_last_of(":"); 
	if ((posDelim1 > 0) && (posDelim2 > 0) && (posDelim1 != posDelim2) && (posDelim2 + 1 < (int)timeStr.size()))
	{
		bool res1, res2, res3;
		int _localHour = StrToInt(timeStr.substr(0, posDelim1), res1);
		int _localMinute = StrToInt(timeStr.substr(posDelim1 + 1, posDelim2 - posDelim1 - 1), res2);
		int _localSecond = StrToInt(timeStr.substr(posDelim2 + 1, timeStr.size() - posDelim2 - 1), res3);
		if ((_localHour >= 0) && (_localHour <= 23) && (_localMinute >= 0) && (_localMinute <= 59) 
			&& (_localSecond >= 0) && (_localSecond <= 59) && (res1) && (res2) && (res3))
		{
			localHour = _localHour;
			localMinute = _localMinute;
			localSecond = _localSecond;
			return true;
		}		
	}

	return false;
}

void CPiwikTracker::setResolution(const unsigned int _width, const unsigned int _height)
{	
	width = _width;
	height = _height;
}

    
void CPiwikTracker::setBrowserHasCookies(const bool enabled)
{
	hasCookies = enabled;
}

bool CPiwikTracker::setDebugStringAppend(const char* const str)
{
	if (!str)
	{
		return false;
	}

	DEBUG_APPEND_URL = "&";
	DEBUG_APPEND_URL += str;
	return true;
}

void CPiwikTracker::setPlugins(const bool flash, const bool java, const bool director, const bool quickTime, 
	const bool realPlayer, const bool pdf, const bool windowsMedia, const bool gears, const bool silverlight)
{
	plugins =
		"&fla=" + IntToStr(flash ? 1 : 0) +
		"&java=" + IntToStr(java ? 1 : 0) +
		"&dir=" + IntToStr(director ? 1 : 0) +
		"&qt=" + IntToStr(quickTime ? 1 : 0) +
		"&realp=" + IntToStr(realPlayer ? 1 : 0) +
		"&pdf=" + IntToStr(pdf ? 1 : 0) +
		"&wma=" + IntToStr(windowsMedia ? 1 : 0) +
		"&gears=" + IntToStr(gears ? 1 : 0) +
		"&ag=" + IntToStr(silverlight ? 1 : 0);
}


void CPiwikTracker::disableCookieSupport()
{
	configCookiesDisabled = true;
}

unsigned int CPiwikTracker::getRequestTimeout()const 
{
	return requestTimeout;
}

void CPiwikTracker::setRequestTimeout(const unsigned int timeout)
{
	requestTimeout = timeout;
	m_requestQueue.SetRequestTimeout(timeout);
}
    
std::string CPiwikTracker::getCookieMatchingName(std::string name)
{
	if (configCookiesDisabled) 
	{
		return "";
	}
        
	name = getCookieName(name);
	for (unsigned int i = 0; i < name.size(); i++)
	if (name[i] == '.')
		name[i] = '_';

	std::string value;
	if (m_cookies.GetValueByNameMask(name, value))
	{
		return value;
	}
	else
	{
		return "";
	}
}

void CPiwikTracker::setFirstPartyCookies()
{
	if (configCookiesDisabled) 
	{
		return;
	}

	if (cookieVisitorId.size() == 0) 
	{
		loadVisitorIdCookie();
	}

	// Set the 'ses' cookie
    setCookie("ses", "*", configSessionCookieTimeout);

	// Set the 'id' cookie
	visitCount = visitCount + 1;
	std::string cookieValue = getVisitorId();
	cookieValue += ".";
	cookieValue += Int64ToStr(createTs);
	cookieValue += ".";
	cookieValue += IntToStr(visitCount);
	cookieValue += ".";
	cookieValue += Int64ToStr(currentTs);
	cookieValue += ".";
	cookieValue += Int64ToStr(lastVisitTs);
	cookieValue += ".";
	cookieValue += Int64ToStr(ecommerceLastOrderTimestamp);
    setCookie("id", cookieValue, configVisitorCookieTimeout);

// Set the 'cvar' cookie
	setCookie("cvar", visitorCustomVars.ToJSON(), configSessionCookieTimeout);
}

void CPiwikTracker::setCookie(std::string cookieName, std::string cookieValue, long long cookieTTL)
{
	long long cookieExpire = currentTs + cookieTTL;

	m_cookies.Set(getCookieName(cookieName), cookieValue, cookieExpire);
}

bool CPiwikTracker::getCustomVariablesFromCookie(CCustomVars& vars)
{
	std::string cookieValue = getCookieMatchingName("cvar");
	if (cookieValue.size() == 0) 
	{
		return false;
	}
	
	return vars.FromJSON(cookieValue);
}

void CPiwikTracker::waitForRequest(const unsigned int requestID)
{
	int status = m_requestQueue.GetRequestStatus(requestID);
	if (status == REQUEST_STATUS_REQUEST_NOT_FOUND)
	{
		return;
	}

	while ((status != REQUEST_STATUS_OK) && (status != REQUEST_STATUS_FAILED))
	{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			status = m_requestQueue.GetRequestStatus(requestID);
	}
}

int CPiwikTracker::getRequestStatus(const unsigned int requestID)
{
	return m_requestQueue.GetRequestStatus(requestID);
}

char* CPiwikTracker::getRequestErrorString(const unsigned int requestID)
{
	return m_requestQueue.GetRequestErrorString(requestID);
}

char* CPiwikTracker::getRequestResponse(const unsigned int requestID)
{
	return m_requestQueue.GetRequestResponse(requestID);
}
	
CPiwikTracker::~CPiwikTracker()
{      
}
