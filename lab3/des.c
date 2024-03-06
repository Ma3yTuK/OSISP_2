#include "des.h"



const int bits_in_byte = 8;
const int bits_in_tetrade = 4;
const int key_size_in_bytes = 8;
const int value_size_in_bytes = 8;
const int round_key_size_in_bytes = 7;
const int subkey_size_in_bytes = 6;
const int part_size_in_bytes = 4;
const int round_count = 16;
const int round_shifts[] = {  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };
const int subkey_indexes[] = {  14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4, 
                                26, 8,  16, 7,  27, 20, 13, 2,  41, 52, 31, 37, 47, 55, 30, 40,
                                51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32  };
const int S_boxes[][4][16] =    {   {   {   14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7    },
                                        {   0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8    },
                                        {   4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0    },
                                        {   15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13    }    },
                                    {   {   15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10    },
                                        {   3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5    },
                                        {   0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15    },
                                        {   13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9    }    },
                                    {   {   10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8    },
                                        {   13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1    },
                                        {   13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7    },
                                        {   1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12    }    },
                                    {   {   7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15    },
                                        {   13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9    },
                                        {   10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4    },
                                        {   3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14    }    },
                                    {   {   2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9    },
                                        {   14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6    },
                                        {   4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14    },
                                        {   11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3    }    },
                                    {   {   12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11    },
                                        {   10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8    },
                                        {   9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6    },
                                        {   4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13    }   },
                                    {   {   4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1    },
                                        {   13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6    },
                                        {   1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2    },
                                        {   6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12    }    },
                                    {   {   13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7    },
                                        {   1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2    },
                                        {   7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8    },
                                        {   2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11    }    }   };
const int P[] = { 16, 7, 20, 21, 29, 12, 28, 17,
            1, 15, 23, 26, 5, 18, 31, 10,
            2, 8, 24, 14, 32, 27, 3, 9,
            19, 13, 30, 6, 22, 11, 4, 25    };
const int permutation_indexes[] = { 4, 0, 5, 1, 6, 2, 7, 3 };



void shiftLeft(unsigned char* data, int shift, int data_length)
{
    const unsigned char ones = 0b11111111;
    const int bits_not_remembered = bits_in_byte - shift;
    const unsigned char remembered_bits = (ones << bits_not_remembered) & ones;

    unsigned char tmp = 0;

    for (int i = data_length - 1; i >= 0; i--)
    {
        unsigned char new_byte = (data[i] << shift) | tmp;
        tmp = (data[i] & remembered_bits) >> bits_not_remembered;
        data[i] = new_byte;
    }

    data[data_length - 1] |= tmp;
}


void shiftRight(unsigned char* data, int shift, int data_length)
{
    const unsigned char ones = 0b11111111;
    const int bits_not_remembered = bits_in_byte - shift;
    const unsigned char remembered_bits = (ones >> bits_not_remembered) & ones;

    unsigned char tmp = 0;

    for (int i = 0; i < data_length; i++)
    {
        unsigned char new_byte = (data[i] >> shift) | tmp;
        tmp = (data[i] & remembered_bits) << bits_not_remembered;
        data[i] = new_byte;
    }

    data[0] |= tmp;
}


unsigned char getBit(unsigned char* data, int index)
{
    const int bit_pos = index % bits_in_byte;
    const int byte_pos = index / bits_in_byte;
    const unsigned char byte_with_bit_set = 0b10000000 >> bit_pos;
    return (data[byte_pos] & byte_with_bit_set) >> (bits_in_byte - bit_pos);
}


void setBit(unsigned char* data, int index, unsigned char value)
{
    const int bit_pos = index % bits_in_byte;
    const int byte_pos = index / bits_in_byte;

    value <<= bits_in_byte - bit_pos;

    if (value) {
        data[byte_pos] |= value;
    } else {
        data[byte_pos] &= ~value;
    }
}


void permutedChoice(unsigned char* key, unsigned char* result)
{
    unsigned char byte_with_bit_set = 0b10000000;

    for (int i = 0; i < round_key_size_in_bytes; i++)
    {
        result[round_key_size_in_bytes - 1 - i] = 0;
        for (int j = 0; j < key_size_in_bytes; j++)
        {
            result[round_key_size_in_bytes - 1 - i] |= ((key[j] & byte_with_bit_set) << i) >> (value_size_in_bytes - 1 - j);
        }
        byte_with_bit_set >>= 1;
    }

    unsigned char tmp = result[4];
    result[4] = result[6];
    result[6] = tmp;

    shiftLeft(result + 3, 4, 4);
}


