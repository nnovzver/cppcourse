#include <iostream>
#include <cstring>
#include "lzw.h"


namespace {

void printUsage(char const *name)
{
  std::cerr << "Usage:\n"
            << '\t' << name << " -c fromfile tofile\tCompress fromfile into tofile\n"
            << '\t' << name << " -u fromfile tofile\tUncompress fromfile into tofile\n";
}

} // namespace


int main(int argc, char **argv)
{

  if (argc != 4) {
    printUsage(argv[0]);
  }

  // compress
  if (std::strncmp(argv[1], "-c", 2) == 0) {
    int ret = compress(argv[2], argv[3]);
    if (ret == ERR_FEXIST)
      std::cerr << "Error: file doesn't exist" << std::endl;
  }

  // decompress
  if (std::strncmp(argv[1], "-u", 2) == 0) {
    int ret = decompress(argv[2], argv[3]);
    if (ret == ERR_DECOMPRESS)
      std::cerr << "Error: decompress" << std::endl;
  }
}
