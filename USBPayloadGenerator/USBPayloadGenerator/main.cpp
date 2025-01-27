//----------------------------------------------
// Project:	USBPayloadGenerator v1.2
// Author:	Nikola Vitanovic
// Email:	nikola@vitanovic.net
// Descr:	Generates Payload.h file needed 
//			for USBHIDTest project that is 
//			uploaded to the AT90USB162
//----------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
using namespace std;

int num = 0;
std::string int_to_hex(int i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(2)
		<< std::hex << i;
	return stream.str();
}
string KeyToDefine(char key)
{
	string x;
	for (int i = 'a'; i <= 'z'; i++)
	{
		if(i == key)
			return int_to_hex(i-97+4);
	}
	for (int i = '1'; i <= '9'; i++)
	{
		if (i == key)
			return int_to_hex(i - 0x31 + 0x1E);
	}
	if (key == ' ')
	{
		return "HID_KEYBOARD_SC_SPACE";
	}
	else if (key == '-')
	{
		return "HID_KEYBOARD_SC_KEYPAD_MINUS";
	}
	else if (key == '0')
	{
		return "0x27";
	}
	else if (key == '~')
	{
		return "0x00";
	}
	else if (key == '`')
	{
		return "HID_KEYBOARD_SC_LEFT_GUI";
	}
	return "0x00";
}
string FormPacket(char key)
{
	return "payload[" + std::to_string(num++) + "] = " + KeyToDefine(key) + ";";
}
string FormDelay(int len)
{
	string t;
	for (int i = 0; i < len; i++) //formiraj pauzu
		t += "payload[" + std::to_string(num++) + "] = 0x00;";
	return t;
}
string Parse(string line)
{
	//cout << line.find(" ");
	
	string total = "";
	string cmd = line.substr(0,line.find(" "));
	string args = line.substr(line.find(" ")+1);
	//cout << cmd.c_str() << endl << args.c_str() << endl;
	if		(cmd == "GUI")
	{
		total += "\tpayload[" + std::to_string(num++) + "] = HID_KEYBOARD_SC_LEFT_GUI;";
		//total += FormDelay(2) + "\n";
		if (args == "r")
			total += FormPacket(args[0]) + "\n";
		else
			total += "\n";
	}
	else if (cmd == "DELAY")
	{
		total += "\t" + FormDelay(atoi(args.c_str())) + "\n";
	}
	else if (cmd == "STRING")
	{
		for (int i = 0; i < args.length(); i++)
		{
			total += "\t" + FormPacket(args[i]) + "\n";
		}
	}
	else if (cmd == "ENTER")
	{
		total +=  "\tpayload[" + std::to_string(num++) + "] = HID_KEYBOARD_SC_ENTER;" + "\n";
	}
	return total;
}
int main(int argc, char ** argv)
{
	ifstream citac;
	ofstream pisac;
	string	 code;
	if (argc < 2)
		citac.open("skripta.txt");
	else
		citac.open(argv[1]);

	if (citac.is_open())
	{
		while (!citac.eof())
		{
			char * str = new char[2048];
			citac.getline(str, 2048);
			string s = str;
			string res = Parse(s);
			code += "\t// " + s + "\n";
			code += res;
		}
		string tmax = std::to_string(num++);
		code += "\tmax = " + tmax + ";" + "\n";
		code = "void InitPayload() {\n" + code;
		code += "}";
		code = "int max = 1;\nuint8_t payload[" + tmax + "];\n" + code;
		code = "//Payload.h\n//Generated by USBPayloadGenerator\n" + code;
		cout << endl;
		cout << endl;
		cout << code;
		pisac.open("Payload.h");
		if (pisac.is_open())
			pisac << code;
		pisac.close();
		cout << endl << "Generated file is in current directory named \"Payload.h\"" << endl << endl;
	}
	else
	{
		cout << endl << endl << "USBPayloadGenerator <DUCKY_SCRIPT_FILENAME>" << endl << "Generated file is in current directory named \"Payload.h\"" << endl;
	}
	citac.close();
	return 0;
}