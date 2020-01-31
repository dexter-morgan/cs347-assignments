// A C/C++ program for splitting a string
// using strtok()
#include <stdio.h>
#include <string.h>

int main()
{
    char str[] = "Geeks yoyoyo ; for this one ; Geeks";

    printf("string before : %s\n", str);
    // Returns first token
    char *token = strtok(str, ";");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, ";");
    }

    printf("string after : %s\n", str);

    return 0;
}
