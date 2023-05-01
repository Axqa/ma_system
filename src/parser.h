#ifndef PARSER_H
#define PARSER_H

/*! This class contains several static methods which can be used for parsing
    string messages and uses some of the ideas contained in CMU'99 source code
    of Peter Stone. Tests shows that scanning integers has a performance
    increase of 30.3% over the method used by CMUnited and 68.0% over sscanf.
    For parsing doubles the performance increase was 15.4% compared to CMUnited
    and 85.1% compared to sscanf. */
class Parse
{
public:

  // methods which return a specific type of value from a string message
  static double parseFirstDouble              ( char** strMsg                );
  static int    parseFirstInt                 ( char** strMsg                );

  // methods which move to a specific position in a string message
  static char   gotoFirstSpaceOrClosingBracket( char** strMsg                );
  static int    gotoFirstOccurenceOf          ( char   c      , char** strMsg);
  static char   gotoFirstNonSpace             ( char** strMsg                );
};


#endif // PARSER_H
