#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <cstring>
#include <map>
#include <vector>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <cmath>

namespace {

void printUsage(char const *name)
{
  std::cout << "Usage:\n"
            << '\t' << name << " -c filename\tCompress file into filename.bin\n"
            << '\t' << name << " -u filename.bin\tUncompress file into filename.bin.U"
            << std::endl;
}

#define MASK(bits) ((1 << bits) - 1)
#define MASK_SHIFT(bits, shift) (((1 << bits) - 1) << shift)

struct BinaryArray {
  BinaryArray() : bitIdx(0) {}

  void addBits(uint16_t bits, size_t bitSize)
  {
    assert(bitSize <= 16);

    size_t freeBits = buff.size() * 8 - bitIdx;

    if (freeBits == 0) {
      buff.push_back(0);
      freeBits = 8;
    }

    size_t bitsRemain = bitSize;

    while (bitsRemain) {
      if (bitsRemain == freeBits) {
        uint8_t &last = buff.back();
        last |= bits & MASK(bitsRemain);
        bitsRemain = 0;
      }
      else if (bitsRemain < freeBits) {
        uint8_t &last = buff.back();
        last |= (bits & MASK(bitsRemain)) << (freeBits - bitsRemain);
        bitsRemain = 0;
      }
      else if (bitsRemain > freeBits) {
        uint8_t &last = buff.back();
        last |= (bits & MASK_SHIFT(freeBits, (bitsRemain - freeBits))) >> (bitsRemain - freeBits);
        bitsRemain -= freeBits;
        buff.push_back(0);
        freeBits = 8;
      }
    }

    bitIdx += bitSize;
  }

  uint16_t getBits(size_t idx, size_t size)
  {
    assert(size <= 16);

    uint16_t res = 0;

    size_t remain = size;
    size_t vecIdx = idx / 8;
    size_t dataBits = (8 - idx % 8);
    while (remain) {
      if (dataBits == remain) {
        res |= buff[vecIdx] & MASK(remain);
        remain = 0;
      }
      else if (remain < dataBits) {
        res |= (buff[vecIdx] >> (dataBits - remain)) & MASK(remain);
        remain = 0;
      }
      else if (remain > dataBits) {
        res |= ((buff[vecIdx]) << (remain - dataBits)) & MASK_SHIFT(dataBits, (remain - dataBits));
        ++vecIdx;
        remain -= dataBits;
        dataBits = 8;
      }
    }
  }

  size_t bsize()
  {
    return buff.size() * 8;
  }

  int save(std::string filename)
  {
    std::ofstream f(filename, std::ios::binary);
    if (f.good()) {
      std::copy(buff.begin(), buff.end(), std::ostreambuf_iterator<char>(f));
    }
  }

  int load(std::string filename)
  {
    std::ifstream f(filename, std::ios::binary);
    if (f.good()) {
      buff.clear();
      std::copy(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>(),
                std::back_inserter(buff));
    }
  }

  void print() {
    for (uint8_t e: buff) {
      std::cout << std::hex << "0x" << (int)e << ", ";
    }
    std::cout << std::endl;
  }

private:
  size_t bitIdx;
  std::vector<uint8_t> buff;
};

void testBinaryArray() {
  BinaryArray ba;
  ba.addBits(0xAA, 8);
  ba.addBits(0xBB, 8);
  ba.addBits(0x3, 2);
  ba.addBits(0x1, 2);
  ba.addBits(0x3, 2);
  ba.addBits(0x1, 2);
  ba.addBits(0xAAB, 12);
  ba.addBits(0x1, 4);
  ba.print();
  std::cout << std::hex << "0x" << ba.getBits(0, 8) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(8, 8) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(16, 2) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(18, 2) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(20, 2) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(22, 2) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(24, 12) << std::endl;
  std::cout << std::hex << "0x" << ba.getBits(36, 4) << std::endl;
}


} // namespace


int main(int argc, char **argv)
{

  if (argc != 3) {
    printUsage(argv[0]);
  }

  // compress
  if (std::strncmp(argv[1], "-c", 2) == 0) {
    std::ifstream fin(argv[2], std::ios::binary);
    if (fin.good()) {
      // fill dictionary
      std::map<std::string, int> dict;
      for (int i = 0; i < 256; ++i) dict[std::string(1, i)] = i;
      uint16_t dictSize = 256;

      size_t bitsInCode = 9;

      BinaryArray compressed;
      std::string before;
      while (!fin.eof()) {
        char current = fin.get();
        if (fin.good()) {
          std::string befCur = before + current;
          if (dict.count(befCur)) {
            before = befCur;
          }
          else {
            // std::cout << "key " << dict[before] << " bitsInCode " << bitsInCode << std::endl;
            compressed.addBits(dict[before], bitsInCode);
            if (dictSize < 65535) {
              dict[befCur] = dictSize++;
            }
            before = std::string(1, current);
            if (pow(2, bitsInCode) < dictSize && bitsInCode < 16)
              ++bitsInCode;
          }
        }
      }

      if (!before.empty()) {
        compressed.addBits(dict[before], bitsInCode);
      }

      // for (int i = 0; i < 17; ++i) {
      //   int j = i * 9;
      //   std::cout << compressed.getBits(j, 9) << std::endl;
      // }
      // compressed.print();

      compressed.save(std::string(argv[2]) + ".bin");
    }
    else {
      std::cout << "Error: file doesn't exist" << std::endl;
    }
  }

  // decompress
  if (std::strncmp(argv[1], "-u", 2) == 0) {
    std::ofstream fout(std::string(argv[2]) + ".U", std::ios::binary);
    if (fout.good()) {
      BinaryArray compressed;
      compressed.load(std::string(argv[2]));

      // compressed.print();
      // for (int i = 0; i < 17; ++i) {
      //   int j = i * 9;
      //   std::cout << compressed.getBits(j, 9) << std::endl;
      // }

      std::map<uint16_t, std::string> dict;
      for (uint16_t i = 0; i < 256; ++i) dict[i] = std::string(1, i);
      uint16_t dictSize = 256;

      size_t bitsInCode = 9;
      size_t bitIdx = 0;
      if (bitIdx + bitsInCode > compressed.bsize()) {
        return -1;
      }
      std::string before = std::string(1, compressed.getBits(bitIdx, bitsInCode));
      bitIdx += bitsInCode;
      fout << before;

      std::string current;
      while (bitIdx + bitsInCode < compressed.bsize()) {
        if (pow(2, bitsInCode) <= dictSize && bitsInCode < 16)
          ++bitsInCode;
        uint16_t key = compressed.getBits(bitIdx, bitsInCode);
        // std::cout << "bitIdx " << bitIdx << " bitsInCode " << bitsInCode << std::endl;
        // std::cout << "key " << key << " bitsInCode " << bitsInCode << std::endl;
        bitIdx += bitsInCode;
        if (dict.count(key)) {
          current = dict[key];
        }
        else if (key == dictSize) {
          current = before + before[0];
        }
        else {
          throw "decompress error";
        }

        fout << current;
        fout.flush();
        if (dictSize < 65535) {
          dict[dictSize++] = before + current[0];
        }

        before = current;
      }
    }
  }
}
