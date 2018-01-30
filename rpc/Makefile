LIBS = -lomnithread -lomniORB4 
STUBS = chatSK.cc 
STUB_HEADERS = chat.hh

all: server client

$(STUBS): ./chat.idl
	omniidl -bcxx ./chat.idl

server: chat_server.cc $(STUBS)
	g++ -o server $(LIBS) chat_server.cc $(STUBS)

client: chat_client.cc $(STUBS)
	g++ -o client $(LIBS) chat_client.cc $(STUBS)

clean:
	rm -f client server $(STUBS) $(STUB_HEADERS)
