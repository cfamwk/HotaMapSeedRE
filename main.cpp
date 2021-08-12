#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <time.h>
#include <iostream>
#include <map>
#include <vector>

#pragma comment(lib, "winmm.lib")

static constexpr auto SECONDS_PER_YEAR      = 31556926;
static constexpr auto SECONDS_PER_DAY       = 86400;
static constexpr auto SECONDS_PER_HOUR      = 3600;
static constexpr auto SECONDS_PER_MINUTE    = 60;
static constexpr auto MINUTES_PER_HOUR      = 60;
static constexpr auto MINUTES_PER_DAY       = 1440;

/// <summary>
/// Rebuilt from H3 disassembly. Returns a random number within a range.
/// </summary>
int h3_get_random_number_in_rage(int min, int max)
{
    if (max == min)
    {
        return max;
    }
    if (max < min)
    {
        return min;
    }
    return min + rand() % (max - min + 1);
}

/// <summary>
/// Rebuilt from H3 disassembly. The game uses this function to generate a map seed.
/// </summary>
uint32_t h3_generate_rmg_seed()
{
    uint32_t time;

    tm * gmtime; 
    
    __time64_t time64; 
    
    uint32_t seed;

    time = timeGetTime();

    srand(time);

    _time64(&time64);

    gmtime = _gmtime64(&time64);

    seed = ((((h3_get_random_number_in_rage(1, 59) ^ gmtime->tm_sec) + 60 * (gmtime->tm_min + 60 * (gmtime->tm_hour + 24 * gmtime->tm_yday))) & 0x1FFFFFF) << 6) | seed & 0x8000003F;

    seed = 16 * (h3_get_random_number_in_rage(0, 3) & 3) | seed & 0xFFFFFFCF;

    seed = (gmtime->tm_year - 117) % 16 & 0xF | seed & 0xFFFFFFF0;

    seed &= 0x7FFFFFFFu;

    return seed;
}

/// <summary>
/// Utility function. Returns a vector of all numbers within a range
/// </summary>
std::vector<int32_t> get_numbers_in_range(int32_t min, int32_t max)
{
    std::vector<int32_t> numbers;

    for (auto i = min; i <= max; i++)
    {
        numbers.push_back(i);
    }

    return std::move(numbers);
}

/// <summary>
/// Bruteforces the generation timestamp given a seed. If a match can be found, the timestamp will be printed to the console window
/// </summary>
uint32_t print_seed_generation_time(uint32_t seed, uint32_t seconds_range)
{
    auto get_timestamp = [] ()
    {
        __time64_t time64;

        // Returns the time as seconds elapsed since midnight, January 1, 1970, or -1 in the case of an error.
        _time64(&time64);

        return *_gmtime64(&time64);
    };

    std::cout << "Testing seed, please wait, this may take awhile... " << std::endl;

    auto current_time = timeGetTime();

    auto current_timestamp = get_timestamp();

    uint32_t min_time = current_time - (seconds_range / 2);

    uint32_t max_time = current_time + (seconds_range / 2);

    auto v_1_to_59 = get_numbers_in_range(1, 59);

    auto v_0_to_3 = get_numbers_in_range(0, 3);

    uint32_t seconds_elapsed = 0;

    for (auto current_seed = min_time; current_seed <= max_time; current_seed++, seconds_elapsed++)
    {
        srand(current_seed);

        tm modified_timestamp = current_timestamp;

        modified_timestamp.tm_sec += (seconds_elapsed - (seconds_range / 2));
        
        mktime(&modified_timestamp);

        for (auto & n1 : v_0_to_3)
        {
            for (const auto & n2 : v_1_to_59)
            {
                uint32_t test_seed = ((((n2 ^ modified_timestamp.tm_sec) +
                    60 * (modified_timestamp.tm_min + 60 * (modified_timestamp.tm_hour + 24 * modified_timestamp.tm_yday))) & 0x1FFFFFF) << 6) | test_seed & 0x8000003F;
                
                test_seed = 16 * (n1 & 3) | test_seed & 0xFFFFFFCF;
                
                test_seed = (modified_timestamp.tm_year - 117) % 16 & 0xF | test_seed & 0xFFFFFFF0;
                
                test_seed &= 0x7FFFFFFFu;
        
                if (test_seed == seed)
                {
                    std::cout << "Match: Year: " << modified_timestamp.tm_year << " Day: " << modified_timestamp.tm_yday << " Hour: " <<
                        modified_timestamp.tm_hour << " Minutes: " << modified_timestamp.tm_min << " Seconds: " << abs(modified_timestamp.tm_sec) << std::endl;
                }
            }
        }
    }

    return 0;
}

int main()
{
    std::string str_seed;

    uint32_t int_seed;

    std::cout << "Enter seed" << std::endl;

    std::cin >> str_seed;

    int_seed = std::atoi(str_seed.c_str());

    print_seed_generation_time(int_seed, SECONDS_PER_DAY * 60);

    std::cout << "Done." << std::endl;
}
