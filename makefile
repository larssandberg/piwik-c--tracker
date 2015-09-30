COMPILE_PARAMS = -I src/ -I /usr/include/boost/ -I /usr/include/curl/ -L /usr/lib/i386-linux-gnu -l boost_unit_test_framework -l boost_date_time -l boost_thread -l boost_system -l curl -pthread
CC = g++

all: piwikstatic piwikstaticexample piwikdynamic piwikdynamicexample piwiktests

piwikstatic: 
	$(CC) -o Cookies.o -c src/Cookies.cpp $(COMPILE_PARAMS)
	$(CC) -o CURLHttpRequest.o -c src/CURLHttpRequest.cpp $(COMPILE_PARAMS)
	$(CC) -o CustomVar.o -c src/CustomVar.cpp $(COMPILE_PARAMS)
	$(CC) -o EcommerceItem.o -c src/EcommerceItem.cpp $(COMPILE_PARAMS)
	$(CC) -o HttpRequestQueue.o -c src/HttpRequestQueue.cpp $(COMPILE_PARAMS)
	$(CC) -o PiwikTracker.o -c src/PiwikTracker.cpp $(COMPILE_PARAMS)
	$(CC) -o Utils.o -c src/Utils.cpp $(COMPILE_PARAMS)
	$(CC) -o PiwikCreator.o -c src/PiwikCreator.cpp $(COMPILE_PARAMS)
	ar rc libpiwikstatic.a Cookies.o CURLHttpRequest.o CustomVar.o EcommerceItem.o HttpRequestQueue.o PiwikTracker.o PiwikCreator.o Utils.o

piwikstaticexample:
	$(CC) -o piwikstaticexample examples/Linux/static_example.cpp libpiwikstatic.a $(COMPILE_PARAMS)

piwikdynamic: 
	$(CC) -fPIC -o Cookies.o -c src/Cookies.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o CURLHttpRequest.o -c src/CURLHttpRequest.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o CustomVar.o -c src/CustomVar.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o EcommerceItem.o -c src/EcommerceItem.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o HttpRequestQueue.o -c src/HttpRequestQueue.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o PiwikTracker.o -c src/PiwikTracker.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o Utils.o -c src/Utils.cpp $(COMPILE_PARAMS)
	$(CC) -fPIC -o PiwikCreator.o -c src/PiwikCreator.cpp $(COMPILE_PARAMS)
	$(CC) -shared -o libpiwikdynamic.so Cookies.o CURLHttpRequest.o CustomVar.o EcommerceItem.o HttpRequestQueue.o PiwikTracker.o  PiwikCreator.o Utils.o $(COMPILE_PARAMS)

piwikdynamicexample:
	$(CC) -o piwikdynamicexample examples/Linux/dynamic_example.cpp -ldl

piwiktests: 
	$(CC) -o piwiktests tests/PiwikTrackerTests.cpp src/Cookies.cpp src/CURLHttpRequest.cpp src/CustomVar.cpp src/EcommerceItem.cpp src/HttpRequestQueue.cpp src/PiwikTracker.cpp  src/Utils.cpp  -I src/ -I PiwikTests/ -I /usr/include/boost/ -I /usr/include/curl/ -L /usr/lib/i386-linux-gnu -l boost_unit_test_framework -l boost_date_time -l boost_thread -l boost_system -l curl -pthread

clean:
	rm *.o piwiktests piwikstaticexample piwikdynamicexample libpiwikdynamic.so libpiwikstatic.a
