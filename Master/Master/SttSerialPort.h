#pragma once
#include <Windows.h>
class SttSerialPort
{
public:
	SttSerialPort();
	~SttSerialPort();
	/*
	打开串口
	portname(串口名)
	baudrate(波特率)
	parity(校验位)
	databit(数据位)
	stopbit(停止位)
	*/
	bool OpenSerialPort(const char* portname, int baudrate, char parity, char databit, char stopbit);
	//关闭串口
	void CloseSerialPort();
	//发送
	int Send(const void *buf, int len);
	//接收
	int Receive(void *buf, int maxlen);
	//功能码0x01读取线圈/离散量输出状态
	void ReadCoilAndDisStatus(unsigned char i_Address, unsigned short i_StartAddress, unsigned char i_RegNum);
	//功能码0x03读取保持寄存器值
	void ReadKeepReg(unsigned char i_Address, unsigned short i_StartAddress, unsigned char i_RegNum);

	unsigned int ModbusCrc16(unsigned char *data, unsigned char len);
private:
	HANDLE m_Hcom = nullptr;
	int m_pHandle[16];
};

