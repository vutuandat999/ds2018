#include "chat.hh"
#include <iostream>
using namespace std;

#define LENGTH 512

static void sendFile(RPC::Chat_ptr e){
    char* fs_name = "./client_send.txt";
    char sdbuf[LENGTH];
    printf("[Client] Sending %s to the Server... ", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }

    // Tell server to receive file
    e->receiveFile(sdbuf);
    printf("Ok File %s from Client was Sent!\n", fs_name);
}

static void receiveFile(RPC::Chat_ptr e){
    CORBA::String_var revbuf;
    printf("[Client] Receiveing file from Server and saving it as client_receive.txt...");
    char* fr_name = "./client_receive.txt";
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL)
        printf("File %s Cannot be opened.\n", fr_name);
    else
    {
        revbuf = e->sendFile();
        int write_sz = fwrite(revbuf, sizeof(char), sizeof(revbuf), fr);
        printf("Ok received from server!\n");
        fclose(fr);
    }
}


int main(int argc, char* argv[]) {
    string s;
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        //Check missing argument
        if (argc != 2) {
        cerr << "Usage: client <object reference>" << endl;
        return 1;
        }

        CORBA::Object_var obj = orb->string_to_object(argv[1]);

        RPC::Chat_var chatref = RPC::Chat::_narrow(obj);

        if (CORBA::is_nil(chatref)) {
        cerr << "Can't narrow reference." << endl;
        return 1;
        }

        while (1) {
            sendFile(chatref);
            receiveFile(chatref);

            printf("Continue?(Y/N)\n");
            printf("client>");
            scanf("%s", s);
            if (s[0] == 'N'){
                printf("[Client] Connection lost.\n");
                return 0;
            }
        }
        orb->destroy();
    }
    catch (CORBA::TRANSIENT&) {
        cerr << "Caught system exception TRANSIENT -- unable to contact the "
            << "server." << endl;
    }
    catch (CORBA::SystemException& ex) {
        cerr << "Caught a CORBA::" << ex._name() << endl;
    }
    catch (CORBA::Exception& ex) {
        cerr << "Caught CORBA::Exception: " << ex._name() << endl;
    }
    return 0;
}