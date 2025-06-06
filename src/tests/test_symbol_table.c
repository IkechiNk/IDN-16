#include "Unity/unity.h"
#include "../tools/assembler/symbol_table.h"
#include <string.h>

void setUp(void) {
    // Reset symbol table before each test
    free_symbols();
}

void tearDown(void) {
    free_symbols();
}

void test_insert_single_symbol(void) {
    insert_symbol("test_label", 0x1000, 1);
    
    // Sort symbols so they can be found with binary search
    sort_symbols();
    
    int type;
    int value = get_symbol("test_label", &type);
    
    TEST_ASSERT_EQUAL_INT(0x1000, value);
    TEST_ASSERT_EQUAL_INT(1, type);
}

void test_insert_multiple_symbols(void) {
    insert_symbol("label1", 0x1000, 1);
    insert_symbol("label2", 0x2000, 2);
    insert_symbol("var1", 0x3000, 0);
    
    sort_symbols();
    
    int type;
    
    int value1 = get_symbol("label1", &type);
    TEST_ASSERT_EQUAL_INT(0x1000, value1);
    TEST_ASSERT_EQUAL_INT(1, type);
    
    int value2 = get_symbol("label2", &type);
    TEST_ASSERT_EQUAL_INT(0x2000, value2);
    TEST_ASSERT_EQUAL_INT(2, type);
    
    int value3 = get_symbol("var1", &type);
    TEST_ASSERT_EQUAL_INT(0x3000, value3);
    TEST_ASSERT_EQUAL_INT(0, type);
}

void test_get_symbol_without_type(void) {
    insert_symbol("no_type_check", 0x4000, 5);
    
    int value = get_symbol("no_type_check", NULL);
    TEST_ASSERT_EQUAL_INT(0x4000, value);
}

void test_sort_symbols(void) {
    insert_symbol("zebra", 0x1000, 1);
    insert_symbol("alpha", 0x2000, 1);
    insert_symbol("beta", 0x3000, 1);
    
    sort_symbols();
    
    // After sorting, symbols should be searchable
    int type;
    int value_alpha = get_symbol("alpha", &type);
    TEST_ASSERT_EQUAL_INT(0x2000, value_alpha);
    
    int value_beta = get_symbol("beta", &type);
    TEST_ASSERT_EQUAL_INT(0x3000, value_beta);
    
    int value_zebra = get_symbol("zebra", &type);
    TEST_ASSERT_EQUAL_INT(0x1000, value_zebra);
}

void test_symbols_with_negative_values(void) {
    insert_symbol("negative", -100, 1);
    insert_symbol("large", 0xFFFF, 2);
    
    sort_symbols();
    
    int type;
    int value_neg = get_symbol("negative", &type);
    TEST_ASSERT_EQUAL_INT(-100, value_neg);
    TEST_ASSERT_EQUAL_INT(1, type);
    
    int value_large = get_symbol("large", &type);
    TEST_ASSERT_EQUAL_INT(0xFFFF, value_large);
    TEST_ASSERT_EQUAL_INT(2, type);
}

void test_symbols_with_long_names(void) {
    const char* long_name = "this_is_a_very_long_symbol_name_for_testing";
    insert_symbol(long_name, 0x5000, 3);
    
    sort_symbols();
    
    int type;
    int value = get_symbol(long_name, &type);
    TEST_ASSERT_EQUAL_INT(0x5000, value);
    TEST_ASSERT_EQUAL_INT(3, type);
}

void test_case_sensitive_symbols(void) {
    insert_symbol("CaseSensitive", 0x1000, 1);
    insert_symbol("casesensitive", 0x2000, 2);
    insert_symbol("CASESENSITIVE", 0x3000, 3);
    
    sort_symbols();
    
    int type;
    int value1 = get_symbol("CaseSensitive", &type);
    TEST_ASSERT_EQUAL_INT(0x1000, value1);
    TEST_ASSERT_EQUAL_INT(1, type);
    
    int value2 = get_symbol("casesensitive", &type);
    TEST_ASSERT_EQUAL_INT(0x2000, value2);
    TEST_ASSERT_EQUAL_INT(2, type);
    
    int value3 = get_symbol("CASESENSITIVE", &type);
    TEST_ASSERT_EQUAL_INT(0x3000, value3);
    TEST_ASSERT_EQUAL_INT(3, type);
}

void test_zero_value_symbol(void) {
    insert_symbol("zero_val", 0, 0);
    
    sort_symbols();
    
    int type;
    int value = get_symbol("zero_val", &type);
    TEST_ASSERT_EQUAL_INT(0, value);
    TEST_ASSERT_EQUAL_INT(0, type);
}

void test_special_characters_in_names(void) {
    insert_symbol("_underscore", 0x1000, 1);
    insert_symbol("number123", 0x2000, 2);
    
    sort_symbols();
    
    int type;
    int value1 = get_symbol("_underscore", &type);
    TEST_ASSERT_EQUAL_INT(0x1000, value1);
    
    int value2 = get_symbol("number123", &type);
    TEST_ASSERT_EQUAL_INT(0x2000, value2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_insert_single_symbol);
    RUN_TEST(test_insert_multiple_symbols);
    RUN_TEST(test_get_symbol_without_type);
    RUN_TEST(test_sort_symbols);
    RUN_TEST(test_symbols_with_negative_values);
    RUN_TEST(test_symbols_with_long_names);
    RUN_TEST(test_case_sensitive_symbols);
    RUN_TEST(test_zero_value_symbol);
    RUN_TEST(test_special_characters_in_names);
    return UNITY_END();
}