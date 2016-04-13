#ifndef LZW_H
#define LZW_H

#define ERR_FEXIST     1
#define ERR_DECOMPRESS 2

int compress(char const* path);

int decompress(char const* path);

#endif
