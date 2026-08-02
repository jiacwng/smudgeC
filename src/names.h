#ifndef NAMES_H
#define NAMES_H

#include <stdio.h>

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
int  load_protected_names(const char *path, NameSet *set);     /* 1 on success, 0 on failure */
int  collect_header_identifiers(const char *path, NameSet *set);  /* 1 on success, 0 on failure */

void symbol_table_init(SymbolTable *table);
void symbol_table_free(SymbolTable *table);
void symbol_table_write_map(const SymbolTable *table, FILE *map_file);
int is_keyword(char *word);
char *get_obfuscated_name(SymbolTable *table, char *identifier, const char *prefix);

#endif