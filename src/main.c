#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>


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






int main(int argc, char **argv)
{

    /* FILE MANAGEMENT ---------------------------------------------------- */

    if(argc != 2)
    {
        printf("usage: smudgec input.c\n");
        return 1;
    }

    char *input_path = argv[1];
    char *slash = strrchr(input_path,'/');
    char *filename;
    // finding the filename from the folder
    if(slash != NULL)
    {
        filename = slash + 1;
    }
    else
    {  
        filename = input_path;
    }
    printf("filename: %s\n", filename);
    // finding the extension type from the filename
    char *dot = strrchr(filename,'.');
    if(dot == NULL)
    {
        printf("expected a .c file\n");
        return 1;
    }
    int name_length = dot - filename;
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "out/%.*s_obfuscated.c", name_length, filename);
    printf("output path: %s\n", output_path);

    FILE *input_file = fopen(input_path, "r");
    if(input_file == NULL)
    {
        perror("smudgec");
        return 1;
    }
    printf("opened: %s\n",input_path);

    mkdir("out",0755);

    FILE *output_file = fopen(output_path, "w");
    if(output_file == NULL)
    {
        perror("smudgec");
        fclose(input_file);
        return 1;
    }

    /* ----------------------------------------------------------------- */


    int ch;
    int at_line_start = 1;
    char original_names[128][128];
    char obfuscated_names[128][128];
    int symbol_count = 0;   


    /* SCAN LOOP */

    while((ch = fgetc(input_file)) != EOF)
    {
        /* Preprocessor handler */

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
                // Filling lookup table for unique names we've given to identifiers and mapping them to an obfuscated table
                int found_index = -1;
                for (int i = 0; i < symbol_count; i++)
                {
                    if (strcmp(original_names[i],identifier) == 0)
                    {
                    found_index = i;
                    break;
                    }
                }
                if(found_index != -1)
                {
                    fputs(obfuscated_names[found_index], output_file);
                }
                else
                {
                    snprintf(obfuscated_names[symbol_count],sizeof(obfuscated_names[symbol_count]), "_sm%d", symbol_count);
                    strcpy(original_names[symbol_count],identifier);
                    fputs(obfuscated_names[symbol_count], output_file);
                    symbol_count += 1;
                }
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
    printf("wrote: %s\n", output_path);


    
    fclose(input_file);
    fclose(output_file);
    return 0;
}

