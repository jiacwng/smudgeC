#include "names.h"
#include <stdio.h>
#include <string.h>

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
    table->symbol_count = 0;
}


char *get_obfuscated_name(SymbolTable *table, char *identifier)
{
    for (int i = 0; i < table->symbol_count; i++)
    {
        if (strcmp(table->original_names[i], identifier) == 0)
        {
            return table->obfuscated_names[i];
        }
    }

    snprintf(
        table->obfuscated_names[table->symbol_count],
        sizeof(table->obfuscated_names[table->symbol_count]),
        "_sm%d",
        table->symbol_count
    );

    strcpy(table->original_names[table->symbol_count], identifier);
    table->symbol_count++;
    
    return table->obfuscated_names[table->symbol_count - 1];

}