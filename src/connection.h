#ifndef _CONNECTION_
#define _CONNECTION_

#include <stdio.h>     // printf, FILE
#include <iostream>    // ostream

#include <netdb.h>     // sockaddr_in
#include <arpa/inet.h> // sockaddr_in

using namespace std;

/*! Socket is a combination of a filedescriptor with a server adress.        */
typedef struct _socket{
    int    socketfd  ;                /*!< File descriptor of the socket.    */
    struct sockaddr_in  serv_addr ;   /*!< Server information of the socket. */
} Socket ;

/********************** CONNECTION *******************************************/

/*! This class creates creates a (socket) connection using a hostname and a
    port number. After the connection is created it is possible to send and
    receive messages from this connection. It is based on the client program
    supplied with the soccer server defined in client.c and created by
    Istuki Noda et al.*/
class Connection {

  Socket         m_sock;        /*!< communication protocol with the server. */
  int            m_iMaxMsgSize; /*!< max  message size for send and receive  */
public:

  // constructors
  Connection            (                                               );
  Connection            ( const char *hostname, int  port,   int iSize  );
  ~Connection           (                                               );

  // methods that deal with the connection itself
  bool connect          ( const char *host,     int  port               );
  void disconnect       ( void                                          );
  bool isConnected      ( void                                          )const;

  int  receiveMessage   ( char       *msg,      int  maxsize            );
  bool sendMessage      ( const char *msg                               );

  void show             ( ostream    os                                 );

};

#endif
