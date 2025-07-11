void add_newline_to_file(const char* filename);
int internal_assemble(const char* filein, const char* fileout);

// Reset lexer state to initial values (call before each assembly run)
void reset_lexer(void);