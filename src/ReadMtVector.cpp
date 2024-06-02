#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <stdexcept>
#include <vector>



//==========================================================================================================
// throwRuntime() - Throws a runtime exception
//==========================================================================================================
static void throwRuntime(const char* fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);

    throw std::runtime_error(buffer);
}
//=========================================================================================================


//=================================================================================================
// is_ws() - Returns true if the character pointed to is a space or tab
//=================================================================================================
static bool is_ws(const char* p) {return ((*p == 32) || (*p == 9));}
//=================================================================================================


//=================================================================================================
// is_eol() - Returns true if the character pointed to is an end-of-line character
//=================================================================================================
static bool is_eol(const char* p) {return ((*p == 10) || (*p == 13) || (*p == 0));}
//=================================================================================================


//=================================================================================================
// skip_comma() - On return, the return value points to either an end-of-line character, or to 
//                the character immediately after a comma
//=================================================================================================
static const char* skip_comma(const char* p)
{
    while (true)
    {
        if (*p == ',') return p+1;
        if (is_eol(p)) return p;
        ++p;
    }
}
//=================================================================================================



//=================================================================================================
// readMtVector() - Reads a CSV file full of integers and returns a vector containing them.
//                  Values in file can be in hex or decimal, and can be comma separated into lines
//                  of arbitrary length.   File can contain blank lines or comment lines beginning
//                  with either "#" or "//"
//
// Will throw std::runtime error if file doesn't exist
//=================================================================================================
std::vector<uint32_t> readMtVector(std::string filename)
{
    char buffer[0x10000];
    std::vector<uint32_t> result;

    // Try to open the input file
    FILE* ifile = fopen(filename.c_str(), "r");

    // Complain if we can't
    if (ifile == NULL) throwRuntime("can't read %s", filename.c_str());

    // Loop through each line of the file
    while (fgets(buffer, sizeof buffer, ifile))
    {
        // Point to the first byte of the buffer
        const char* p = buffer;

        // Skip over any leading whitespace
        while (is_ws(p)) ++p;

        // If the line is a "//" comment, skip it
        if (p[0] == '/' && p[1] == '/') continue;

        // If the line is a '#' comment, skip it
        if (*p == '#') continue;

        // This loop parses out comma-separated fields
        while (true)
        {
            // Skip over leading whitespace
            while (is_ws(p)) ++p;

            // If we've found the end of the line, we're done
            if (is_eol(p)) break;

            // Extract this value from the string
            uint32_t value = strtoul(p, nullptr, 0);

            // Append it to our result vector
            result.push_back(value);
        
            // Point to the next field
            p = skip_comma(p);
        }
    }

    // Close the input file, we're done reading it
    fclose(ifile);

    // Hand the resulting vector to the caller
    return result;
}
//=================================================================================================

