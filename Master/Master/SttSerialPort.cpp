#include "SttSerialPort.h"
#include <Windows.h>
#include <qdebug.h>

SttSerialPort::SttSerialPort()
{

}
SttSerialPort::~SttSerialPort()
{

}

//打开串口
bool SttSerialPort::OpenSerialPort(const char* portname, int baudrate, char parity, char databit, char stopbit)
{
	m_Hcom = CreateFileA(portname, //串口名
		GENERIC_READ | GENERIC_WRITE, //支持读写
		0, //独占方式，串口不支持共享
		NULL,//安全属性指针，默认值为NULL
		OPEN_EXISTING, //打开现有的串口文件
		0, //0：同步方式，FILE_FLAG_OVERLAPPED：异步方式
		NULL);//用于复制文件句柄，默认值为NULL，对串口而言该参数必须置为NULL

	if (m_Hcom == (HANDLE)-1)
	{
		return false;
	}

	//配置缓冲区大小 
	if (!SetupComm(m_Hcom, 1024, 1024))
	{
		return false;
	}

	// 配置参数 
	DCB p;
	memset(&p, 0, sizeof(p));
	p.DCBlength = sizeof(p);
	p.BaudRate = baudrate; // 波特率
	p.ByteSize = databit; // 数据位

	switch (parity) //校验位
	{
	case 0:
		p.Parity = NOPARITY; //无校验
		break;
	case 1:
		p.Parity = ODDPARITY; //奇校验
		break;
	case 2:
		p.Parity = EVENPARITY; //偶校验
		break;
	}

	switch (stopbit) //停止位
	{
	case 1:
		p.StopBits = ONESTOPBIT; //1位停止位
		break;
	case 2:
		p.StopBits = TWOSTOPBITS; //2位停止位
		break;
	case 3:
		p.StopBits = ONE5STOPBITS; //1.5位停止位
		break;
	}

	if (!SetCommState(m_Hcom, &p))
	{
		// 设置参数失败
		return false;
	}

	//超时处理,单位：毫秒
	//总超时＝时间系数×读或写的字符数＋时间常量
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout = 1000; //读间隔超时
	TimeOuts.ReadTotalTimeoutMultiplier = 500; //读时间系数
	TimeOuts.ReadTotalTimeoutConstant = 5000; //读时间常量
	TimeOuts.WriteTotalTimeoutMultiplier = 500; // 写时间系数
	TimeOuts.WriteTotalTimeoutConstant = 2000; //写时间常量
	SetCommTimeouts(m_Hcom, &TimeOuts);

	PurgeComm(m_Hcom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空串口缓冲区

	memcpy(m_pHandle, &m_Hcom, sizeof(m_Hcom));// 保存句柄
	return true;
}
//关闭串口
void SttSerialPort::CloseSerialPort()
{
	CloseHandle(m_Hcom);
}

int SttSerialPort::Send(const void *buf, int len)
{
	DWORD dwBytesWrite = len; //成功写入的数据字节数
	BOOL bWriteStat = WriteFile(m_Hcom, //串口句柄
		buf, //数据首地址
		dwBytesWrite, //要发送的数据字节数
		&dwBytesWrite, //DWORD*，用来接收返回成功发送的数据字节数
		NULL); //NULL为同步发送，OVERLAPPED*为异步发送
	if (!bWriteStat)
	{
		return 0;
	}
	return dwBytesWrite;
}

int SttSerialPort::Receive(void *buf, int maxlen)
{
	DWORD wCount = maxlen; //成功读取的数据字节数
	BOOL bReadStat = ReadFile(m_Hcom, //串口句柄
		buf, //数据首地址
		wCount, //要读取的数据最大字节数
		&wCount, //DWORD*,用来接收返回成功读取的数据字节数
		NULL); //NULL为同步发送，OVERLAPPED*为异步发送
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