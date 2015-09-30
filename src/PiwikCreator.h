
#pragma once

#include "IPiwikTracker.h"

extern "C" IPiwikTracker* createPiwikTracker(unsigned int idSite, char* apiUrl);
extern "C" void deletePiwikTracker(IPiwikTracker* piwikTracker);
