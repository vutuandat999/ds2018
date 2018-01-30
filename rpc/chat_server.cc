#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define LENGTH 512

#include "chat.hh"
#include <omniconfig.h>			// platform-specific preprocessor variables

#include <iostream> 
using namespace std;

class Server : public POA_RPC::Chat
{
public:
  inline Server() {}
  virtual ~Server() {}
  virtual char* sendFile();
  virtual char* receiveFile(const char* mesg);
};

char* Server::sendFile()
{
    char* fs_name = "./server_send.txt";
    char sdbuf[LENGTH]; // Send buffer
    printf("[Server] Sending %s to the Client...", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found on server.\n", fs_name);
        exit(1);
    }

    fread(sdbuf, sizeof(char), LENGTH, fs);
    printf("Ok sent to client!\n");
    cout << "Send \"" << sdbuf << "\", sending it back." << endl;
    return CORBA::string_dup(sdbuf);
}

char* Server::receiveFile(const char* mesg)
{
    char* fr_name = "./server_receive.txt";
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL)
        printf("File %s Cannot be opened file on server.\n", fr_name);
    else
    {
        int write_sz = fwrite(mesg, sizeof(char), sizeof(mesg), fr);
        printf("Ok received from client!\n");
        fclose(fr);
    }
  return CORBA::string_dup(mesg);
}

int main(int argc, char** argv)
{
	try {
		CORBA::ORB_var          orb = CORBA::ORB_init(argc, argv);
		CORBA::Object_var       obj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

		PortableServer::Servant_var<Server> server = new Server();

		poa->activate_object(server);

		CORBA::String_var sior(orb->object_to_string(server->_this()));
		cout << sior << endl;

	 	PortableServer::POAManager_var pman = poa->the_POAManager();
		pman->activate();

		orb->run();
		orb->destroy();
	}
	catch (CORBA::SystemException& ex) {
	    cerr << "Caught CORBA::" << ex._name() << endl;
	}
	catch (CORBA::Exception& ex) {
	    cerr << "Caught CORBA::Exception: " << ex._name() << endl;
	}
	return 0;
}