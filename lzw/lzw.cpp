#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include <map>
#include <vector>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <climits>

#include "lzw.h"

namespace {

#define CODEBIT_MAX 16
#define CODEBIT_MIN 9

#define MASK(bits) ((1 << bits) - 1)
#define MASK_SHIFT(bits, shift) (((1 << bits) - 1) << shift)

struct BinaryArray {
  BinaryArray() : bitIdx(0) {}

  void addBits(uint16_t bits, size_t bitSize)
  {
    assert(bitSize <= CODEBIT_MAX);

    size_t freeBits = buff.size() * CHAR_BIT - bitIdx;

    if (freeBits == 0) {
      buff.push_back(0);
      freeBits = CHAR_BIT;
    }

    size_t bitsRemain = bitSize;

    while (bitsRemain) {
      if (bitsRemain <= freeBits) {
        uint8_t &last = buff.back();
        last |= (bits & MASK(bitsRemain)) << (freeBits - bitsRemain);
        bitsRemain = 0;
      }
      else if (bitsRemain > freeBits) {
        uint8_t &last = buff.back();
        last |= (bits & MASK_SHIFT(freeBits, (bitsRemain - freeBits))) >> (bitsRemain - freeBits);
        bitsRemain -= freeBits;
        buff.push_back(0);
        freeBits = CHAR_BIT;
      }
    }

    bitIdx += bitSize;
  }

  uint16_t getBits(size_t bitIdx, size_t size)
  {
    assert(size <= CODEBIT_MAX);

    uint16_t res = 0;

    size_t remain = size;
    size_t vecIdx = bitIdx / CHAR_BIT;
    size_t dataBits = (CHAR_BIT - bitIdx % CHAR_BIT);
    while (remain) {
      if (remain <= dataBits) {
        res |= (buff[vecIdx] >> (dataBits - remain)) & MASK(remain);
        remain = 0;
      }
      else if (remain > dataBits) {
        res |= ((buff[vecIdx]) << (remain - dataBits)) & MASK_SHIFT(dataBits, (remain - dataBits));
        ++vecIdx;
        remain -= dataBits;
        dataBits = CHAR_BIT;
      }
    }
  }

  size_t bsize()
  {
    return buff.size() * CHAR_BIT;
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

} // namespace

int compress(char const* path)
{
  std::ifstream fin(path, std::ios::binary);
  if (fin.good()) {
    // fill dictionary
    std::map<std::string, int> dict;
    for (int i = 0; i < UCHAR_MAX + 1; ++i) dict[std::string(1, i)] = i;
    uint16_t dictSize = UCHAR_MAX + 1;

    size_t bitsInCode = CODEBIT_MIN;

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
          if (dictSize < USHRT_MAX) {
            dict[befCur] = dictSize++;
          }
          before = std::string(1, current);
          if (pow(2, bitsInCode) < dictSize && bitsInCode < CODEBIT_MAX)
            ++bitsInCode;
        }
      }
    }

    if (!before.empty()) {
      compressed.addBits(dict[before], bitsInCode);
    }

    compressed.save(std::string(path) + ".bin");
  }
  else {
    return ERR_FEXIST;
  }

  return 0;
}

int decompress(char const* path)
{
  std::ofstream fout(std::string(path) + ".U", std::ios::binary);
  if (fout.good()) {
    BinaryArray compressed;
    compressed.load(std::string(path));

    std::map<uint16_t, std::string> dict;
    for (uint16_t i = 0; i < UCHAR_MAX + 1; ++i) dict[i] = std::string(1, i);
    uint16_t dictSize = UCHAR_MAX + 1;

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
      if (pow(2, bitsInCode) <= dictSize && bitsInCode < CODEBIT_MAX)
        ++bitsInCode;
      uint16_t key = compressed.getBits(bitIdx, bitsInCode);
      // std::cout << "key " << key << " bitsInCode " << bitsInCode << std::endl;
      bitIdx += bitsInCode;
      if (dict.count(key)) {
        current = dict[key];
      }
      else if (key == dictSize) {
        current = before + before[0];
      }
      else {
        return ERR_DECOMPRESS;
      }

      fout << current;
      fout.flush();
      if (dictSize < USHRT_MAX) {
        dict[dictSize++] = before + current[0];
      }

      before = current;
    }
  }
}
