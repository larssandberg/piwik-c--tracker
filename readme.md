# C++ Tracking API for gameinsight.io and appinsight.io #
The services of gameinsight.io, appinsight.io and shopinsight.io are based on Piwik. Hence, every API that works with Piwik, also works with gameinsight.io and appinsight.io. In order to track Games that are written in C++ with Piwik, we implemented our own C++ API.


## LINUX ##

Execute "make all" to compile library, tests and examples. Latest version of
Boost and CURL librars must be installed.

## LINUX DEBIAN ##

On Debian Squeeze the package requires
    	g++
        libboost-system-dev
        libboost-thread-dev
        libboost-date-time-dev
        libboost-test-dev
        libboost-dev
        libcurl4-openssl-dev

## WINDOWS ##

1. Download CURL library to "3party/curl" folder.
2. Download Boost library with precompiled binaries to "3party/boost" folder.
http://sourceforge.net/projects/boost/files/boost-binaries/
3. Open solution for your Visual Studio version and build all.
