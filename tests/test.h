#pragma once

#include "helpers/all.h"

#include <iostream>

namespace sedfer::test {

struct stats {
    static usize passed;
    static usize failed;
};

static consteval const char* trim_path(const char * file) {
    const char * result = file;
    const char * curr = file;
    while(*curr != '\0') {
        if(*curr == '/' || *curr == '\\') {
            result = curr + 1;
        }
        curr += 1;
    }

    return result;
}

}

#define EXPR_FILE_LINE_FUNCTION(expr) "'" << (expr) << "'\n  " << sedfer::test::trim_path(__FILE__) << ":" << __LINE__ << "\n  " << __PRETTY_FUNCTION__
#define EXPECT(expr, details) do { if(expr) {                                                \
                                     sedfer::test::stats::passed += 1;                        \
                                   } else {                                                  \
                                     sedfer::test::stats::failed += 1;                        \
                                     std::cout << "FAIL " << EXPR_FILE_LINE_FUNCTION(#expr); \
                                     std::cout << "\n  " << details << std::endl;            \
                                   }                                                         \
                                 } while(false)

#define ASSERT(expr) do { if(not (expr)) {                                                                \
                            std::cout << "ASSERT FAILED " << EXPR_FILE_LINE_FUNCTION(#expr) << std::endl; \
                            abort();                                                                      \
                          }                                                                               \
                        } while(false)
