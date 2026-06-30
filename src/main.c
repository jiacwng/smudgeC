#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

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

    if(slash != NULL)
    {
        filename = slash + 1;
    }
    else
    {  
        filename = input_path;
    }
    printf("filename: %s\n", filename);

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

    while((ch = fgetc(input_file)) != EOF)
    {
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

            printf("identifier: %s\n", identifier);
            fputs(identifier, output_file);
            continue;
        }
        fputc(ch, output_file); /* non identifier */
    }
    printf("wrote: %s\n", output_path);


    
    fclose(input_file);
    fclose(output_file);
    return 0;
}

