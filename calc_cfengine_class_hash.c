#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <class.h>

/**
 * This is a small utility to calculate hashes for CFEngine
 * class names. I used this to detect hash collisions and 
 * implement alternative hashing method as described in the
 * below pull request
 * https://github.com/cfengine/core/pull/2489
 */
static unsigned int GetHash(ClassTable *t, const char *name)
{
    ClassTablePut(t, NULL, name, false, CONTEXT_SCOPE_NAMESPACE, NULL);
    Class *cls = ClassTableGet(t, NULL, name);

    return (cls ? cls->hash : 0);
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: ./calc_cfengine_class_hash filename\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r");
    if ( fp == NULL)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    //assuming class names can be 1024 characters long at max
    char line[1024] = {'\0'};

    ClassTable *t = ClassTableNew();
    if(!t)
    {
        printf("Could not create ClassTable, exiting\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof line, fp) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        printf("%s\t%u\n", line, GetHash(t, line));
    }

    //clean up
    fclose(fp);
    ClassTableDestroy(t);
    exit(EXIT_SUCCESS);
}
