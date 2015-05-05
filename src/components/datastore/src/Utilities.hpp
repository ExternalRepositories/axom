/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   Header file containing utility functions.
 *
 ******************************************************************************
 */

#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include <iostream>
#include <iomanip>
#include <string>


/*******************************************************************************
*
* The pre-processor macros below should be used to catch erroneous, or
* potentially dangerous, run-time conditions in the ASC CS Toolkit. When 
* used extensively in the code, they help simplify the debugging process for 
* developers and users. In partcular, enable developers to specify conditions 
* when the code cannot continue correct execution or when continued execution 
* is suspect. For example, they are especially helpful to check arguments 
* passed to methos; e.g., null pointers, values out of range, etc.  There 
* are many other uses as well.
*
* The following four macros are defined below:
*
*   - ATK_ERROR(message)
*   - ATK_WARNING(message)
*   - ATK_ASSERT(bool_expr)
*   - ATK_ASSERT_MSG(bool_expr, message)
*
* Each macro accepts a message string or boolean expression and message string.
* A message string argument may be any formatted character string argument 
* that can be accepted by standard C++ I/O stream utilities. A boolean 
* expression should evaulate to 'true' when the program is correct and 'false' 
* otherwise. When triggered, all macros print the bool_expr and/or message to 
* std output along with the file and line number where encountered.
*
* The first two macros, ATK_ERROR and ATK_WARNING, are always active when
* placed in the code and so should be used wherever runtime error or warning 
* messages should be generated, such as when improper program state is 
* defined by user-supplied information. The first one causes the program to 
* stop and exit. The second one does not do this and the program continues 
* execution. 
*
* The other two 'ASSERT' macros may be compiled in and out of the code. When
* the preprocessor constant 'ATK_DEBUG' is defined, they are active; otherwise,
* they are disabled. This provides a simple way to add aggressive diagnostic 
* checks to the code without incurring excessive run-time overhead. 
* 
* NOTE: These macros provide rudimentary functionality and should be enhanced 
*       or replaced when necessary.
*
*******************************************************************************/



//-----------------------------------------------------------------------------
//
/// The ATK_ERROR macro prints the message, file, and line number to std out
/// and then ends the program by calling exit().  It is always active.
///
/// Usage: ATK_ERROR( "Abandon Ship!!" );  
///
//-----------------------------------------------------------------------------
#define ATK_ERROR( msg )                                      \
do {                                                                \
    std::ostringstream oss;                                         \
    oss << "Error Message: " << msg << std::ends;                   \
    asctoolkit::utilities::processAbort( oss.str(), __FILE__, __LINE__);  \
} while (0)


//-----------------------------------------------------------------------------
//
/// The ATK_WARNING macro prints the message, file, and line number to std 
/// out and does not force the program to quit.  It is always active.
///
/// Usage: ATK_WARNING( "Hal, open the pod bay doors." );  
///
//-----------------------------------------------------------------------------
#define ATK_WARNING( msg )                                    \
do {                                                                \
    std::ostringstream oss;                                         \
    oss << "Warning Message: " << msg << std::ends;                 \
    asctoolkit::utilities::processWarning( oss.str(), __FILE__, __LINE__);\
} while (0)


#if defined(ATK_DEBUG) || 1
//-----------------------------------------------------------------------------
//
/// The ATK_ASSERT macro can be used to capture an assertion when
/// the given expression does not evaluate to true. It prints the failed
/// the failed assertion, file, and line number to std out and then forces 
/// the program to exit in the same way as ATK_ERROR above.
///
/// Usage:  ATK_ASSERT( my_val == 1 ); 
///
//-----------------------------------------------------------------------------
#define ATK_ASSERT( EXP )                                        \
do {                                                                   \
    if (!(EXP)) {                                                      \
       std::ostringstream oss;                                         \
       oss << "Failed Assert: " << # EXP << std::ends;                 \
       asctoolkit::utilities::processAbort( oss.str(), __FILE__, __LINE__);  \
    }                                                                  \
} while (0)


//-----------------------------------------------------------------------------
//
/// The ATK_ASSERT_MSG macro can be used to capture an assertion when
/// the given expression does not evaluate to true. It prints the failed
/// the failed assertion, file, line number, and message to std out and 
//  then forces the program to exit in the same way as ATK_ERROR above.
///
/// Usage:  ATK_ASSERT( my_val == 1, "my_val must always be one" ); 
///
//-----------------------------------------------------------------------------
#define ATK_ASSERT_MSG( EXP, msg )                               \
do {                                                                   \
    if (!(EXP)) {                                                      \
       std::ostringstream oss;                                         \
       oss << "Failed Assert: " << # EXP << std::endl << msg << std::ends;\
       asctoolkit::utilities::processAbort( oss.str(), __FILE__, __LINE__);  \
    }                                                                  \
} while (0)


#else  // ASSERTION CHECKS TURNED OFF....

#define ATK_ERROR( ignore_message ) ((void) 0) 
#define ATK_WARNING( ignore_message ) ((void) 0) 
#define ATK_ASSERT( ignore_EXP ) ((void) 0) 
#define ATK_ASSERT_MSG( ignore_EXP, ignore_message ) ((void) 0) 

#endif


/*!
 ******************************************************************************
 *
 * Namespace containing 
 *  
 ******************************************************************************
 */
namespace asctoolkit
{
namespace utilities
{

   /*!
    * \brief Print message, file, line number to preferred output stream 
    *        (not specified here).
    */
   void printMessage(
      const std::string& message,
      const std::string& filename,
      const int line);

   /*!
    * \brief Process error message with file and line number information 
    *        and abort the program.
    */
   void processAbort(
      const std::string& message,
      const std::string& filename,
      const int line); 

   /*!
    * \brief Process warning message with file and line number information 
    *        and let program exectution continue.
    */
   void processWarning(
      const std::string& message,
      const std::string& filename,
      const int line);

   /*!
    * \brief Convert an integer to a string.
    *
    * The returned string is padded with zeros as needed so that it
    * contains at least the number of characters indicated by the
    * minimum width argument.  When the number is positive, the
    * string is padded on the left. When the number is negative,
    * the '-' sign appears first, followed by the integer value
    * padded on the left with zeros.  
    *
    * For example, intToString(12, 5) returns "00012" and 
    * intTo2String(-12, 5) returns "-0012".
    */
   std::string intToString(
      int val,
      int min_width = 1);

}  // ending brace for utilities namespace
}  // ending brace for asctoolkit namespace

#endif /* UTILITIES_HPP_ */
