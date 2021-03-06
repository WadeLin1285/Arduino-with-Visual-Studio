#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#define L 5  // width of the number to print out (cout)

/* For port communication, we need these namespaces */
using namespace System;
using namespace System::IO::Ports;

/* Switch (.NET type) String^ type into (standard) string */
void MarshalString(String ^ s, std::string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

/* Define the serial communication function */
int serial(array<System::String ^> ^args)
{
	SerialPort port("COM4", 9600);             // define the serial port ref class
	port.Open();                               // open the USB port  [NOTE: Don't put these two operations into loop. They can only execute once.]
	
	Console::WriteLine("Start");

	while (true) // keep reading the serial buffer
	{
		std::string message2;
		String^ message = port.ReadLine();

		MarshalString(message, message2);      // translate String^ type into std::string

		std::stringstream  lineStream;         // convenient for translate string to integer 
		lineStream << message2;                // insert string message into stream
		float a, b, c;
		lineStream >> a >> b >> c;             // take out message as integers
		std::cout << "a: " << std::setw(L) << a << "  b: " << std::setw(L) << b << "  c: " << std::setw(L) << c << std::endl; // prints out in console
	}

	return 0;
}

void main() 
{
	array<System::String ^> ^argss;
	serial(argss);
}