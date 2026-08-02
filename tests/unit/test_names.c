#include "unity.h"
#include "names.h"

void setUp(void) {}
void tearDown(void) {}

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

static void test_load_protected_names(void)
{
    NameSet set;
    name_set_init(&set);

    TEST_ASSERT_EQUAL_INT(1, load_protected_names("data/protected_names.txt", &set));
    TEST_ASSERT_EQUAL_INT(1, name_set_contains(&set, "printf"));
    TEST_ASSERT_EQUAL_INT(1, name_set_contains(&set, "qsort"));
    TEST_ASSERT_EQUAL_INT(0, name_set_contains(&set, "user_defined_thing"));

    name_set_free(&set);
}

static void test_collect_header_identifiers(void)
{
    NameSet set;
    name_set_init(&set);

    TEST_ASSERT_EQUAL_INT(1, collect_header_identifiers("src/names.h", &set));
    TEST_ASSERT_EQUAL_INT(1, name_set_contains(&set, "NameSet"));
    TEST_ASSERT_EQUAL_INT(1, name_set_contains(&set, "name_set_add"));
    TEST_ASSERT_EQUAL_INT(0, name_set_contains(&set, "int"));

    name_set_free(&set);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_keyword_known_and_unknown);
    RUN_TEST(test_name_set_add_contains_and_free);
    RUN_TEST(test_load_protected_names);
    RUN_TEST(test_collect_header_identifiers);
    return UNITY_END();
}
