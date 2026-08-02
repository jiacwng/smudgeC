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

static void test_name_set_add_contains_and_free(void)
{
    NameSet set;
    name_set_init(&set);

    TEST_ASSERT_EQUAL_INT(1, name_set_add(&set, "WIDTH"));
    TEST_ASSERT_EQUAL_INT(1, name_set_add(&set, "HEIGHT"));
    TEST_ASSERT_EQUAL_INT(1, name_set_add(&set, "WIDTH"));
    TEST_ASSERT_EQUAL_INT(2, set.count);

    TEST_ASSERT_EQUAL_INT(1, name_set_contains(&set, "WIDTH"));
    TEST_ASSERT_EQUAL_INT(1, name_set_contains(&set, "HEIGHT"));
    TEST_ASSERT_EQUAL_INT(0, name_set_contains(&set, "DEPTH"));

    name_set_free(&set);
    TEST_ASSERT_EQUAL_INT(0, set.count);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_protected_identifier_known_and_unknown);
    RUN_TEST(test_keyword_known_and_unknown);
    RUN_TEST(test_name_set_add_contains_and_free);
    return UNITY_END();
}
