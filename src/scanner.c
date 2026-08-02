#include "scanner.h"
#include "names.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void scanner_options_init(ScannerOptions *options)
{
    options->strip_comments = 0;
    options->encode_ints = 0;
    options->encode_strings = 0;
}


char *read_identifier(FILE *input_file,int first_ch)
{
    int buffer_size = 16;
    int length = 0;
    int ch;
    char *buffer = malloc(buffer_size*sizeof(char));
    if (buffer == NULL)
    {
        return NULL;
    }
    buffer[length] = first_ch;
    length += 1;

    while ((ch = fgetc(input_file)) != EOF)
    {
        if (!(isalnum(ch) || ch == '_'))
        {
            ungetc(ch, input_file);
            break;
        }
        if (length == buffer_size - 1)
        {
            buffer_size = buffer_size*2;
            char *temp_buffer = realloc(buffer, buffer_size);
            if (temp_buffer == NULL)
            {
                free(buffer);
                return NULL;
            }
            buffer = temp_buffer;
        }
        buffer[length] = ch;
        length += 1;
    }

    buffer[length] = '\0';
    return buffer;
}

static char *read_number(FILE *input_file, int first_ch)
{
    int buffer_size = 16;
    int length = 0;
    int ch;
    char *buffer = malloc(buffer_size * sizeof(char));

    if (buffer == NULL)
    {
        return NULL;
    }

    buffer[length] = first_ch;
    length += 1;

    while ((ch = fgetc(input_file)) != EOF)
    {
        if (!isdigit(ch))
        {
            ungetc(ch, input_file);
            break;
        }

        if (length == buffer_size - 1)
        {
            buffer_size *= 2;

            char *temp_buffer = realloc(buffer, buffer_size);
            if (temp_buffer == NULL)
            {
                free(buffer);
                return NULL;
            }

            buffer = temp_buffer;
        }

        buffer[length] = ch;
        length += 1;
    }

    buffer[length] = '\0';
    return buffer;
}

static int choose_integer_key(int number)
{
    int key = (number * 31) % 97;
    if (key < 0)
    {
        key = -key;
    }
    return key + 17;
}



static void write_encoded_integer(FILE *output_file, const char *number_text)
{
    int number = atoi(number_text);
    if (number == 0)
    {
        fputs("0", output_file);
        return;
    }

    int key = choose_integer_key(number);
    int encoded = number ^ key;
    fprintf(output_file, "((%d ^ %d))", encoded, key);
}

static int handle_number(
    FILE *input_file,
    FILE *output_file,
    int first_ch,
    ScannerOptions options
)
{
    char* number = read_number(input_file, first_ch);
    if (number == NULL)
    {
        return SCANNER_ERROR;
    }

    int next = fgetc(input_file);

    if (next != EOF && (isalpha(next) || next == '_'))
    {
        fputs(number, output_file);
        fputc(next, output_file);

        while ((next = fgetc(input_file)) != EOF)
        {
            if (!(isalnum(next) || next == '_'))
            {
                ungetc(next, input_file);
                break;
            }

            fputc(next, output_file);
        }

        free(number);
        return SCANNER_OK;
    }

    if (next == '.')
    {
        fputs(number, output_file);
        fputc(next, output_file);

        while ((next = fgetc(input_file)) != EOF)
        {
            if (!isdigit(next))
            {
                ungetc(next, input_file);
                break;
            }

            fputc(next, output_file);
        }

        free(number);
        return SCANNER_OK;
    }

    if (next != EOF)
    {
        ungetc(next, input_file);
    }



    if (options.encode_ints == 1)
    {
        write_encoded_integer(output_file, number);
    }
    else
    {
        fputs(number, output_file);
    }

    free(number);
    return SCANNER_OK;
}


static void write_encoded_string_char(FILE *output_file, int ch)
{
    fprintf(output_file, "\\x%02X", (unsigned char)ch);
}

