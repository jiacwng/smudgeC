#include "scanner.h"
#include "names.h"

#include <ctype.h>
#include <stdio.h>

int scan_file(FILE *input_file, FILE *output_file)
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
                at_line_start = 1;
                continue;
            }
            else if (next == '*')
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
            char identifier[128];
            int length = 0;
            identifier[length] = ch;
            length++;

            while ((ch = fgetc(input_file)) != EOF && (isalnum(ch) || ch == '_'))
            {
                identifier[length] = ch;
                length++;
            }

            identifier[length] = '\0';
            if (ch != EOF)
            {
                ungetc(ch, input_file);
            }
            
            if (is_keyword(identifier))
            {
                printf("keyword: %s\n", identifier);
                fputs(identifier, output_file);
            }
            else if (is_protected_identifier(identifier))
            {
                printf("protected: %s\n", identifier);
                fputs(identifier, output_file);
            }
            else
            {
                char *obfuscated_name = get_obfuscated_name(&table, identifier);
                fputs(obfuscated_name, output_file);
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
    return 0;
}