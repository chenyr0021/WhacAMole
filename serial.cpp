#include "serial.h"


HANDLE initialSerialPort(char *szStr) {
	//const char temp[5]="COM4";

	WCHAR wszClassName[5];
	memset(wszClassName, 0, sizeof(wszClassName));
	MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName,
		sizeof(wszClassName) / sizeof(wszClassName[0]));
	HANDLE hCom1 = CreateFile(wszClassName,//COM1��
		GENERIC_READ | GENERIC_WRITE, //�������д
		0, //��ռ��ʽ
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���
		0, //ͬ����ʽ
		NULL);

	//if (hCom1 == (HANDLE)-1)
	if (hCom1 == INVALID_HANDLE_VALUE)
	{
		printf("��COMʧ��!\n");
		//return FALSE;
	}
	else
	{
		printf("COM�򿪳ɹ���\n");
	}

	SetupComm(hCom1, 1024, 1024); //���뻺����������������Ĵ�С����1024
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 5000;
	TimeOuts.ReadTotalTimeoutConstant = 5000;
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 500;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(hCom1, &TimeOuts); //���ó�ʱ
	DCB dcb;
	GetCommState(hCom1, &dcb);
	dcb.BaudRate = 9600; //������Ϊ9600
	dcb.ByteSize = 8; //ÿ���ֽ���8λ
	dcb.Parity = NOPARITY; //����żУ��λ
	dcb.StopBits = ONESTOPBIT; //1��ֹͣλ
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


