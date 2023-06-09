#include "logger.h"
#include <stdio.h>    // needed for vsprintf
#include <string>     // needed for string
#include <stdarg.h>
#include <pthread.h>
#include <string.h>

Logger Log;          /*!<Logger instantation that can be used by all classes */
Logger LogDraw;      /*!<Drawing logger instantation for all classes         */

/*****************************************************************************/
/**************************** LOGGER *****************************************/
/*****************************************************************************/

/*!This is the constructor for the Logger. The output stream, the minimal and
   maximal log level can all be specified. The timer in this class is also
   restarted.
   \param o outputstream (file or cout) to which information is printed
            (default cout)
   \param iMin minimal log level (default 0)
   \param iMax maximal log level (default 0) */
Logger::Logger( ostream& o, int iMin, int iMax )
{
  pthread_mutex_init( &mutex_stream, NULL );
  strcpy( m_strHeader, "" );
  m_timing.restartTime();
  addLogRange( iMin, iMax );
  m_os = &o;
}

/*!This method can be used to log information. Only when the specified
   level of the message is part of the set of logged levels the
   information is logged. This method receives a a normal string that
     is logged.
   \param iLevel level corresponding to this message
   \param str string that is logged when iLevel is a logging lvel.
   \return bool indicating whether the message was logged or not. */
bool Logger::log( int iLevel, string str)
{
  if( isInLogLevel( iLevel ) )
  {
    *m_os << "'" << iLevel << "' :" << m_strHeader << str;
    return true;
  }

  return false;
}

/*!This method can be used to log information. Only when the specified
   level of the message is part of the set of logged values the
   information is logged. This method receives a character string that may
   contain format specifiers that are also available to 'printf' (like %d, %f,
   etc.). The remaining arguments are the variables that have to be filled in
   at the location of the specifiers.
   \param iLevel level corresponding to this message
   \param str character string with (possible) format specifiers
   \param ... variables that define the values of the specifiers.
   \return bool indicating whether the message was logged or not. */
bool Logger::log( int iLevel, const char *str, ... )
{
  if( isInLogLevel( iLevel ) )
  {
    logSignal(); // test whether there are no old strings left to log
    va_list ap;

    va_start( ap, str );
    if( vsnprintf( m_buf, MAX_LOG_LINE-1, str, ap ) == -1 )
      cerr << "Logger::log, buffer is too small!\n" ;
    va_end(ap);
    *m_os << "'" << iLevel << "' :" << m_strHeader << m_buf << "\n";
    return true;
  }

  return false;
}

/*!This method can be used to log information. Only when the specified
   level of the message is an element in the set of logged levels the
   information is logged. This method receives a character string that may
   contain format specifiers that are also available to 'printf' (like %d, %f,
   etc.). The remaining arguments are the variables that have to be filled in
   at the location of the specifiers. Before the logged message the elapsed
   time since the timer has been restarted is printed.
   \param iLevel level corresponding to this message
   \param str character string with (possible) format specifiers
   \param ... variables that define the values of the specifiers.
   \return bool indicating whether the message was logged or not. */
bool Logger::logWithTime( int iLevel, const char *str, ... )
{
  if( isInLogLevel( iLevel ) )
  {
    logSignal(); // test whether there are no old strings left to log
    va_list ap;

    va_start( ap, str );

    if( vsnprintf( m_buf, MAX_LOG_LINE-1, str, ap ) == -1 )
      cerr << "Logger::log, buffer is too small!" << "\n";
    va_end(ap);

    string s = m_strHeader;
    s.append( m_buf );
    s.copy( m_buf, string::npos );
    m_buf[s.length()] = '\0';
    m_timing.printTimeDiffWithText( *m_os, m_buf );

    return true;
  }

  return false;
}

/*!This method can be used to log information. The main difference
 * with the standard log method is that in this case the string is not
 * printed to the output stream immediately but only at the next call
 * of log (or logWithTime). This is a work-around for specific
 * compiler cases in which a deadlock occurs when something is logged
 * at the same instance by two different threads.

 \param iLevel level corresponding to this message
 \param str character string with (possible) format specifiers
 \param ... variables that define the values of the specifiers.
 \return bool indicating whether the message was logged or not. */
bool Logger::logFromSignal( int iLevel, const char   *str, ...        )
{
  char buf[MAX_LOG_LINE];
  if( isInLogLevel( iLevel ) )
  {
    va_list ap;

    va_start( ap, str );

    if( vsnprintf( buf, MAX_LOG_LINE-1, str, ap ) == -1 )
      cerr << "Logger::log, buffer is too small!" << "\n";
    va_end(ap);

    char str[16];
    sprintf( str, "%2.2f: ", m_timing.getElapsedTime()*1000 );
    m_strSignal.append( str );
    m_strSignal.append( m_strHeader );
    m_strSignal.append( buf );
    m_strSignal.append(  "\n\0" );

    return true;
  }

  return false;

}

/*! This method actually writes the string that was created with
 *  logFromSignal to the specified output stream.
 \return bool indicating whether the message was logged or not. */
bool Logger::logSignal( )
{
  if( ! m_strSignal.empty() )
  {
    *m_os << "\n" << m_strSignal ;
    m_strSignal = "";
    return true;
  }
  return false;
}