void shiftRoundKey(unsigned char* last_key, int shift_size)
{
    const unsigned char D_last_bits = (0b00001111 >> (bits_in_tetrade - shift_size)) << bits_in_tetrade;
    const unsigned char C_last_bits = 0b11111111 >> (bits_in_byte - shift_size);
    const int D_last_byte = 3;
    const int C_last_byte = 6;

    shiftLeft(last_key, shift_size, key_size_in_bytes);

    unsigned char tmp = last_key[D_last_byte] & D_last_bits;
    last_key[D_last_byte] = (last_key[D_last_byte] & ~D_last_bits) | ((last_key[C_last_byte] & C_last_bits) << bits_in_tetrade);
    last_key[C_last_byte] = (last_key[C_last_byte] & ~C_last_bits) | (tmp >> bits_in_tetrade);
}


void getSubKey(unsigned char* round_key, unsigned char* result)
{
    const int round_key_size_in_bits = round_key_size_in_bytes * bits_in_byte;

    for (int i = 0; i < subkey_size_in_bytes * bits_in_byte; i++)
    {
        int index = (round_key_size_in_bits / 2 + subkey_indexes[i] - 1) % (round_key_size_in_bits - 1);
        setBit(result, i, getBit(round_key, index));
    }
}


void expansion(unsigned char* vec32, unsigned char* result)
{
    const int vec_size_in_bytes = 4;
    const int shift = 1;
    const unsigned char ones = 0b11111111;
    const int bits_not_remembered = bits_in_byte - shift;
    const unsigned char remembered_bits_left = (ones << bits_not_remembered) & ones;
    const unsigned char remembered_bits_right = (ones >> bits_not_remembered) & ones;

    const int block_size_in_bits = 6;
    const unsigned char left_bits = 0b00111111;
    const unsigned char right_bits = 0b11111100;

    unsigned char left_arr[vec_size_in_bytes];
    unsigned char right_arr[vec_size_in_bytes];

    unsigned char tmp_left = 0;
    unsigned char tmp_right = 0;

    for (int i = 0; i < vec_size_in_bytes; i++)
    {
        left_arr[vec_size_in_bytes - 1 - i] = (vec32[value_size_in_bytes - 1 - i] << shift) | tmp_left;
        right_arr[i] = (vec32[i] >> shift) | tmp_right;
        tmp_left = (vec32[vec_size_in_bytes - 1 - i] & remembered_bits_left) >> bits_not_remembered;
        tmp_right = (vec32[i] & remembered_bits_right)<< bits_not_remembered;
    }

    left_arr[vec_size_in_bytes - 1] |= tmp_left;
    right_arr[0] |= tmp_right;

    for (int i = 0; i < vec_size_in_bytes; i++)
    {
        int current_byte = 2 * block_size_in_bits * i / bits_in_byte;
        int current_bit = 2 * block_size_in_bits * i % bits_in_byte;
        unsigned char left_val = left_arr[i] & left_bits;
        unsigned right_val = right_arr[i] & right_bits;

        if (current_bit == bits_in_tetrade - 1) {
            result[current_byte] |= right_val >> bits_in_tetrade;
            result[current_byte + 1] = (right_val << bits_in_tetrade) | left_val;
        } else {
            result[current_byte] = right_val | (left_val >> bits_in_tetrade);
            result[current_byte + 1] = left_val << bits_in_tetrade;
        }
    }
}


