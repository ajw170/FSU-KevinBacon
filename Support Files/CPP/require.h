/*
   require.h

   replaces assert macro:

   (1) improves formatting of report
   (2) shows values of variables at time of failure
   (3) x versions ensure that condition is executed when NDEBUG is defined

   Nore that these are defined in <cassert>:
     __FILE__     name of source code file where assertion failed
     __LINE__     line number in file  where assertion failed
     __FUNCTION__ name of function  where assertion failed

*/

#ifndef NDEBUG
#include <cassert>
#include <iostream>

#define Require(condition) \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n'; \
    abort(); \
  } \
}

#define Require1(condition,x)                     \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n' \
              << " Variables: " << #x << " = " << x << '\n';      \
    abort(); \
  } \
}

#define Require2(condition,x,y)                  \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n' \
              << " Variables: " << #x << " = " << x << '\n'       \
              << "            " << #y << " = " << y << '\n';      \
    abort(); \
  } \
}

#define Require3(condition,x,y,z)               \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n' \
              << " Variables: " << #x << " = " << x << '\n'       \
              << "            " << #y << " = " << y << '\n'       \
              << "            " << #z << " = " << z << '\n';      \
    abort(); \
  } \
}

#define Requirex(condition) \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n'; \
    abort(); \
  } \
}

#define Require1x(condition,x)                     \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n' \
              << " Variables: " << #x << " = " << x << '\n';      \
    abort(); \
  } \
}

#define Require2x(condition,x,y)                  \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n' \
              << " Variables: " << #x << " = " << x << '\n'       \
              << "            " << #y << " = " << y << '\n';      \
    abort(); \
  } \
}

#define Require3x(condition,x,y,z)               \
{ \
  if(!(condition)) \
  { \
    std::cerr << "Requirement failed at " << __FILE__ << ":" << __LINE__ << '\n' \
              << " Function:  " << __FUNCTION__ << '\n' \
              << " Condition: " << #condition << '\n' \
              << " Variables: " << #x << " = " << x << '\n'       \
              << "            " << #y << " = " << y << '\n'       \
              << "            " << #z << " = " << z << '\n';      \
    abort(); \
  } \
}

#else

#define Require(condition) 
#define Require1(condition, x) 
#define Require2(condition, x, y) 
#define Require3(condition, x, y, z) 

#define Requirex(condition) (condition)
#define Require1x(condition, x) (condition)
#define Require2x(condition, x, y) (condition)
#define Require3x(condition, x, y, z) (condition)
#endif
