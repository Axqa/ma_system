#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "worldmodel.h"
#include "connection.h"

void* message_callback( void *v );

class MessageHandler
{
    WorldModel *wm;
    Connection *cn;


public:
    MessageHandler(WorldModel *wm, Connection *cn);

    void handleMessagesFromServer();
    bool analyzeMessage(char* msg);

    bool analyzeInitMessage(char* msg);
    bool analyzeSeeMessage(char* msg);
    bool analyzeHearMessage(char* msg);
    bool analyzeSenseMessage(char* msg);
};

#endif // MESSAGEHANDLER_H
