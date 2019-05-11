/* Read/write files bit-by-bit.
 *
 * Author: Joey Rees-Hill
 *
 * Date: May 2019
 */

#ifndef BITBUFFER_H
#define BITBUFFER_H

class BitBuffer {
    public:
        BitBuffer(FILE*);
        ~BitBuffer(void);
        bool getBit(void);
        void putBit(bool);
        void flush(void);

    private:
        FILE *file;
        unsigned char buffer;
        int bit_count;
};

#endif
