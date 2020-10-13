#include "SttSerialPort.h"
#include <Windows.h>
#include <qdebug.h>

SttSerialPort::SttSerialPort()
{

}
SttSerialPort::~SttSerialPort()
{

}

//�򿪴���
bool SttSerialPort::OpenSerialPort(const char* portname, int baudrate, char parity, char databit, char stopbit)
{
	m_Hcom = CreateFileA(portname, //������
		GENERIC_READ | GENERIC_WRITE, //֧�ֶ�д
		0, //��ռ��ʽ�����ڲ�֧�ֹ���
		NULL,//��ȫ����ָ�룬Ĭ��ֵΪNULL
		OPEN_EXISTING, //�����еĴ����ļ�
		0, //0��ͬ����ʽ��FILE_FLAG_OVERLAPPED���첽��ʽ
		NULL);//���ڸ����ļ������Ĭ��ֵΪNULL���Դ��ڶ��Ըò���������ΪNULL

	if (m_Hcom == (HANDLE)-1)
	{
		return false;
	}

	//���û�������С 
	if (!SetupComm(m_Hcom, 1024, 1024))
	{
		return false;
	}

	// ���ò��� 
	DCB p;
	memset(&p, 0, sizeof(p));
	p.DCBlength = sizeof(p);
	p.BaudRate = baudrate; // ������
	p.ByteSize = databit; // ����λ

	switch (parity) //У��λ
	{
	case 0:
		p.Parity = NOPARITY; //��У��
		break;
	case 1:
		p.Parity = ODDPARITY; //��У��
		break;
	case 2:
		p.Parity = EVENPARITY; //żУ��
		break;
	}

	switch (stopbit) //ֹͣλ
	{
	case 1:
		p.StopBits = ONESTOPBIT; //1λֹͣλ
		break;
	case 2:
		p.StopBits = TWOSTOPBITS; //2λֹͣλ
		break;
	case 3:
		p.StopBits = ONE5STOPBITS; //1.5λֹͣλ
		break;
	}

	if (!SetCommState(m_Hcom, &p))
	{
		// ���ò���ʧ��
		return false;
	}

	//��ʱ����,��λ������
	//�ܳ�ʱ��ʱ��ϵ��������д���ַ�����ʱ�䳣��
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout = 1000; //�������ʱ
	TimeOuts.ReadTotalTimeoutMultiplier = 500; //��ʱ��ϵ��
	TimeOuts.ReadTotalTimeoutConstant = 5000; //��ʱ�䳣��
	TimeOuts.WriteTotalTimeoutMultiplier = 500; // дʱ��ϵ��
	TimeOuts.WriteTotalTimeoutConstant = 2000; //дʱ�䳣��
	SetCommTimeouts(m_Hcom, &TimeOuts);

	PurgeComm(m_Hcom, PURGE_TXCLEAR | PURGE_RXCLEAR);//��մ��ڻ�����

	memcpy(m_pHandle, &m_Hcom, sizeof(m_Hcom));// ������
	return true;
}
//�رմ���
void SttSerialPort::CloseSerialPort()
{
	CloseHandle(m_Hcom);
}

int SttSerialPort::Send(const void *buf, int len)
{
	DWORD dwBytesWrite = len; //�ɹ�д��������ֽ���
	BOOL bWriteStat = WriteFile(m_Hcom, //���ھ��
		buf, //�����׵�ַ
		dwBytesWrite, //Ҫ���͵������ֽ���
		&dwBytesWrite, //DWORD*���������շ��سɹ����͵������ֽ���
		NULL); //NULLΪͬ�����ͣ�OVERLAPPED*Ϊ�첽����
	if (!bWriteStat)
	{
		return 0;
	}
	return dwBytesWrite;
}

int SttSerialPort::Receive(void *buf, int maxlen)
{
	DWORD wCount = maxlen; //�ɹ���ȡ�������ֽ���
	BOOL bReadStat = ReadFile(m_Hcom, //���ھ��
		buf, //�����׵�ַ
		wCount, //Ҫ��ȡ����������ֽ���
		&wCount, //DWORD*,�������շ��سɹ���ȡ�������ֽ���
		NULL); //NULLΪͬ�����ͣ�OVERLAPPED*Ϊ�첽����
	if (!bReadStat)
	{
		return 0;
	}
	return wCount;
}

void SttSerialPort::ReadCoilAndDisStatus(unsigned char i_Address, unsigned short i_StartAddress, unsigned char i_RegNum)
{
	uint8_t *pSendCommend = new uint8_t[8];
	pSendCommend[0] = (uint8_t)i_Address;
	pSendCommend[1] = 0x01;
	pSendCommend[2] = (uint8_t)(i_StartAddress / 256);
	pSendCommend[3] = (uint8_t)(i_StartAddress % 256);
	pSendCommend[4] = (uint8_t)(i_RegNum / 256);
	pSendCommend[5] = (uint8_t)(i_RegNum % 256);

	uint32_t crc = ModbusCrc16(pSendCommend, 6);
	pSendCommend[6] = (uint8_t)(crc % 256);
	pSendCommend[7] = (uint8_t)(crc / 256);

	Send(pSendCommend, 8);
	//int responseLength = 5 + 2 * i_RegNum;
}

void SttSerialPort::ReadKeepReg(unsigned char i_Address, unsigned short i_StartAddress, unsigned char i_RegNum)
{
	uint8_t *pSendCommend = new uint8_t[8];
	pSendCommend[0] = (uint8_t)i_Address;
	pSendCommend[1] = 0x03;
	pSendCommend[2] = (uint8_t)(i_StartAddress / 256);
	pSendCommend[3] = (uint8_t)(i_StartAddress % 256);
	pSendCommend[4] = (uint8_t)(i_RegNum / 256);
	pSendCommend[5] = (uint8_t)(i_RegNum % 256);

	uint32_t crc = ModbusCrc16(pSendCommend, 6);
	pSendCommend[6] = (uint8_t)(crc % 256);
	pSendCommend[7] = (uint8_t)(crc / 256);

	Send(pSendCommend, 8);
}

unsigned int SttSerialPort::ModbusCrc16(unsigned char *data, unsigned char len)
{
	uint8_t i;
	uint32_t reg_crc = 0xffff;
	while (len--)
	{
		reg_crc ^= *data++;
		for (i = 0; i < 8; i++)
		{
			if (reg_crc & 0x01)
			{
				reg_crc = (reg_crc >> 1) ^ 0xA001;
			}
			else
			{
				reg_crc = reg_crc >> 1;
			}
		}
	}
	return reg_crc;
}