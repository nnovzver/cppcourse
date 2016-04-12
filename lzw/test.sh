#!/bin/bash

make

head -c 1M /dev/urandom > random.file
./lzw -c random.file
./lzw -u random.file.bin
diff random.file random.file.bin.U
if [ $? -eq 0 ]
then
  echo "OK"
else
  echo "FAIL!"
fi
rm random.file{,.bin,.bin.U}
