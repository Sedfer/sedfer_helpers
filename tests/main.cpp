#include "tests/types.h"
#include "tests/packed.h"

#include "test.h"

namespace sedfer::test {

usize stats::passed = 0;
usize stats::failed = 0;

void test_const_buffer();

static void print_result() {
    if(test::stats::failed) {
        std::cout << "FAILED " << test::stats::failed << ", PASSED " << test::stats::passed << std::endl;
    } else {
        std::cout << "OK, PASSED " << test::stats::passed << std::endl;
    }
}

static void test_all() {
    test_const_buffer();

    print_result();
}

}

int main() {
    srand(time(nullptr));

    sedfer::test::test_all();

    return 0;
}
