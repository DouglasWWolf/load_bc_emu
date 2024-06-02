#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include "ReadMtVector.h"
#include "PciDevice.h"

using namespace std;

PciDevice        PCI;
string           filename;
int              which_fifo;
string           device = "10EE:903F";

vector<uint32_t> readDataFile(string filename);
void             storeDataInFifo(int fifo, vector<uint32_t>& data);
void             parseCommandLine(int argc, const char** argv);
void             execute();

//=================================================================================================
// main() - Program execution begins here
//=================================================================================================
int main(int argc, const char** argv)
{
    // Parse the command line
    parseCommandLine(argc, argv);

    // Execute the main body of the program
    try
    {
        execute();
    }

    // If anything throws a runtime error, display it and exit
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    // If we get here, all is well    
    return 0;
}
//=================================================================================================



//=================================================================================================
// parseCommandLine() - Parse the command line
//=================================================================================================
void parseCommandLine(int argc, const char** argv)
{
    // If we have the wrong number of command line arguments, show usage
    if (argc != 3)
    {
        printf("usage: load_bc_emu <1|2> <filename>\n");
        exit(1);
    }

    // Find out which FIFO we're going to load
    which_fifo = atoi(argv[1]);

    // Store the name of the file we're going to read
    filename = argv[2];

    // Validate the fifo number to ensure it's valid
    if (which_fifo != 1 && which_fifo != 2)
    {
        fprintf(stderr, "fifo number must be 1 or 2\n");
        exit(1);
    }

}
//=================================================================================================


//=================================================================================================
// execute() - This is the main body of the program
//=================================================================================================
void execute()
{
    // Read the vector of integers from the file
    vector<uint32_t> data = readMtVector(filename);

    // Store those integers into the appropriate FIFO on the FPGA
    storeDataInFifo(which_fifo, data);
}
//=================================================================================================


//=================================================================================================
// storeDataInFifo() - Stores our vector of data into the specified FIFO on the FPGA
//=================================================================================================
void storeDataInFifo(int fifo_number, vector<uint32_t>& data)
{
    // The registers we want to program are in resource region #0
    int pciRegion = 0;

    // The addreses of the two FIFOs
    const uint32_t BASE_ADDR   = 0x01000;
    const uint32_t REG_LOAD_F0 = BASE_ADDR + 0x08;
    const uint32_t REG_LOAD_F1 = BASE_ADDR + 0x0C;

    // Determine which AXI register we want to store data in
    uint32_t axiAddr = (fifo_number == 1) ? REG_LOAD_F0 : REG_LOAD_F1;

    // Map the PCI memory-mapped resource regions into user-space
    PCI.open(device);

    // Fetch the list of memory mapped resource regions
    auto resource = PCI.resourceList();

    // Get a user-space reference to the AXI register that feeds our FIFO
    uint32_t& FIFO = *(uint32_t*)(resource[pciRegion].baseAddr + axiAddr);

    // Load the data into the FIFO
    for (auto v : data)
    {
        FIFO = v;
        usleep(100);
    }
}
//=================================================================================================
