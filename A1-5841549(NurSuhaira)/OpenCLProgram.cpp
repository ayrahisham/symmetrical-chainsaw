// Name: Nur Suhaira Bte Badrul Hisham
// Student ID: 5841549
// Assignment 1

// Without this, the compiler will use the default OpenCL 2.x, 
// where some OpenCL 1.2 functions are deprecated.
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

// By default, OpenCL disables exceptions.This line enables it.
#define __CL_ENABLE_EXCEPTIONS

// This defines the minimum number of devices you want to create a context for. 
// Note that can create as many contexts as you want, 
// all devices in a context must be from the same platform.
#define NUMBER_OF_DEVICES 1	// minimum number of devices required

// The headers from the C++ standard library and STL that are used
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>

// Including the OpenCL header. 
// Depending on which Operating System (OS) 
// we are using, this checks whether we are running the program on Mac OS 
// or other OS :
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// Include a user defined header file :
#include "error.h"

// Having to type std:: and cl:: prefixes in front of the C++
// standard library or OpenCL functions
// using namespace std;
// using namespace cl;

cl_device_type getDeviceType (std::string);
void getNoAvailDevices (std::vector <cl::Platform>, std::vector <cl::Device> devices, 
	std::string, cl_device_type, cl_device_type);
void printDevice (cl_device_type);
void printLocalMemSize (long);
int getUserDevice (std::vector <int>);
bool is_number(const std::string&);
void printFileContents (std::string);
void displayBuildLog (cl::Program, std::vector <cl::Device>);
void displayKernels (std::vector <cl::Kernel>);

