#include <errno.h>       // EWOULDBLOCK
#include <string.h>      // strlen
#include <sys/types.h>   // socket

#ifndef WIN32
  #include <unistd.h>      // close
  #include <sys/socket.h>  // socket
#endif

#ifdef Solaris
#include <sys/socket.h>  // AF_INET SOCK_DGRAM
#endif

#include "connection.h"
#include "logger.h"      // LOG
//#include<iostream>
using namespace std;
extern Logger Log; /*!< This is a reference to the Logger for writing info to*/

/*****************************************************************************/
/********************** CONNECTION *******************************************/
/*****************************************************************************/

/*! Default constructor. Only sets the maximum message size. */
Connection::Connection( )
{
  m_iMaxMsgSize = 2048;
}

/*! Constructor makes a connection with the server using the connect method.
    \param hostname string representation of host machine (string or IP number)
    \param port port number for connection of the host machine
    \param iMaxSize maximum message size that can be sent or received */
Connection::Connection(const char *hostname, int port, int iMaxSize)
{
  m_iMaxMsgSize = iMaxSize;
  if(  connect( hostname, port ) )
    Log.log( 1,"(Connection:connection) Socket connection made with %s:%d",
                                                             hostname, port );
  else
    Log.log( 1,"(Connection:Connection) Could not create connection with %s:%d"
                                                          , hostname, port );
}

/*! Deconstructor closes the connection with the server */
Connection::~Connection()
{
  disconnect();
}

/*! This method sets up a connection with the server.
    \param hostname string representation of host machine (string or IP number)
    \param port port number for connection of the host machine
    \return bool indicating whether connection was made */
bool Connection::connect(const char *host, int port )
{
  struct hostent *host_ent;
  struct in_addr *addr_ptr;
  struct sockaddr_in  cli_addr ;
  int    sockfd ;

  m_sock.socketfd = -1 ;

  if( (host_ent = (struct hostent*)gethostbyname(host)) == NULL)
  {
    // if not a string, get information from IP adress.
    if( inet_addr(host) == INADDR_NONE )
    {
      cerr << "(Connection::connect) Cannot find host " << host << endl;
      return false ;
    }
  }
  else   // get the necessary information from the hostname (string)
  {
    addr_ptr = (struct in_addr *) *host_ent->h_addr_list;
    host = inet_ntoa(*addr_ptr);
  }

  //  Open UDP socket.
  if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    cerr << "(Connection::connect) Cannot create socket " << host << endl;
    return false ;
  }

  // insert the information of the client
  cli_addr.sin_family      = AF_INET ;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
  cli_addr.sin_port        = htons(0) ;

  // bind the client to the server socket
  if(bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0)
  {
    cerr << "(Connection::connect) Cannot bind local address " << host << endl;
    return false ;
  }

  //  Fill in the structure with the address of the server.
  m_sock.socketfd = sockfd ;

  m_sock.serv_addr.sin_family       = AF_INET ;
  m_sock.serv_addr.sin_addr.s_addr  = inet_addr(host);
  m_sock.serv_addr.sin_port         = htons(port) ;

  return true;
}

/*! This method closes the current socket connection.  */
void Connection::disconnect( void )
{
  if (isConnected() )
  {
    close(m_sock.socketfd) ;
    m_sock.socketfd = -1; // This also 'sets' isConnected() to false
  }
}

/*! This method determines whether the socket connection is connected.
    \return bool indicating whether socket connection is available */
bool Connection::isConnected(void) const
{
  return(m_sock.socketfd != -1);
}

/*! This method reads a message from the connection. When there is
    no message available, it blocks till it receives a message.
    \param msg string in which message is stored
    \param maxsize maximum size of the message.
    \return -1: error, 0: 0 bytes were read, 1 when read was succesfull */
int Connection::receiveMessage( char *msg, int maxsize )
{

  socklen_t servlen ;
  int n;
  struct sockaddr_in serv_addr ;

  servlen = sizeof(serv_addr) ;

  // receive message from server
  n = recvfrom(m_sock.socketfd, msg, maxsize, 0,
                              (struct sockaddr *)&serv_addr, &servlen);

  if(n < 0)                                   // error
  {
    if( n == -1 && errno == EWOULDBLOCK)
    {
      msg[0] = '\0' ;
      return 0 ;
    }
    else
      return -1;
  }
  else                                       // succesfull, set new server info
  {                                          // next message will go to there
    m_sock.serv_addr.sin_port = serv_addr.sin_port ;
    msg[n] = '\0' ;

    return ( n == 0 ) ? 0 : 1 ;
  }
}

/*! This method sends a message to the server using the current connection.
    \param msg string which contains message
    \return true on succes, false in case of failure */
bool Connection::sendMessage( const char *msg )
{
  int n;

  n = strlen(msg) + 1 ;
  if( sendto(m_sock.socketfd, msg, n, 0,
      (struct sockaddr *)&m_sock.serv_addr, sizeof(m_sock.serv_addr)) != n )
    return false ;
  return true ;
}


/*! This method prints whether the connection is up or not.
    \param os output stream to which output should be directed */
void Connection::show( ostream os )
{
  if( ! isConnected() )
    os << "Not connected" << endl;
  else
    os << "Connected" << endl;

}



