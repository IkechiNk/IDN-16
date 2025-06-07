#include "../unity/unity.h"
#include "idn16/codegen.h"
#include "idn16/symbol_table.h"
#include "idn16/instructions.h"
#include <stdio.h>
#include <unistd.h>

void setUp(void) {
    // Reset codegen state before each test  
    free_symbols();
}

void tearDown(void) {
    free_symbols();
}

void test_basic_emit_and_finalize(void) {
    // Test basic emit functionality by calling emit functions
    // and then finalizing - this tests the public API without
    // accessing internal state
    
    emit_reg_format(0, 0, 1, 2, 3, 0);  // ADD r1, r2, r3
    emit_imm_format(2, 8, 1, 0, 42);     // LDI r1, 42
    
    char temp_filename[] = "/tmp/test_codegen_basic_XXXXXX";
    int fd = mkstemp(temp_filename);
    close(fd);
    
    // This should not crash and should create a file
    finalize_output(temp_filename);
    
    // Verify the file was created and has content
    FILE* f = fopen(temp_filename, "rb");
    TEST_ASSERT_NOT_NULL(f);
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    TEST_ASSERT_TRUE(size > 0);
    
    fclose(f);
    unlink(temp_filename);
}

void test_symbol_resolution_integration(void) {
    // Test symbol resolution through the public API
    insert_symbol("test_label", 16, 0);  // Label at address 16
    
    emit_jb_format_identifier(0, 16, "test_label");  // JMP to test_label
    
    char temp_filename[] = "/tmp/test_codegen_symbols_XXXXXX";
    int fd = mkstemp(temp_filename);
    close(fd);
    
    // This should resolve the symbol and create a valid binary
    finalize_output(temp_filename);
    
    // Verify the file was created  
    FILE* f = fopen(temp_filename, "rb");
    TEST_ASSERT_NOT_NULL(f);
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    TEST_ASSERT_TRUE(size > 0);
    
    fclose(f);
    unlink(temp_filename);
}

int main(void) {
    UNITY_BEGIN(); 
    RUN_TEST(test_basic_emit_and_finalize);
    RUN_TEST(test_symbol_resolution_integration);
    
    return UNITY_END();
}