static int handle_string(
    FILE *input_file,
    FILE *output_file,
    ScannerOptions options
)
{
    int ch;

    fputc('"', output_file);

    while ((ch = fgetc(input_file)) != EOF)
    {
        if (ch == '\\')
        {
            fputc(ch, output_file);

            ch = fgetc(input_file);
            if (ch == EOF)
            {
                break;
            }

            fputc(ch, output_file);
        }
        else if (ch == '"')
        {
            fputc(ch, output_file);
            break;
        }
        else if (options.encode_strings)
        {
            write_encoded_string_char(output_file, ch);
        }
        else
        {
            fputc(ch, output_file);
        }
    }

    return SCANNER_OK;
}

static int handle_char_literal(FILE *input_file, FILE *output_file)
{
    int ch;

    fputc('\'', output_file);

    while ((ch = fgetc(input_file)) != EOF)
    {
        fputc(ch, output_file);

        if (ch == '\\')
        {
            ch = fgetc(input_file);
            if (ch == EOF)
            {
                break;
            }

            fputc(ch, output_file);
        }
        else if (ch == '\'')
        {
            break;
        }
    }

    return SCANNER_OK;
}

static int handle_comment(
    FILE *input_file,
    FILE *output_file,
    ScannerOptions options,
    int *handled_comment,
    int *at_line_start
)
{
    int ch;
    int next = fgetc(input_file);

    *handled_comment = 0;

    if (next == '/')
    {
        if (options.strip_comments)
        {
            while ((ch = fgetc(input_file)) != EOF)
            {
                if (ch == '\n')
                {
                    fputc('\n', output_file);
                    break;
                }
            }
        }
        else
        {
            fputc('/', output_file);
            fputc(next, output_file);

            while ((ch = fgetc(input_file)) != EOF)
            {
                fputc(ch, output_file);

                if (ch == '\n')
                {
                    break;
                }
            }
        }

        *at_line_start = 1;
        *handled_comment = 1;
        return SCANNER_OK;
    }

    if (next == '*')
    {
        int previous = 0;

        if (options.strip_comments)
        {
            while ((ch = fgetc(input_file)) != EOF)
            {
                if (previous == '*' && ch == '/')
                {
                    break;
                }

                previous = ch;
            }

            fputc(' ', output_file);
        }
        else
        {
            fputc('/', output_file);
            fputc(next, output_file);

            while ((ch = fgetc(input_file)) != EOF)
            {
                fputc(ch, output_file);

                if (previous == '*' && ch == '/')
                {
                    break;
                }

                previous = ch;
            }
        }

        *handled_comment = 1;
        return SCANNER_OK;
    }

    if (next != EOF)
    {
        ungetc(next, input_file);
    }

    return SCANNER_OK;
}

static void record_macro_name(const char *line, NameSet *macros)
{
    int i = 0;

    while (line[i] == ' ' || line[i] == '\t')
    {
        i++;
    }

    const char *keyword = "define";
    int k = 0;
    while (keyword[k] != '\0' && line[i] == keyword[k])
    {
        i++;
        k++;
    }

    if (keyword[k] != '\0')
    {
        return;
    }

    if (line[i] != ' ' && line[i] != '\t')
    {
        return;
    }

    while (line[i] == ' ' || line[i] == '\t')
    {
        i++;
    }

    int start = i;
    while (isalnum((unsigned char)line[i]) || line[i] == '_')
    {
        i++;
    }

    if (i == start)
    {
        return;
    }

    int name_length = i - start;
    char *name = malloc(name_length + 1);
    if (name == NULL)
    {
        return;
    }

    memcpy(name, line + start, name_length);
    name[name_length] = '\0';

    name_set_add(macros, name);
    free(name);
}

