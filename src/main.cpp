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
#include "playermapper.h"
#include "strategy.h"
#include "globals.h"
#include <fstream>
#include <list>
#include <cstring>
#include "parser.h"

extern Logger Log;


using namespace std;

void printOptions();
void readConfigFile(const char* filename);


int main(int argc, char * argv[])
{
    pthread_t      listen, msgs;
    ofstream os;

    string host = "localhost";
    int port = 6000;
    string teamName = "Test";
    string formationFile = "formations.conf";
    string strategyFile = "strategy.conf";
    string confFile = "config.conf";
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
            case 's':
                strategyFile = string(argv[i+1]);
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
                case 'i':
                    HISTORY_SIZE = atoi(argv[i+1]);
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
    if (moveType == OBSERVE || moveType == NO_MOVE_OBSERVE)
    {
        readConfigFile(confFile.data());
    }

    if (logFile.empty() == false)
    {
        os.open(logFile);
        Log.setOutputStream(os);
//        Log.setLogAll(true);
        Log.addLogLevel(4);
    }

    PlayerMapper *pm;
    switch (MAP_TYPE)
    {
    case MAP_CLOSEST:
        pm = new ClosestMapper();
        break;
    case MAP_MINSUM:
        pm = new MinSumMapper();
        break;
    case MAP_CLOSEST_PRED:
        pm = new ClosestPredictMapper();
        break;
    case MAP_MINSUM_PRED:
        pm = new MinSumPredictMapper();
        break;
    default:
        cout << "Wrong PlayerMapper " << MAP_TYPE << endl;
        exit(1);
    }

//    ClosestMapper pm;
//    MinSumMapper pm;
//    ClosestPredictMapper pm;
//    MinSumPredictMapper pm;

    Strategy st(strategyFile.data());

    Formations fm(formationFile.data(), playerUnum);
    Connection cn(host.data(), port, MAX_MSG);
    WorldModel wm(&fm, pm, &st);
    ActHandler act(&cn, &wm);

    MessageHandler mh(&wm, &cn);

    Player player(&wm, &act, &fm, &st, teamName, 17, moveType);

    srand(fm.getPlayerNum() * (1 + wm.getSide()));

    pthread_create( &msgs, NULL, message_callback, &mh);        // start message handling

    if (useInput)
        pthread_create( &listen, NULL, stdin_callback, &player);    // start stdin handling

    player.mainLoop();

    cn.disconnect();

    if (logFile.empty() == false)
        os.close();

    delete pm;

    return 0;
}

void printOptions()
{
    cout <<
            "Help: \n" <<
            "-[ho]st                - host name \n" <<
            "-[he]lp                - print options \n"<<
            "-[hi]story             - history size \n" <<
            "-[p]ort                - port num \n" <<
            "-[t]eam                - teamname \n" <<
            "-[n]um                 - player number \n" <<
            "-[l]og                 - log file \n" <<
            "-[s]trategy            - strategy file \n" <<
            "-[i]nput               - enable input from console\n" <<
            "-[m]oveType            - agent move type, 0 - observe, 1 - random line\n" << endl;
}

void readConfigFile(const char* filename)
{
    fstream in(filename);
    if( !in )
    {
      cerr << "Could not open file '" <<
        filename << "'" << endl;
      return;
    }


    char line[400], *str;
    int curX, curY;
    bool ret = true;

    int curLine = -1;

    string history_size = "history_size";
    string mapper = "mapper";
    string predict_type = "predict_type";
    string observer_pos = "observer_pos";
    while(ret && in.getline(line, 400))
    {
        str = &line[0];
        // comment and empty lines should be skipped
        if( !(line[0] == '\n' || line[0] == '#' || line[0]=='\0' ||
              Parse::gotoFirstNonSpace( &str ) == '\0' ) )
        {
            if (strncmp(str, history_size.data(), history_size.size()) == 0)
            {
                HISTORY_SIZE = Parse::parseFirstInt(&str);
                cout << history_size << " " << HISTORY_SIZE << endl;
            }
            if (strncmp(str, mapper.data(), mapper.size()) == 0)
            {
                MAP_TYPE = (MapT)Parse::parseFirstInt(&str);
                cout << mapper << " " << MAP_TYPE << endl;
            }
            if (strncmp(str, predict_type.data(), predict_type.size()) == 0)
            {
                PREDICT_TYPE = (PredT)Parse::parseFirstInt(&str);
                cout << predict_type << " " << PREDICT_TYPE << endl;
            }
            if (strncmp(str, observer_pos.data(), observer_pos.size()) == 0)
            {
                int x = Parse::parseFirstInt(&str);
                int y = Parse::parseFirstInt(&str);
                AGENT_POS = VecPosition(x,y);
                cout << observer_pos << " " << AGENT_POS << endl;
            }


        }
    }
    in.close();
}

