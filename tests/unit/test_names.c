#include "unity.h"
#include "names.h"

void setUp(void) {}
void tearDown(void) {}

static void test_protected_identifier_known_and_unknown(void)
{
    TEST_ASSERT_EQUAL_INT(1, is_protected_identifier("printf"));
    TEST_ASSERT_EQUAL_INT(0, is_protected_identifier("my_function"));
}

static void test_keyword_known_and_unknown(void)
{
    TEST_ASSERT_EQUAL_INT(1, is_keyword("int"));
    TEST_ASSERT_EQUAL_INT(0, is_keyword("banana"));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_protected_identifier_known_and_unknown);
    RUN_TEST(test_keyword_known_and_unknown);
    return UNITY_END();
}
