#include "RandomGenerator.h"
#include <iostream>
#include <algorithm>

vector<int> usedRandomNums;

int generateRandom(int start, int end)
{
    random_device rd;  // Seed for the random number engine
    mt19937 gen(rd()); // Mersenne Twister engine
    uniform_int_distribution<> distr(start, end);
    int randomNum = round(distr(gen));
    return randomNum;
}