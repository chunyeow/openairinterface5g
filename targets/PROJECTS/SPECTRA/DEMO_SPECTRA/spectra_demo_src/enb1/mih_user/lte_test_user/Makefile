main: main.cpp 
	g++-4.8  -Wall main.cpp   -I /opt/casablanca/Release/include/ -L/usr/local/lib -L/opt/casablanca/Binaries/Release32/ -L. -std=c++11 -lcrmclient -lpthread  -lboost_system -lboost_thread -lcasablanca -lboost_filesystem -o CRMClientmain

lib:  CRMClient.cpp

# 64 bits compilation
# 	g++-4.8  -Wall -fPIC -c CRMClient.cpp  -I /opt/casablanca/Release/include/ -L/usr/local/lib -L/opt/casablanca/Binaries/Release32/ -std=c++11 -m64 -lpthread  -lboost_system -lboost_thread -lcasablanca -lboost_filesystem
# 	g++-4.8 -shared -m64 -Wl,-soname,libcrmclient.so.1 -o libcrmclient.so.1.0 CRMClient.o
# 	ln -sf libcrmclient.so.1.0 libcrmclient.so
# 	ln -sf libcrmclient.so.1.0 libcrmclient.so.1

# 32 bits compilation
	g++-4.8  -Wall -fPIC -c CRMClient.cpp  -I /opt/casablanca/Release/include/ -L/usr/local/lib -L/opt/casablanca/Binaries/Release32/ -std=c++11  -lpthread  -lboost_system -lboost_thread -lcasablanca -lboost_filesystem
	g++-4.8 -shared  -Wl,-soname,libcrmclient.so.1 -o libcrmclient.so.1.0 CRMClient.o
	ln -sf libcrmclient.so.1.0 libcrmclient.so
	ln -sf libcrmclient.so.1.0 libcrmclient.so.1
	
clean: 
	rm *.o *.so*
