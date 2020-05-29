CPP_VERSION = -std=c++17
INCLUDE_GCH = -include client/PCHClient.h

server_src = server/app.cpp server/Handlers.cpp utils/RFCprotocol.cpp server/Hub.cpp server/Connection.cpp server/MessageSendController.cpp

client_src = client/app.cpp client/Client.cpp utils/RFCprotocol.cpp client/Handlers.cpp client/Executors.cpp client/ui/Screen.cpp utils/StrManip.cpp
client_obj = $(patsubst %.cpp,%.o,$(client_src))

%.o: %.cpp
	g++ -c -o $@ $(INCLUDE_GCH) $< `pkg-config gtkmm-3.0 --cflags` $(CPP_VERSION) 

all: client/PCHClient.h.gch $(client_obj) $(server_src)
	g++ -o bin/app $(client_obj) `pkg-config gtkmm-3.0 --libs` $(CPP_VERSION)
	g++ -o bin/hub $(server_src) -pthread $(CPP_VERSION) -pthread

client/PCHClient.h.gch: client/PCHClient.h
	g++ -c -o $@ $< `pkg-config gtkmm-3.0 --cflags` $(CPP_VERSION)
	
client: bin/app
	./bin/app
	
server: bin/hub
	./bin/hub