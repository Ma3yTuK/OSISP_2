#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "des.h"


const char* encrypt_flag = "-e";
const char* decrypt_flag = "-d";
const unsigned char key[] = { 1, 2, 3, 4, 5, 6, 7 };


int main(int argc, char *argv[])
{
    const int character_size = 1;

    if (argc > 3 || argc < 2)
    {
        fprintf(stderr, "%s", "Wrong number of arguments!\n");
        return 1;
    }

    void (*process)(unsigned char*, unsigned char**, unsigned char*);

    if (!strcmp(argv[1], encrypt_flag))
    {
        process = encrypt;
    }
    else if (!strcmp(argv[1], decrypt_flag))
    {
        process = decrypt;
    }
    else
    {
        fprintf(stderr, "%s", "Unrecognized flag!\n");
        return 1;
    }

    FILE* file;
    if (argc == 3)
        file = fopen(argv[2], "w");
    else
        file = stdin;

    unsigned char buffer[value_size_in_bytes];
    unsigned char encrypted_buffer[value_size_in_bytes];
    unsigned char** subkey_buffer = (unsigned char**)malloc(round_count * sizeof(unsigned char*));
    for (int i = 0; i < round_count; i++)
    {
        subkey_buffer[i] = (unsigned char*)malloc(subkey_size_in_bytes * sizeof(unsigned char));
    }

    int read_count;

    getSubKeys(key, subkey_buffer);

    while ((read_count = fread(buffer, character_size, value_size_in_bytes / character_size, file)) == value_size_in_bytes / character_size)
    {
        process(buffer, subkey_buffer, encrypted_buffer);
        fwrite(encrypted_buffer, character_size, value_size_in_bytes / character_size, stdout);
    }

    if (read_count)
    {
        printf("%d", read_count);
        for (int i = read_count * character_size; i < value_size_in_bytes; i++)
        {
            buffer[i] = 0;
        }

        process(buffer, subkey_buffer, encrypted_buffer);
        fwrite(encrypted_buffer, character_size, value_size_in_bytes / character_size, stdout);
    }

    for (int i = 0; i < round_count; i++)
    {
        free(subkey_buffer[i]);
    }
    free(subkey_buffer);

    return 0;
}