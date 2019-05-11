/* Read and write files bit-by-bit.
 *
 * Author: Joey Rees-Hill
 *
 * Date: May 2019
 */

#include <cstdio>
#include <string>
#include <climits>
#include "BitBuffer.h"

BitBuffer::BitBuffer(FILE* f) {
    file = f;
    bit_count = 0;
    buffer = 0;
}

BitBuffer::~BitBuffer() {
}

bool BitBuffer::getBit() {
    if (0 == bit_count) {
        int readVal = fgetc(file);
        buffer = readVal != EOF ? readVal : 0;
        bit_count = CHAR_BIT;
    }

    return buffer & (0x1 << --bit_count);
}

void BitBuffer::putBit(bool bit) {
    buffer <<= 1;
    buffer |= bit;
    bit_count++;

    if (CHAR_BIT == bit_count) {
        fputc(buffer, file);
        bit_count = 0;
    }
}

void BitBuffer::flush() {
    while (bit_count != 0) {
        putBit(false);
    }
}
