#include "names.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* keyword identification to separate them from identifiers */

int is_keyword(char *word)
{
    static const char *keywords[] = {
        "int",
        "char",
        "return",
        "void",
        "if",
        "else",
        "for",
        "while",
        "do",
        "break",
        "continue",
        "struct",
        "typedef",
        "const",
        "static",
        "unsigned",
        "signed",
        "long",
        "short",
        "float",
        "double"
    };

    int count = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/* separate protected identifier from usual identifier */

int is_protected_identifier(char *word)
{
    static const char *protected_names[] = {
        "main",
        "printf",
        "scanf",
        "malloc",
        "free",
        "NULL",
        "puts",
        "putchar",
        "getchar",
        "strlen",
        "strcmp",
        "strcpy",
        "fopen",
        "fclose",
        "fgetc",
        "fputc",
        "FILE",
        "EOF"
    };

    int count = sizeof(protected_names) / sizeof(protected_names[0]);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(word, protected_names[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}


void symbol_table_init(SymbolTable *table)
{
    table->symbols = NULL;
    table->count = 0;
    table->capacity = 0;
}

void symbol_table_free(SymbolTable *table)
{
    for (int i = 0; i < table->count; i++)
    {
        free(table->symbols[i].original);
        free(table->symbols[i].obfuscated);
    }
    free(table->symbols);

    symbol_table_init(table);
}


char *get_obfuscated_name(SymbolTable *table, char *identifier)
{
    for (int i = 0; i < table->count; i++)
    {
        if (strcmp(table->symbols[i].original, identifier) == 0)
        {
            return table->symbols[i].obfuscated;
        }
    }

    // grow table if full
    if (table->count == table->capacity)
    {
        if (table->capacity == 0)
        {
            table->capacity += 8;
        }
        else
        {
            table->capacity = table->capacity * 2;
        }
        Symbol *temp_symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);

        if (temp_symbols == NULL)
        {
            return NULL;
        }

        table->symbols = temp_symbols;
    }

    // store new symbol
    char *original = malloc(strlen(identifier) + 1);
    if (original == NULL)
    {
        return NULL;
    }
    strcpy(original,identifier);

    char temp_name[32];
    snprintf(temp_name, sizeof(temp_name), "_sm%d", table->count);
    char *obfuscated = malloc(strlen(temp_name) + 1);
    if (obfuscated == NULL)
    {
        free(original);
        return NULL;
    }
    strcpy(obfuscated,temp_name);

    table->symbols[table->count].original = original;
    table->symbols[table->count].obfuscated = obfuscated;
    table->count += 1;

    return table->symbols[table->count - 1].obfuscated;

}