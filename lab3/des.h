#ifndef DES_H
#define DES_H


extern const int bits_in_byte;
extern const int bits_in_tetrade;
extern const int key_size_in_bytes;
extern const int value_size_in_bytes;
extern const int round_key_size_in_bytes;
extern const int subkey_size_in_bytes;
extern const int part_size_in_bytes;
extern const int round_count;


void encrypt(unsigned char* value64, unsigned char** subkeys, unsigned char* result);
void decrypt(unsigned char* value64, unsigned char** subkeys, unsigned char* result);

#endif