#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <cstdint>
#include <cassert>

class Hanoi
{
  public:
    Hanoi(uint8_t disks) : disks(disks), steps(0)
    {
        for (uint8_t disk = disks; disk > 0; --disk)
        {
            pins[0].push_back(disk);
        }
        maxDiskWidth = diskWidth(disks);
    }

    void render()
    {
        for (uint8_t height = disks; height > 0; --height)
        {
            for (uint8_t pin = 0; pin < 3; ++pin)
            {
                if (pins[pin].size() >= height)
                {
                    uint8_t dw      = diskWidth(pins[pin][height - 1]);
                    uint8_t padding = (maxDiskWidth - dw) / 2;
                    std::cout << std::string(padding, ' ') + std::string(dw, '=') + std::string(padding, ' ') + ' ';
                }
                else
                {
                    uint8_t padding = (maxDiskWidth - 1) / 2;
                    std::cout << std::string(padding, ' ') + '|' + std::string(padding, ' ') + ' ';
                }
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

    void run()
    {
        render();
        moveTower(0, 2, disks);
        std::cout << "Algorithm used " << (int)steps << " steps" << std::endl;
    }

  private:
    uint8_t disks;
    uint8_t maxDiskWidth;
    std::vector<uint8_t> pins[3];
    uint8_t steps;

    uint8_t diskWidth(uint8_t disk) { return 1 + 2 * (disk - 1); }
    uint8_t findFreePin(uint8_t pin1, uint8_t pin2)
    {
        assert(0 <= pin1 && pin1 <= 2);
        assert(0 <= pin2 && pin2 <= 2);

        return std::abs(pin1 + pin2 - 3);
    }

    void moveDisk(uint8_t fromPin, uint8_t toPin)
    {
        assert(0 <= fromPin && fromPin <= 2);
        assert(0 <= toPin && toPin <= 2);

        uint8_t disk = pins[fromPin].back();
        pins[fromPin].pop_back();
        pins[toPin].push_back(disk);

        ++steps;

        render();
    }

    void moveTower(uint8_t fromPin, uint8_t toPin, uint8_t size)
    {
        assert(0 <= fromPin && fromPin <= 2);
        assert(0 <= toPin && toPin <= 2);
        assert(0 <= size && size <= disks);

        uint8_t freePin = findFreePin(fromPin, toPin);

        if (size == 0)
            return;
        else
            moveTower(fromPin, freePin, size - 1);

        moveDisk(fromPin, toPin);

        moveTower(freePin, toPin, size - 1);
    }
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " disk_number" << std::endl;
        return -1;
    }

    uint8_t disk_number = std::stoul(std::string(argv[1]));
    Hanoi hanoi(disk_number);
    hanoi.run();

    return 0;
}
