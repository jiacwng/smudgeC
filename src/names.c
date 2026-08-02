#include "names.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* keyword identification to separate them from identifiers */

int is_keyword(char *word)
{
    static const char *keywords[] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "inline",
    "int",
    "long",
    "register",
    "restrict",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
    "_Bool",
    "_Complex",
    "_Imaginary"
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

void name_set_init(NameSet *set)
{
    set->names = NULL;
    set->count = 0;
    set->capacity = 0;
}

void name_set_free(NameSet *set)
{
    for (int i = 0; i < set->count; i++)
    {
        free(set->names[i]);
    }
    free(set->names);

    name_set_init(set);
}

int name_set_contains(const NameSet *set, const char *name)
{
    for (int i = 0; i < set->count; i++)
    {
        if (strcmp(set->names[i], name) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int name_set_add(NameSet *set, const char *name)
{
    if (name_set_contains(set, name))
    {
        return 1;
    }

    if (set->count == set->capacity)
    {
        if (set->capacity == 0)
        {
            set->capacity = 8;
        }
        else
        {
            set->capacity = set->capacity * 2;
        }

        char **temp = realloc(set->names, sizeof(char *) * set->capacity);
        if (temp == NULL)
        {
            return 0;
        }

        set->names = temp;
    }

    char *copy = malloc(strlen(name) + 1);
    if (copy == NULL)
    {
        return 0;
    }
    strcpy(copy, name);

    set->names[set->count] = copy;
    set->count += 1;

    return 1;
}

int load_protected_names(const char *path, NameSet *set)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return 0;
    }

    char line[128];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        int i = 0;
        while (line[i] == ' ' || line[i] == '\t')
        {
            i++;
        }

        if (line[i] == '#' || line[i] == '\n' || line[i] == '\r' || line[i] == '\0')
        {
            continue;
        }

        int start = i;
        while (isalnum((unsigned char)line[i]) || line[i] == '_')
        {
            i++;
        }

        if (i == start)
        {
            continue;
        }

        line[i] = '\0';

        if (!name_set_add(set, line + start))
        {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int collect_header_identifiers(const char *path, NameSet *set)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return 0;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '/')
        {
            int next = fgetc(file);
            if (next == '/')
            {
                while ((ch = fgetc(file)) != EOF && ch != '\n')
                {
                }
                continue;
            }
            if (next == '*')
            {
                int previous = 0;
                while ((ch = fgetc(file)) != EOF)
                {
                    if (previous == '*' && ch == '/')
                    {
                        break;
                    }
                    previous = ch;
                }
                continue;
            }
            if (next != EOF)
            {
                ungetc(next, file);
            }
            continue;
        }

        if (ch == '"' || ch == '\'')
        {
            int quote = ch;
            while ((ch = fgetc(file)) != EOF)
            {
                if (ch == '\\')
                {
                    fgetc(file);
                    continue;
                }
                if (ch == quote)
                {
                    break;
                }
            }
            continue;
        }

        if (isalpha(ch) || ch == '_')
        {
            char word[128];
            int length = 0;
            word[length] = (char)ch;
            length += 1;

            while ((ch = fgetc(file)) != EOF && (isalnum(ch) || ch == '_'))
            {
                if (length < (int)sizeof(word) - 1)
                {
                    word[length] = (char)ch;
                    length += 1;
                }
            }
            if (ch != EOF)
            {
                ungetc(ch, file);
            }
            word[length] = '\0';

            if (!is_keyword(word))
            {
                if (!name_set_add(set, word))
                {
                    fclose(file);
                    return 0;
                }
            }
            continue;
        }
    }

    fclose(file);
    return 1;
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

void symbol_table_write_map(const SymbolTable *table, FILE *map_file)
{
    for (int i = 0; i < table->count; i++)
    {
        fprintf(map_file, "%s -> %s\n", table->symbols[i].original, table->symbols[i].obfuscated);
    }
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