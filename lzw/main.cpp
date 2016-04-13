#include <iostream>
#include <cstring>
#include "lzw.h"


namespace {

void printUsage(char const *name)
{
  std::cout << "Usage:\n"
            << '\t' << name << " -c filename\tCompress file into filename.bin\n"
            << '\t' << name << " -u filename.bin\tUncompress file into filename.bin.U"
            << std::endl;
}

} // namespace


int main(int argc, char **argv)
{

  if (argc != 3) {
    printUsage(argv[0]);
  }

  // compress
  if (std::strncmp(argv[1], "-c", 2) == 0) {
    int ret = compress(argv[2]);
    if (ret == ERR_FEXIST)
      std::cerr << "Error: file doesn't exist" << std::endl;
  }

  // decompress
  if (std::strncmp(argv[1], "-u", 2) == 0) {
    int ret = decompress(argv[2]);
    if (ret == ERR_DECOMPRESS)
      std::cerr << "Error: decompress" << std::endl;
  }
}
