#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <cassert>

class Hanoi
{
  public:
    Hanoi(int disks) : disks(disks), steps(0)
    {
        for (int disk = disks; disk > 0; --disk)
        {
            pins[0].push_back(disk);
        }
        maxDiskWidth = diskWidth(disks);
    }

    void render()
    {
        for (int height = disks; height > 0; --height)
        {
            for (int pin = 0; pin < 3; ++pin)
            {
                if (pins[pin].size() >= height)
                {
                    int dw      = diskWidth(pins[pin][height - 1]);
                    int padding = (maxDiskWidth - dw) / 2;
                    std::cout << std::string(padding, ' ') + std::string(dw, '=') + std::string(padding, ' ') + ' ';
                }
                else
                {
                    int padding = (maxDiskWidth - 1) / 2;
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
        std::cout << "Algorithm used " << steps << " steps" << std::endl;
    }

  private:
    int disks;
    int maxDiskWidth;
    std::vector<int> pins[3];
    int steps;

    int diskWidth(int disk) { return 1 + 2 * (disk - 1); }
    int findFreePin(int pin1, int pin2)
    {
        assert(0 <= pin1 && pin1 <= 2);
        assert(0 <= pin2 && pin2 <= 2);

        return std::abs(pin1 + pin2 - 3);
    }

    void moveDisk(int fromPin, int toPin)
    {
        assert(0 <= fromPin && fromPin <= 2);
        assert(0 <= toPin && toPin <= 2);

        int disk = pins[fromPin].back();
        pins[fromPin].pop_back();
        pins[toPin].push_back(disk);

        ++steps;

        render();
    }

    void moveTower(int fromPin, int toPin, int size)
    {
        assert(0 <= fromPin && fromPin <= 2);
        assert(0 <= toPin && toPin <= 2);
        assert(0 <= size && size <= disks);

        int freePin = findFreePin(fromPin, toPin);

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

    int disk_number = std::stoul(std::string(argv[1]));
    Hanoi hanoi(disk_number);
    hanoi.run();

    return 0;
}
