#pragma once
#include <Windows.h>
class SttSerialPort
{
public:
	SttSerialPort();
	~SttSerialPort();
	/*
	�򿪴���
	portname(������)
	baudrate(������)
	parity(У��λ)
	databit(����λ)
	stopbit(ֹͣλ)
	*/
	bool OpenSerialPort(const char* portname, int baudrate, char parity, char databit, char stopbit);
	//�رմ���
	void CloseSerialPort();
	//����
	int Send(const void *buf, int len);
	//����
	int Receive(void *buf, int maxlen);
	//������0x01��ȡ��Ȧ/��ɢ�����״̬
	void ReadCoilAndDisStatus(unsigned char i_Address, unsigned short i_StartAddress, unsigned char i_RegNum);
	//������0x03��ȡ���ּĴ���ֵ
	void ReadKeepReg(unsigned char i_Address, unsigned short i_StartAddress, unsigned char i_RegNum);

	unsigned int ModbusCrc16(unsigned char *data, unsigned char len);
private:
	HANDLE m_Hcom = nullptr;
	int m_pHandle[16];
};

