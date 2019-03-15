#ifndef LZT_TEST_H
#define	LZT_TEST_H

#include "TestException.h"
//TODO merge with lzt_debug when you get rid of libcwd

#ifndef LZT_TEST_CPPU

    #ifndef TEST_ASSERT
        #define TEST_ASSERT(assert) \
        if ( (assert) == false) throw TestException(#assert);
    #endif

    #ifndef TEST_ASSERT_MESSAGE
        #define TEST_ASSERT_MESSAGE(assert, message) \
        if ( (assert) == false) throw TestException(#assert, message);
    #endif

#endif //LZT_TEST_CPPU

///* END LZT_TEST_CPPU */
//#else
//    #include <cppunit/extensions/HelperMacros.h>
//
//    #ifndef TEST_ASSERT
//        #define TEST_ASSERT(assert) \
//         CPPUNIT_ASSERT(assert);
//    #endif
//
//    #ifndef TEST_ASSERT_MESSAGE
//        #define TEST_ASSERT_MESSAGE(assert, message) \
//        CPPUNIT_ASSERT_MESSAGE(assert, message);
//    #endif
//#endif
//

#endif	/* LZT_TEST_H */