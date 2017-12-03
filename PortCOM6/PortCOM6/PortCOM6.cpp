

#include <iostream>
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::Threading;

typedef struct Ramka {

	std::string
		AdresUrzadzenia,
		Nr_komendy,
		Info_Ilosc_danych,
		Wartosc_Ustaona,
		Suma_kontrolna;
	std::string Dane[9];
} ramka;

ramka * pierwszy;


public ref class PortChat
{
private:
	static bool _continue;
	static SerialPort^ serialPort;

public:

	static void PortCom(Ramka ramka);
	static String^ SetPortName(String^ defaultPortName);
	static void Read();
};

String^daneRamka(Ramka ramka) {
	std::string daneWE;
	String^daneWY;

	ramka.AdresUrzadzenia = "0xff";
	ramka.Nr_komendy = "0xC172";
	ramka.Info_Ilosc_danych = "0x0000";
	ramka.Wartosc_Ustaona = "0x47C1";
	ramka.Suma_kontrolna = "0xffff";

	//kompletowanie danych 
	daneWE.append(ramka.AdresUrzadzenia);
	daneWE.append(ramka.Nr_komendy);
	daneWE.append(ramka.Info_Ilosc_danych);
	daneWE.append(ramka.Wartosc_Ustaona);
	daneWE.append(ramka.Suma_kontrolna);
	//konwrtowanie stringa z std::string na String^
	daneWY = gcnew String(daneWE.c_str());

	return daneWY;
}


void PortChat::PortCom(Ramka ramka)
{




	String^ message;
	StringComparer^ stringComparer = StringComparer::OrdinalIgnoreCase;
	Thread^ readThread = gcnew Thread(gcnew ThreadStart(PortChat::Read));
	//ustawienia portu szeregowego
	serialPort = gcnew SerialPort();
	serialPort->PortName = SetPortName(serialPort->PortName);
	serialPort->ReadTimeout = 500;
	serialPort->WriteTimeout = 500;
	serialPort->Open();
	_continue = true;
	readThread->Start();

	Console::WriteLine("Type QUIT to exit");

	while (_continue)
	{
		message = Console::ReadLine();

		if (stringComparer->Equals("quit", message))
		{
			_continue = false;
		}
		else
		{



			serialPort->WriteLine(daneRamka(ramka));

		}
	}

	readThread->Join();
	serialPort->Close();
}

String ^ PortChat::SetPortName(String ^ defaultPortName)
{

	String^ portName;

	Console::WriteLine("Available Ports:");
	for each (String^ s in SerialPort::GetPortNames())
	{
		Console::WriteLine("   {0}", s);
	}

	Console::Write("Enter COM port value (Default: {0}): ", defaultPortName);
	portName = Console::ReadLine();

	if (portName == "")
	{
		portName = defaultPortName;
	}
	return portName;
}

void PortChat::Read()
{

	while (_continue)
	{
		try
		{
			String^ message = serialPort->ReadLine();
			Console::WriteLine(message);
		}
		catch (TimeoutException ^) {}
	}
}



int main()
{
	Ramka ramka;
	PortChat::PortCom(ramka);
}



