#include <vector>
#include <string>
#include <cstdint>
//=================================================================================================
// readMtVector() - Reads a CSV file full of integers and returns a vector containing them.
//                  Values in file can be in hex or decimal, and can be comma separated into lines
//                  of arbitrary length.   File can contain blank lines or comment lines beginning
//                  with either "#" or "//"
//
// Will throw std::runtime error if file doesn't exist
//=================================================================================================
extern std::vector<uint32_t> readMtVector(std::string filename);
