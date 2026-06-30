#ifndef NAMES_H
#define NAMES_H

typedef struct
{
    char original_names[128][128];
    char obfuscated_names[128][128];
    int symbol_count;
} SymbolTable;

void symbol_table_init(SymbolTable *table);
int is_keyword(char *word);
int is_protected_identifier(char *word);
char *get_obfuscated_name(SymbolTable *table, char *identifier);

#endif