int main ()
{
	// Declaring a number of data structures and variables, 
	// the most important are :
	// used to store the platforms and devices available to the system.
	std::vector <cl::Platform> platforms;
	std::vector <cl::Device> devices;
	std::vector <int> platformId;
	std::vector <int> deviceId;
	std::vector <cl::Device> contextDevices;
	std::vector <cl::Kernel> allKernels; // all kernels
	std::string inputString; // user input
	std::string outputString; // string for output
	std::string filename = "source.cl";
	int outputInt; // integer for output
	int workItemSize[3]; // work item size for output
	long outputLong; // long for output
	unsigned int i, j, k; // counters
	unsigned int chosenDevice = 0; // user's selection of 1 device
	cl::Context context; // context for user's chosen device
	cl::CommandQueue queue;	// commandqueue for a context and device

	try
	{
		std::cout << "\n=======================TASK 1==========================" << std::endl;
		std::cout << "OpenCL program launching..." << std::endl;
		// A cl::Platform object represents an installed platform. 
		// The static function cl::Platform::get() function will return 
		// a vector containing a cl::Platform object for each platform
		//obtains the OpenCL platforms that are available on the system
		cl::Platform::get (&platforms);
		//displays the number of available platforms
		std::cout << "\t" << platforms.size() << " OpenCL Platforms found " << std::endl; 
		
		std::cout << "\nEnter the type of device (CPU/GPU): ";
		getline (std::cin, inputString);

		// Format user input string
		transform(inputString.begin(), inputString.end(), inputString.begin(), toupper);

		// Compare user input string and set type based on user's selection
		cl_device_type type = NULL;
		cl_device_type usertype = getDeviceType(inputString);

		getNoAvailDevices (platforms, devices, inputString, type, usertype);

		std::cout << "\n=======================TASK 2==========================" << std::endl;
		// Once we obtain the platform objects, 
		// we can query the platform information for each platform. 
		// Contains a loop which goes through the platforms and 
		// displays information about the platform :
		for (i = 0; i < platforms.size(); i++)
		{
			platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);
			int usersize = 0;

			// For each device
			for (j = 0; j < devices.size(); j++)
			{
				// Based on the user’s selection, 
				// search the system for all CPU or GPU devices
				devices[j].getInfo(CL_DEVICE_TYPE, &type);

				if (type == usertype)
				{
					++chosenDevice;
					
					// Output platform index
					std::cout << "  PLATFORM " << i + 1 << ":" << std::endl;

					// The getInfo() member function allows us to obtain platform information
					// Get and output platform name
					platforms[i].getInfo(CL_PLATFORM_NAME, &outputString);
					std::cout << "\tCL_PLATFORM_NAME: " << outputString << std::endl;

					// Get and output platform vendor name
					outputString = platforms[i].getInfo<CL_PLATFORM_VENDOR>();
					std::cout << "\tCL_PLATFORM_VENDOR: " << outputString << std::endl;

					// get and output OpenCL version supported by the platform
					outputString = platforms[i].getInfo<CL_PLATFORM_VERSION>();
					std::cout << "\tCL_PLATFORM_VERSION: " << outputString << std::endl;

					// Output device index
					std::cout << std::endl << chosenDevice << ") -> Platform: "
						<< platforms[i].getInfo<CL_PLATFORM_NAME>()
						<< " / Device: "
						<< devices[j].getInfo<CL_DEVICE_NAME>()
						<< std::endl;

					// Store platform and device info based on user selection
					platformId.push_back(i);
					deviceId.push_back(j);

					// If only want specific device types, 
					// use for example: CL_DEVICE_TYPE_CPU or CL_DEVICE_TYPE_GPU
					// Get and output device type
					printDevice (usertype);

					// Get and output device name
					outputString = devices[j].getInfo<CL_DEVICE_NAME>();
					std::cout << "\tCL_DEVICE_NAME: " << outputString << std::endl;

					// Get and output number of compute units (int)
					outputInt = devices[j].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
					std::cout << "\tNumber of compute units: " << outputInt << std::endl;

					// Get and output number of maximum work group size
					outputInt = devices[j].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
					std::cout << "\tNumber of maximum work group size: " << outputInt << std::endl;

					// Get and output number of maximum work item sizes
					devices[j].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &workItemSize);
					std::cout << "\tNumber of maximum work item sizes: "
						<< workItemSize[0] << " x "
						<< workItemSize[1] << " x "
						<< workItemSize[2] << std::endl;

					// Get and output of local memory size (long)
					outputLong = devices[j].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
					printLocalMemSize (outputLong);

					// Display divider in between platforms available
					if (i < platforms.size() - 1)
					{
						std::cout << "\n-------------------------------------------------------" << std::endl;
					}
				}
			}	
		}

		// if there exists a device type selected by user
		if (deviceId.size() > 0)
		{
			int choice = getUserDevice(deviceId); // user device choice from display info

			// Checks whether the number of available devices for that platform 
			// meets the minimum number that is requested :
			if (deviceId.size() >= NUMBER_OF_DEVICES)
			{
				system ("cls");
				std::cout << "\n=======================TASK 3==========================" << std::endl;
				std::cout << "Creating a context for device: " 
					      << platforms[platformId[choice - 1]].getInfo<CL_PLATFORM_NAME>() 
					      << std::endl;

				// Create a context for chosen device on that platform
				platforms[platformId[choice - 1]].getDevices (CL_DEVICE_TYPE_ALL, &devices);
				context = cl::Context (devices[deviceId[choice - 1]]);

				std::cout << "Creating a queue for device: " 
	    				  << platforms[platformId[choice - 1]].getInfo<CL_PLATFORM_NAME>()
						  << std::endl;
				// Create command queue
				queue = cl::CommandQueue(context, devices[deviceId[choice - 1]]);

				// Check devices in the context
				std::cout << "\nDevice in the context:" << std::endl;

				// Get device in the context
				contextDevices = context.getInfo<CL_CONTEXT_DEVICES>();

				// output names of device in the context
				if (contextDevices.size() == 1)
				{
					outputString = contextDevices[0].getInfo<CL_DEVICE_NAME>();
					std::cout << "\tChosen CL_DEVICE_NAME: " << outputString << std::endl;
				}
			}

			std::cout << "\n=======================TASK 4==========================" << std::endl;
			std::cout << "Opening file: " << filename << std::endl;

			// Open input file stream to source.cl file
			std::ifstream programFile (filename);

			// Check whether file was opened
			if (!programFile.is_open())
			{
				std::cout << "\nFile not found." << std::endl;
			}
			else
			{
				std::cout << "\nCurrently reading: " << filename << std::endl;
				// Create program string and load contents from the file
				std::string programString(std::istreambuf_iterator<char>(programFile), 
					(std::istreambuf_iterator<char>()));

				printFileContents (programString);

				// Create program source from one input string
				cl::Program::Sources source (1, std::make_pair (programString.c_str(), 
					programString.length() + 1));
				// Create program from source
				cl::Program program (context, source);

				// Try to build program
				try
				{
					// Build the program for the devices in the context
					// An OpenCL framework must provide a runtime compiler since it
					// builds device specific executables.
					// As such, the build function accepts a vector containing the
					// device objects to be targeted by the compiler :
					program.build (contextDevices);

					std::cout << "Creating a program to encapsulate context and kernels" 
						      << std::endl;
					std::cout << "\nProgram build : Successful" << std::endl;
					if (contextDevices.size() == 1)
					{
						displayBuildLog(program, contextDevices);

						std::cout << "\n=======================TASK 5==========================" << std::endl;
						std::cout << "Creating kernels from the program\n" << std::endl;

						// create all kernels in the program
						program.createKernels (&allKernels);
						std::cout << "Number of kernels in the program: " << allKernels.size() << std::endl;

						// display all kernel function names
						displayKernels (allKernels);
						
					}
				}
				catch (cl::Error e)
				{
					// If failed to build program
					if (e.err() == CL_BUILD_PROGRAM_FAILURE)
					{
						// Output program build log
						std::cout << e.what() << ": Failed to build program." << std::endl;

						// Output build log for all devices in context
						if (contextDevices.size() == 1)
						{
							displayBuildLog(program, contextDevices);
						}
					}
					else
					{
						throw e;
					}
				}
			}
		}
		else
		{
			// if no platform has no selected type of device
			std::cout << "Unable to find any device in any platform with CL_DEVICE_TYPE_" 
				 << inputString << std::endl;
		}
	}
	// Catches any OpenCL exceptions and calls the function to display the error information.
	catch (cl::Error e)
	{
		handle_error (e);
	}

	system ("pause");
	return 0;
}

