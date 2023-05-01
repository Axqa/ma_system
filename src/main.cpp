#include <iostream>
#include <pthread.h>
#include "connection.h"
#include "utilities.h"
#include "worldmodel.h"
#include "acthandler.h"
#include "logger.h"
#include "messagehandler.h"
#include "player.h"
#include "formation.h"
#include <cstring>


extern Logger Log;

using namespace std;

void printOptions();

int main(int argc, char * argv[])
{
    pthread_t      listen, msgs;
    ofstream os;

    string host = "localhost";
    int port = 6000;
    string teamName = "Test";
    string formationFile = "formations.conf";
    string logFile = "";
    int playerUnum = 9;
    MoveT moveType = OBSERVE;

    bool useInput = true;

    for( int i = 1 ; i < argc ; i++  )
    {
        if( argv[i][0] == '-' && strlen( argv[i] ) > 1)
        {
            switch (argv[i][1]) {
            case 't':
                teamName = string(argv[i+1]);
                break;
            case 'n':
                playerUnum = atoi(argv[i+1]);
                break;
            case 'l':
                logFile = string(argv[i+1]);
                break;
            case 'i':
                useInput = atoi(argv[i+1]) == 1 ? true : false;
                break;
            case 'm':
                moveType = (MoveT)atoi(argv[i+1]);
                break;
            case 'h':
                switch (argv[i][2]) {
                case 'e':
                    printOptions();
                    return 0;
                case 'o':
                    host = string(argv[i+1]);
                    break;
                default:
                    cout << "Wrong arg: " << argv[i] << endl;
                    return -1;
                }
                break;
            case 'p':
                port = atoi(argv[i+1]);
                break;
            default:
                cout << "Wrong arg: " << argv[i] << endl;
                return -1;
                break;

            }
        }
    }
    if (logFile.empty() == false)
    {
        os.open(logFile);
        Log.setOutputStream(os);
        Log.setLogAll(true);
    }

    Formations fm(formationFile.data(), playerUnum);
    Connection cn(host.data(), port, MAX_MSG);
    WorldModel wm(&fm);
    ActHandler act(&cn, &wm);

    MessageHandler mh(&wm, &cn);

    Player player(&wm, &act, &fm, teamName, 17, moveType);

    pthread_create( &msgs, NULL, message_callback, &mh);        // start message handling

    if (useInput)
        pthread_create( &listen, NULL, stdin_callback, &player);    // start stdin handling

    player.mainLoop();

    cn.disconnect();

    if (logFile.empty() == false)
        os.close();

    return 0;
}

void printOptions()
{
    cout <<
            "Help: \n" <<
            "-[ho]st                - host name \n" <<
            "-[he]lp                - print options \n"<<
            "-[p]ort                - port num \n" <<
            "-[t]eam                - teamname \n" <<
            "-[n]um                 - player number \n" <<
            "-[l]og                 - log file \n" <<
            "-[i]nput               - enable input from console\n" <<
            "-[m]oveType            - agent move type, 0 - observe, 1 - random line\n" << endl;
}
