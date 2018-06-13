#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#define L 5

using namespace System;
using namespace System::IO::Ports;

bool open = true;

void MarshalString(String ^ s, std::string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

int serial(array<System::String ^> ^args)
{
	
	
	Console::WriteLine("Start");
	while (true)
	{
		SerialPort port("COM4", 9600);
		port.Open();
		while (open)
		{
			
			open = false;
		}
		std::string message2;
		String^ message = port.ReadLine();

		//Console::WriteLine(message);
		//int message = port.ReadChar();
		//Console::WriteLine(Convert::ToChar(message));

		MarshalString(message, message2);
		//std::cout << message2 << std::endl;

		std::stringstream  lineStream;
		lineStream << message2;
		float a, b, c, d, e ,f , g;
		lineStream >> a >> b >> c >> d >> e >> f >> g;
		std::cout << "yaw: " << std::setw(L) << a << "  pitch: " << std::setw(L) << b << "  row: " << std::setw(L) << c
		<< "  q0: " << std::setw(L) << d << "  q1: " << std::setw(L) << e << "  q2: " << std::setw(L) << f << "  q3: " << std::setw(L) << g << std::endl;
		port.Close();
	}
	return 0;
}

void main() 
{
	array<System::String ^> ^argss;
	serial(argss);
}