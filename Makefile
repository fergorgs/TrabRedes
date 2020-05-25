CPP_VERSION = -std=c++11
INCLUDE_GCH = -include client/PCHClient.h

sources = client/app.cpp client/Client.cpp utils/RFCprotocol.cpp client/Handlers.cpp client/Executors.cpp client/ui/Screen.cpp utils/StrManip.cpp
objects = $(patsubst %.cpp,%.o,$(sources))

%.o: %.cpp
	g++ -c -o $@ $(INCLUDES) $(INCLUDE_GCH) $< `pkg-config gtkmm-3.0 --cflags` $(CPP_VERSION)

all: client/PCHClient.h.gch $(objects)
	g++ -o bin/app $(objects) `pkg-config gtkmm-3.0 --libs` $(CPP_VERSION)
	g++ -o bin/hub server/hub.cpp $(version)

client/PCHClient.h.gch: client/PCHClient.h
	g++ -c -o $@ $< `pkg-config gtkmm-3.0 --cflags` $(CPP_VERSION)
	
client: bin/app
	./bin/app
	
server: bin/hub
	./bin/hub