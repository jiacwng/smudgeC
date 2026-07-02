#include "scanner.h"
#include "names.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        return 1;
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
        return 0;
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
        return 0;
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
    return 0;
}



int scan_file(FILE *input_file, FILE *output_file, ScannerOptions options)
{
    SymbolTable table;
    symbol_table_init(&table);

    int ch;
    int at_line_start = 1;

    while((ch = fgetc(input_file)) != EOF)
    {
        /* Preprocessor handler*/
        
        if (at_line_start && ch == '#')
        {
            fputc(ch, output_file);

            while ((ch = fgetc(input_file)) != EOF)
            {
                fputc(ch, output_file);

                if (ch == '\n')
                {
                    break;
                }
            }

            at_line_start = 1;
            continue;
        }


        /* String Handler */
        if (ch == '"')
        {
            fputc(ch, output_file);

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
                else if (ch == '"')
                {
                    break;
                }
            }

            continue;
        }


        /* character-literal handler */
        if (ch == '\'')
        {
            fputc(ch, output_file);

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

            continue;
        }


        /* Comment Handler*/

        if (ch == '/')
        {
            int next = fgetc(input_file);

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
                    fputc(ch, output_file);
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

                at_line_start = 1;
                continue;
            }
            else if (next == '*')
            {
                if (options.strip_comments)
                {
                    int previous = 0;

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
                    fputc(ch, output_file);
                    fputc(next, output_file);

                    int previous = 0;

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

                continue;
            }
            else
            {
                if (next != EOF)
                {
                    ungetc(next, input_file);
                }
            }
        }


        /* Number handler */ 
        if (isdigit(ch))
        {
            if (handle_number(input_file, output_file, ch, options) != 0)
            {
                symbol_table_free(&table);
                return 1;
            }

            continue;
        }



        /* Identifier Handler */

        if(isalpha(ch) || ch == '_')
        {
            char *identifier = read_identifier(input_file, ch);
            if(identifier == NULL)
            {
                symbol_table_free(&table);
                return 1;
            }
            
            if (is_keyword(identifier))
            {
                fputs(identifier, output_file);
            }
            else if (is_protected_identifier(identifier))
            {
                fputs(identifier, output_file);
            }
            else
            {
                char *obfuscated_name = get_obfuscated_name(&table, identifier);
                if (obfuscated_name == NULL)
                {
                    symbol_table_free(&table);
                    free(identifier);
                    return 1;
                }
                fputs(obfuscated_name, output_file);
            }
            free(identifier);
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
    return 0;
}