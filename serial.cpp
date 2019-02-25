#include "serial.h"


HANDLE initialSerialPort(char *szStr) {
	//const char temp[5]="COM4";

	WCHAR wszClassName[5];
	memset(wszClassName, 0, sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName,
		sizeof(wszClassName) / sizeof(wszClassName[0]));
	HANDLE hCom1 = CreateFile(wszClassName,//COM1口
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);

	//if (hCom1 == (HANDLE)-1)
	if (hCom1 == INVALID_HANDLE_VALUE)
	{
		printf("打开COM失败!\n");
		//return FALSE;
	}
	else
	{
		printf("COM打开成功！\n");
	}

	SetupComm(hCom1, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 5000;
	TimeOuts.ReadTotalTimeoutConstant = 5000;
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom1, &TimeOuts); //设置超时
	DCB dcb;
	GetCommState(hCom1, &dcb);
	dcb.BaudRate = 9600; //波特率为9600
	dcb.ByteSize = 8; //每个字节有8位
	dcb.Parity = NOPARITY; //无奇偶校验位
	dcb.StopBits = ONESTOPBIT; //1个停止位
	SetCommState(hCom1, &dcb);
	return hCom1;
}


void serialSend(HANDLE com, string data)
{
	WriteFile(com, data.data(), data.size(), NULL, NULL);
}

float readSerial(HANDLE com)
{
	DWORD readsize;
	char cache[100] = { '\0' };

	ReadFile(com, cache, 13, &readsize, NULL);
	float data_float = atof(cache);
	cout << "Recive: " << data_float << endl;
	return data_float;
}


