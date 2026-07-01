#ifndef NAMES_H
#define NAMES_H

typedef struct
{
    char *original;
    char *obfuscated;
} Symbol;

typedef struct 
{
    Symbol *symbols;
    int count;
    int capacity;
}SymbolTable;



void symbol_table_init(SymbolTable *table);
void symbol_table_free(SymbolTable *table);
int is_keyword(char *word);
int is_protected_identifier(char *word);
char *get_obfuscated_name(SymbolTable *table, char *identifier);

#endif