void shrinking(unsigned char* vec48, unsigned char* result)
{
    const int block_size_in_bits = 6;

    for (int i = 0; i < part_size_in_bytes; i++)
    {
        int current_byte = 2 * i * block_size_in_bits / bits_in_byte;
        int current_bit = 2 * i * block_size_in_bits % bits_in_byte;
        unsigned char first_row;
        unsigned char first_col;
        unsigned char last_row;
        unsigned char last_col;

        if (current_bit == bits_in_tetrade - 1)
        {
            first_row = (vec48[current_byte] & 0b00001100) >> 2;
            first_col = ((vec48[current_byte] & 0b00000011) << 2) | ((vec48[current_byte + 1] & 0b11000000) >> 6);
            last_row = (vec48[current_byte + 1] & 0b00110000) >> 4;
            last_col = vec48[current_byte + 1] & 0b00001111;
        }
        else
        {
            first_row = (vec48[current_byte] & 0b11000000) >> 6;
            first_col = (vec48[current_byte] & 0b00111100) >> 2;
            last_row = vec48[current_byte] & 0b00000011;
            last_col = (vec48[current_byte + 1] & 0b11110000) >> 4;
        }
        unsigned char first_val = S_boxes[i * 2][first_row][first_col];
        unsigned char last_val = S_boxes[i * 2 + 1][last_row][last_col];
        result[i] = (first_val << 4) | last_val;
    }
}


void pPermutation(unsigned char* vec32, unsigned char* result)
{
    for (int i = 0; i < part_size_in_bytes * bits_in_byte; i++)
    {
        setBit(result, i, getBit(vec32, P[i]));
    }
}


void startingPermutation(unsigned char* value, unsigned char* result)
{
    unsigned char byte_with_bit_set = 0b10000000;

    for (int i = 0; i < value_size_in_bytes; i++)
    {
        result[permutation_indexes[i]] = 0;
        for (int j = 0; j < value_size_in_bytes; j++)
        {
            result[permutation_indexes[i]] |= ((value[j] & byte_with_bit_set) << i) >> (value_size_in_bytes - 1 - j);
        }
        byte_with_bit_set >>= 1;
    }
}


void endingPermutation(unsigned char* value, unsigned char* result)
{
    unsigned char byte_with_bit_set = 0b00000001;

    for (int i = 0; i < value_size_in_bytes; i++)
    {
        result[i] = 0;
        for (int j = 0; j < value_size_in_bytes; j++)
        {
            result[i] |= ((value[permutation_indexes[j]] & byte_with_bit_set) >> i) << j;
        }
        byte_with_bit_set <<= 1;
    }
}


void xor(unsigned char* vec1, unsigned char* vec2, unsigned char* result, int data_length)
{
    for (int i = 0; i < data_length; i++)
    {
        result[i] = vec1[i] ^ vec2[i];
    }
}


void feistel(unsigned char* vec32, unsigned char* subkey, unsigned char* result)
{
    unsigned char tmp1[subkey_size_in_bytes];
    expansion(vec32, tmp1);
    xor(tmp1, subkey, tmp1, subkey_size_in_bytes);
    unsigned char tmp2[part_size_in_bytes];
    shrinking(tmp1, tmp2);
    pPermutation(tmp2, result);
}


void getSubKeys(unsigned char* key, unsigned char** result)
{
    unsigned char round_key[key_size_in_bytes];
    permutedChoice(key, round_key);

    for (int i = 0; i < round_count; i++)
    {
        shiftRoundKey(round_key, round_shifts[i]);
        getSubKey(round_key, result[i]);
    }
}


void process(unsigned char* value64, unsigned char** subkeys, unsigned char* result, int encrypt)
{
    unsigned char current[value_size_in_bytes];
    startingPermutation(value64, current);
    unsigned char* left_part;
    unsigned char* right_part;

    if (round_count % 2 == 0)
    {
        left_part = current;
        right_part = current + value_size_in_bytes / 2;
    }
    else
    {
        left_part = current + value_size_in_bytes / 2;
        right_part = current;
    }

    for (int i = 0; i < round_count; i++)
    {
        unsigned char* tmp = left_part;
        left_part = right_part;
        if (encrypt)
            feistel(right_part, subkeys[i], result);
        else
            feistel(right_part, subkeys[round_count - 1 - i], result);
        right_part = tmp;
        xor(right_part, result, right_part, part_size_in_bytes);
    }

    endingPermutation(current, result);
}


void encrypt(unsigned char* value64, unsigned char** subkeys, unsigned char* result)
{
    return process(value64, subkeys, result, 1);
}


void decrypt(unsigned char* value64, unsigned char** subkeys, unsigned char* result)
{
    return process(value64, subkeys, result, 0);
}