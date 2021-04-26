#include <chrono>
#include <fstream>

using namespace std;

/**
 * Function for getting timestamp
 * @return timestamp in milliseconds
 */
uint64_t getTimestamp()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

/**
 * Function to output array
 * @param arr the array to output
 * @param arrayLength the length of the array
 * @param os the output stream
 */
void printArray(float arr[], int arrayLength, fstream &os)
{
    bool firstLine = true;
    for (int i = 0; i < arrayLength; i++)
    {
        if (!firstLine)
            os << ", ";
        os << arr[i];
        firstLine = false;
    }
    os << endl;
}