static int handle_preprocessor(FILE *input_file, FILE *output_file, NameSet *macros)
{
    int ch;
    int buffer_size = 32;
    int length = 0;
    char *buffer = malloc(buffer_size);

    if (buffer == NULL)
    {
        return SCANNER_ERROR;
    }

    fputc('#', output_file);

    while ((ch = fgetc(input_file)) != EOF)
    {
        if (length >= buffer_size - 2)
        {
            buffer_size = buffer_size * 2;
            char *temp = realloc(buffer, buffer_size);
            if (temp == NULL)
            {
                free(buffer);
                return SCANNER_ERROR;
            }
            buffer = temp;
        }

        buffer[length] = ch;
        length += 1;

        if (ch == '\\')
        {
            int next = fgetc(input_file);
            if (next == EOF)
            {
                break;
            }

            buffer[length] = next;
            length += 1;
            continue;
        }

        if (ch == '\n')
        {
            break;
        }
    }

    buffer[length] = '\0';

    fputs(buffer, output_file);
    record_macro_name(buffer, macros);

    free(buffer);
    return SCANNER_OK;
}

static int handle_identifier(
    FILE *input_file,
    FILE *output_file,
    int first_ch,
    SymbolTable *table,
    NameSet *macros
)
{
    char *identifier = read_identifier(input_file, first_ch);
    if(identifier == NULL)
    {
        return SCANNER_ERROR;
    }

    if (is_keyword(identifier) || is_protected_identifier(identifier) || name_set_contains(macros, identifier))
    {
        fputs(identifier, output_file);
    }
    else
    {
        char *obfuscated_name = get_obfuscated_name(table, identifier);
        if (obfuscated_name == NULL)
        {
            free(identifier);
            return SCANNER_ERROR;
        }
        fputs(obfuscated_name, output_file);
    }
    free(identifier);
    return SCANNER_OK;
}




int scan_file(FILE *input_file, FILE *output_file, ScannerOptions options)
{
    SymbolTable table;
    symbol_table_init(&table);

    NameSet macros;
    name_set_init(&macros);

    int ch;
    int at_line_start = 1;

    while((ch = fgetc(input_file)) != EOF)
    {
        /* Preprocessor handler*/
        
        if (at_line_start && ch == '#')
        {
            if (handle_preprocessor(input_file, output_file, &macros) != SCANNER_OK)
            {
                symbol_table_free(&table);
                name_set_free(&macros);
                return SCANNER_ERROR;
            }

            at_line_start = 1;
            continue;
        }


        /* String Handler */
        if (ch == '"')
        {
            if (handle_string(input_file, output_file, options) != SCANNER_OK)
            {
                symbol_table_free(&table);
                name_set_free(&macros);
                return SCANNER_ERROR;
            }

            continue;
        }


        /* character-literal handler */
        if (ch == '\'')
        {
            if (handle_char_literal(input_file, output_file) != SCANNER_OK)
            {
                symbol_table_free(&table);
                name_set_free(&macros);
                return SCANNER_ERROR;
            }

            continue;
        }


        /* Comment Handler*/
        if (ch == '/')
        {
            int handled_comment;

            if (handle_comment(input_file, output_file, options, &handled_comment, &at_line_start) != SCANNER_OK)
            {
                symbol_table_free(&table);
                name_set_free(&macros);
                return SCANNER_ERROR;
            }


            /* If the slash entered the handler helper and handled_comment is not 1
            it means it wasn't a comment slash, scan_file should continue normally */
            if (handled_comment) 
            {
                continue;
            }
        }


        /* Number handler */ 
        if (isdigit(ch))
        {
            if (handle_number(input_file, output_file, ch, options) != SCANNER_OK)
            {
                symbol_table_free(&table);
                name_set_free(&macros);
                return SCANNER_ERROR;
            }

            continue;
        }



        /* Identifier Handler */

        if(isalpha(ch) || ch == '_')
        {
            if (handle_identifier(input_file, output_file, ch, &table, &macros) != SCANNER_OK)
            {
                symbol_table_free(&table);
                name_set_free(&macros);
                return SCANNER_ERROR;
            }

            continue;

        }



        fputc(ch, output_file); /* non identifier */
        if (ch == '\n')
        {
            at_line_start = 1;
        }
        else
        {
            at_line_start = 0;
        }
    }   
    symbol_table_free(&table);
    name_set_free(&macros);
    return SCANNER_OK;
}
