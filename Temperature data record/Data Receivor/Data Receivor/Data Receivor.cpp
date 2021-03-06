#include "stdafx.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <time.h>
#include <fstream>

#define L 6  // width of the number to print out (cout)

/* For port communication, we need these namespaces */
using namespace System;
using namespace System::IO::Ports;

bool open = true;               // Define the usb serial open state
float temp;                     // Define the temperature value
char timebuf[26];
long long int millisecond = 0, duration = 0;
time_t ltime;
errno_t err;

/* Create a global structure with ref class SerialPort.
Since SerialPort is ref class, it can't define globally. */
ref struct Port {

	static SerialPort^ port = gcnew SerialPort("COM5", 9600);

};

// Open file
std::ofstream fout;
bool file_open = true;
std::string filename;

/* Define the functions*/
void InitTime();
void TimeRecord();
void MarshalString(String ^ s, std::string& os);
void Serial();
void FileCheck();

void main()
{
	// Enter the file name
	printf("Enter the file name:\n");
	std::cin >> filename; // read the keys entered

	// Check the file
	fout.open(filename);
	FileCheck();

	// Get UNIX-style time and display as number and string.   
	InitTime();

	while (1) {
		TimeRecord();
		Serial();
	}
	
	return;
}

/* Initialoze the time */
void InitTime() {
	time(&ltime);
	err = ctime_s(timebuf, 26, &ltime);
	if (err)
	{
		printf("ctime_s failed due to an invalid argument.");
		exit(1);
	}
	std::cout << "UNIX time and date:\t" << timebuf << std::endl; // prints out in console
	if (file_open) fout << "UNIX time and date:\t" << timebuf << std::endl; // write to file
}

/* Record the current time and duration */
void TimeRecord() {
	SYSTEMTIME st;
	GetLocalTime(&st);
	if (millisecond == 0) {
		millisecond = st.wHour * 60 * 60 * 1000 + st.wMinute * 60 * 1000 + st.wSecond * 1000 + st.wMilliseconds;
	}
	else {
		static long long int old_millisecond = millisecond;
		millisecond = st.wHour * 60 * 60 * 1000 + st.wMinute * 60 * 1000 + st.wSecond * 1000 + st.wMilliseconds;
		duration = millisecond - old_millisecond;
	}

	std::cout << "Time(h/m/s/ms): " << std::setw(2) << st.wHour << ':' << std::setw(2) << st.wMinute 
			  << ':' << std::setw(2) << st.wSecond << '.' << std::setw(3) << st.wMilliseconds 
			  << "  Duration(ms): " << std::setw(10) << duration << "   " ; // prints out in console
	if (file_open) {
		fout << "Time(h/m/s/ms): " << std::setw(2) << st.wHour << ':' << std::setw(2) << st.wMinute
		  	 << ':' << std::setw(2) << st.wSecond << '.' << std::setw(3) << st.wMilliseconds
			 << "  Duration(ms): " << std::setw(10) << duration << "   "; // write to file
	}
}

/* Switch (.NET type) String^ type into (standard) string */
void MarshalString(String ^ s, std::string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}


/* Read the Serial Port */
void Serial()
{
	while (open) // open serial port only once
	{
		Port::port->Open();
		open = false;
	}

	std::string message2;
	String^ message = Port::port->ReadLine();

	MarshalString(message, message2);      // translate String^ type into std::string

	std::stringstream  lineStream;         // convenient for translate string to integer 
	lineStream << message2;                // insert string message into stream
	lineStream >> temp;             // take out message as integers

	std::cout << "Temp(C): " << std::setw(L) << temp << std::endl; // prints out in console
	if (file_open) fout << "Temp(C): " << std::setw(L) << temp << std::endl; // write to file
}

void FileCheck() {
	if (fout.is_open()) {
		printf("File opened.\n");
	}
	else {
		printf("Fail to open file...\n");
		return;
	}
}