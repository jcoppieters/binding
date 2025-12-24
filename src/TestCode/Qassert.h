/********************************************************************
 * Quantum Assertions--Design by Contract(TM of ISI) facilities
// Copyright (c) 2002 Miro Samek, Palo Alto, CA. 
// All Rights Reserved.
 *******************************************************************/
#ifndef qassert_h
#define qassert_h
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/** NASSERT macro disables all contract validations
 * (assertions, preconditions, postconditions, and invariants).
 */
#ifndef NASSERT

   extern void onAssert__(char const *file, unsigned line, int fatal);


   #define DEFINE_THIS_FILE \
      static const char THIS_FILE__[] = __FILE__

   #define ASSERT(test_)\
      if (test_) {      \
      }                 \
      else onAssert__(THIS_FILE__, __LINE__, 0)

   #define ALLEGE(test_)\
      if (test_) {      \
      }                 \
      else onAssert__(THIS_FILE__, __LINE__, 1)

   #define REQUIRE(test_)   	ASSERT(test_)
   #define ENSURE(test_)    	ASSERT(test_)
   #define INVARIANT(test_) 	ASSERT(test_)
   #define ASSERT_FAILED   	onAssert__(THIS_FILE__, __LINE__, 0)



   #define _ASSERT(_test)\
      if (_test) {      \
      }                 \
      else onAssert__(__FILE__,__LINE__,0)

   #define _ALLEGE(_test)\
      if (_test) {      \
      }                 \
      else onAssert__(__FILE__,__LINE__,1)

   #define _REQUIRE(_test)			_ASSERT(_test)
   #define _ENSURE(_test)    		_ASSERT(_test)
   #define _INVARIANT(_test) 		_ASSERT(_test)
   #define _ASSERT_FAILED   		onAssert__(__FILE__,__LINE__, 0)



#else                                           /* #ifndef NASSERT */

   #define DEFINE_THIS_FILE extern const char THIS_FILE__[]

   #define ASSERT(test_)
   #define REQUIRE(test_)
   #define ENSURE(test_)
   #define INVARIANT(test_)
   #define ALLEGE(test_)\
      if (test_) {      \
      }                 \
      else

   #define ASSERT_FAILED


   #define _ASSERT(_test)\
      if (_test) {      \
      }                 \
      onAssert__(__FILE__,__LINE__)

   #define _REQUIRE(_test)
   #define _ENSURE(_test)
   #define _INVARIANT(_test)
   #define _ALLEGE(_test) \
      if (_test) {      \
      }                 \
      else

   #define _ASSERT_FAILED

#endif


#ifdef __cplusplus
}
#endif

/******************************************************************************/
#endif
/******************************************************************************/

