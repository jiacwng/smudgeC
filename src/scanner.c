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