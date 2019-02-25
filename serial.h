#ifndef _SERIAL_H
#define _SERIAL_H
#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>  
using namespace std;

HANDLE initialSerialPort(char *szStr);
void serialSend(HANDLE com, string data);
float readSerial(HANDLE com);



#endif // !__SERIAL_H

