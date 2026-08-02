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

typedef struct
{
    char **names;
    int count;
    int capacity;
} NameSet;

void name_set_init(NameSet *set);
void name_set_free(NameSet *set);
int  name_set_add(NameSet *set, const char *name);       /* 1 on success, 0 on alloc failure */
int  name_set_contains(const NameSet *set, const char *name);  /* 1 if present, else 0 */

void symbol_table_init(SymbolTable *table);
void symbol_table_free(SymbolTable *table);
int is_keyword(char *word);
int is_protected_identifier(char *word);
char *get_obfuscated_name(SymbolTable *table, char *identifier);

#endif