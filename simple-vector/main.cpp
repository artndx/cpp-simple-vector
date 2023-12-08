#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>
#include "simple_vector.h"

#include "tests.h"

using namespace std;

int main() {
    Test1();
    Test2();
    TestReserveConstructor();
    TestReserveMethod();
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();
    return 0;
}