cl_device_type getDeviceType (std::string inputString)
{
	cl_device_type usertype = NULL;

	// Format user input string
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), toupper);

	// Compare user input string and set type based on user's selection
	if (inputString.compare("CPU") == 0)
	{
		usertype = CL_DEVICE_TYPE_CPU;
	}
	else if (inputString.compare("GPU") == 0)
	{
		usertype = CL_DEVICE_TYPE_GPU;
	}

	return usertype;
}

void getNoAvailDevices (std::vector <cl::Platform> platforms, std::vector <cl::Device> devices, 
	std::string inputString, cl_device_type type, cl_device_type usertype)
{
	int usersize;

	for (int i = 0; i < platforms.size(); i++)
	{
		usersize = 0;
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

		for (int j = 0; j < devices.size(); j++)
		{
			devices[j].getInfo(CL_DEVICE_TYPE, &type);

			if (type == usertype)
			{
				++usersize; // increment of user selection size
			}
		}

		std::cout << "\t\t" << usersize << " " << inputString
			      << " device(s) found supporting OpenCL on Platform " 
			      << i+1 << ": " 
			      << platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
	}

}

void printDevice (cl_device_type usertype)
{
	std::cout << "\tCL_DEVICE_TYPE: ";
	if (usertype == CL_DEVICE_TYPE_CPU)
	{
		std::cout << "CPU" << std::endl;
	}
	else if (usertype == CL_DEVICE_TYPE_GPU)
	{
		std::cout << "GPU" << std::endl;
	}
	else
	{
		std::cout << "Other" << std::endl;
	}
}

void printLocalMemSize (long outputLong)
{
	std::cout << "\tLocal memory size: " << outputLong;

	if (outputLong >= 32000 && outputLong < 64000)
	{
		std::cout << " (32KB)" << std::endl;
	}
	else if (outputLong >= 64000 && outputLong < 128000)
	{
		std::cout << " (64KB)" << std::endl;
	}
}

int getUserDevice(std::vector <int> deviceId)
{
	int choice;
	std::string input;

	do
	{
		// Based on the devices available, allow the user to select one device
		std::cout << "\nEnter one device id [1-"
				  << deviceId.size()
				  << "] to create context: ";
		std::getline (std::cin, input);
		if (is_number(input)) // check it is character
		{
			choice = std::atoi(input.c_str());
		}
		else
		{
			choice = std::atoi(input.c_str());
		}
	} while (choice <= 0 || choice > deviceId.size()); 
	// make sure user choose device within availability
	
	return choice;
}

// source: https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool is_number (const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

void printFileContents (std::string programString)
{
	// Output contents of the file
	// The reason for this is because to build the program, 
	// before can build the program, 
	// the source code must be stored in a buffer.
	// While it is possible to define the source code as a collection
	// of strings in the host application code itself, 
	// most proper programs will read in the source code from
	// a separate file.This keeps the host and device code separate.
	// Thus, it is a good idea to have a
	// function to read the contents of a file
	std::cout << "\nContents of program string" << std::endl;
	std::cout << "--------------------------\n" << std::endl;
	std::cout << programString << std::endl;
}

void displayBuildLog (cl::Program program, std::vector <cl::Device> contextDevices)
{
	// If there were build errors, an exception will be thrown. 
	// We can check the build log to see what caused the error.
	// The following code checks whether the exception was caused by a failure 
	// to build the program, the displays the device name and build log, 
	// using the getBuildInfo() function, for all devices in the context :
	// Display build_log
	std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevices[0]);

	// Get device name
	std::cout << "Device: " << contextDevices[0].getInfo<CL_DEVICE_NAME>() << std::endl;
	std::cout << "\nBuild Log: " << std::endl;
	std::cout << build_log << std::endl;

}

void displayKernels (std::vector <cl::Kernel> allKernels)
{
	std::cout << "\nDisplay Kernel Function Names" << std::endl;
	std::cout << "-----------------------------" << std::endl;

	// output kernel name for each index
	for (int i = 0; i < allKernels.size(); i++)
	{
		std::cout << "Kernel " << i << ": " 
			      << allKernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>() << std::endl;
	}

	std::cout << std::endl;
}