/*!This method restarts the timer associated with this Logger. */
void Logger::restartTimer()
{
  return m_timing.restartTime();
}

/*! Return the instance of the timing class that denotes the time the counter
    is running. */
Timing Logger::getTiming( )
{
  return m_timing;
}


/*!This method returns whether the supplied log level is recorded, thus
   within the interval [min_log_level..max_log_level] or equal to the extra
   log level.
   \param iLevel log level that should be checked
   \return bool indicating whether the supplied log level is logged. */
bool Logger::isInLogLevel( int iLevel )
{
//  return true;
  return logAll || m_setLogLevels.find( iLevel ) != m_setLogLevels.end() ;
}

/*! This method inserts the log level 'iLevel' to the set of logged levels.
    Information from this log level will be printed.
        \param iLevel level that will be added to the set
    \return bool indicating whether the update was successfull. */
bool  Logger::addLogLevel( int iLevel )
{
  m_setLogLevels.insert( iLevel );
  return true;
}

/*! This method inserts all the log levels in the interval [iMin..iMax] to
    the set of logged levels.
    \param iMin minimum log level that is added
    \param iMax maximum log level that is added
    \return bool indicating whether the update was successfull. */
bool  Logger::addLogRange( int iMin, int iMax )
{
  bool bReturn = true;
  for( int i = iMin ; i <= iMax;  i++ )
    bReturn &= addLogLevel( i );
  return bReturn;
}

/**
 * @brief This method sets parameter which specify if
 * all logs are needed
 * @param iLogAll
 */
void Logger::setLogAll(bool iLogAll)
{
    logAll = iLogAll;
}

/*! This method returns the current header that is written before the actual
    text that has to be logged.
    \return current header */
char* Logger::getHeader( )
{
  return m_strHeader;
}

/*! This method sets the header that is written before the actual logging text.
    \param str that represents the character string
    \return bool indicating whether the update was succesfull */
bool Logger::setHeader( char *str )
{
  strcpy( m_strHeader, str );
  return true;
}

/*! This method sets a predefined header that is written before the actual
    logging text. The header is represented by two integers which are
    written between parentheses, i.e. (9, 2401).
    \param i1 first integer
    \param i2 second integer
    \return bool indicating whether the update was succesfull */
bool Logger::setHeader( int i1, int i2 )
{
  sprintf( m_strHeader, "(%d, %d) ", i1, i2 );
  return true;
}

/*! This method sets a predefined header that is written before the actual
    logging text. The header is represented by one integer which is followed by
    a semicolon (":") .
    \param integer
    \return bool indicating whether the update was succesfull */
bool Logger::setHeader( int i )
{
  sprintf( m_strHeader, " %d: ", i );
  return true;
}


/*!This method sets the output stream to which the log information is written.
   This outputstream can be standard output (cout) or a reference to a
   file.
   \param o outputstream to which log information is printed.
   \return bool indicating whether update was succesfull. */
bool Logger::setOutputStream( ostream& o )
{
  m_os = &o;
  return true;
}

/*!This method returns the output stream to which the log information is
   written. This outputstream can be standard output (cout) or a reference to a
   file.
   \return o outputstream to which log information is printed. */
ostream& Logger::getOutputStream(  )
{
  return *m_os;
}

/*! This method outputs all the log levels that are logged to the output stream
    os.
    \param os output stream to which log levels are printed. */
void Logger::showLogLevels( ostream &os )
{
  set<int>::iterator itr;
  for (itr = m_setLogLevels.begin() ; itr != m_setLogLevels.end() ; itr++)
    os << *itr << " " ;
  os << "\n";
}
/*****************************************************************************/
/********************** CLASS TIMING *****************************************/
/*****************************************************************************/

/*! This method returns the difference between two timevals in seconds.
    \param tv1 first timeval
    \param tv2 second timeval
    \return double representing the difference between t1 and t2 in seconds */
double Timing::getTimeDifference( struct timeval tv1, struct timeval tv2 )
{

  return  ((double)tv1.tv_sec + (double)tv1.tv_usec/1000000 ) -
          ((double)tv2.tv_sec + (double)tv2.tv_usec/1000000 ) ;
}

/*! This method prints the time in seconds that elapsed since
    the timer was restarted. It is possible to multiply this time with a
    factor, such that it can be used for different quantity values. In the
    default case this value is 1000 with the effect that the resulting value
    resembles miliseconds.
    \param os output stream to which output should be written.
    \param str that should be printed
    \param iFactor with which the elapsed time is multiplied (default 1000) */
void Timing::printTimeDiffWithText( ostream &os, char *str, int iFactor )
{
  // set the with to 6 and fill remaining places with '0'.
  os <<setw(6)<< setfill('0')<< getElapsedTime()*iFactor << ":" << str << "\n";
}

/*! This method returns the time (in seconds) since the last time the timer
    was restarted.
    \return double seconds that have passed since last restart of timer */
double Timing::getElapsedTime( int iFactor )
{
  struct timeval time2;
  gettimeofday( &time2, NULL );

  return getTimeDifference( time2, time1 )*iFactor;
}

/*! This method restarts the timer by setting it to the current time */
void Timing::restartTime( )
{
  gettimeofday( &time1, NULL );
}

