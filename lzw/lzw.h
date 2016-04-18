#ifndef LZW_H
#define LZW_H

#define ERR_FEXIST     1
#define ERR_DECOMPRESS 2

int compress(char const*, char const*);

int decompress(char const*, char const*);

#endif
