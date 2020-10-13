#include "Master.h"
#include "SttSerialPort.h"
#include <qlayout.h>
#include <qdebug.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qtextedit.h>
#include <qtablewidget.h>
#include <qlineedit.h>
#include <qheaderview.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <string>
Master::Master(QWidget *parent)
    : QWidget(parent)
{
	iniUi();
}

void Master::iniUi()
{
	setWindowTitle(tr("Modbus Master"));

	QHBoxLayout *pHlay = new QHBoxLayout();

	QVBoxLayout *pVlay = new QVBoxLayout();

	QGroupBox *pModeSel = new QGroupBox(this);
	pModeSel->setMaximumHeight(80);
	pModeSel->setMaximumWidth(340);
	pModeSel->setTitle(tr("Mode setting"));
	QHBoxLayout *pModeSelLay = new QHBoxLayout();
	QRadioButton *pRtuBtn = new QRadioButton();
	pRtuBtn->setText(tr("Modbus RTU"));
	QRadioButton *pTcpBtn = new QRadioButton();
	pTcpBtn->setText(tr("Modbus TCP"));
	QButtonGroup *pRtuTcpGrp = new QButtonGroup();
	pRtuTcpGrp->addButton(pRtuBtn, 1);
	pRtuTcpGrp->addButton(pTcpBtn, 2);
	pRtuBtn->setChecked(true);
	connect(pRtuTcpGrp, SIGNAL(buttonClicked(int)), this, SLOT(ModeSelection(int)));
	pModeSelLay->addWidget(pRtuBtn);
	pModeSelLay->addWidget(pTcpBtn);
	pModeSel->setLayout(pModeSelLay);


	m_pSerialPortSet = new QGroupBox(this);
	m_pSerialPortSet->setTitle(tr("Serial port setting"));
	m_pSerialPortSet->setMaximumWidth(340);
	QGridLayout *pSerialPortLay = new QGridLayout();
	QLabel *pSerialPortSelLab = new QLabel();
	pSerialPortSelLab->setText(tr("Port"));
	m_pSerialPortSel = new QComboBox();
	QStringList comList = getEnableCommPort();
	for (int i = 0; i < comList.length(); i++)
	{
		m_pSerialPortSel->addItem(comList[i]);
	}
	QLabel *pBaudRateSelLab = new QLabel();
	pBaudRateSelLab->setText(tr("Baudrate"));
	m_pBaudrateSel = new QComboBox();
	m_pBaudrateSel->addItem("1200");
	m_pBaudrateSel->addItem("4800");
	m_pBaudrateSel->addItem("9600");
	m_pBaudrateSel->addItem("19200");
	m_pBaudrateSel->addItem("38400");
	m_pBaudrateSel->addItem("57600");
	m_pBaudrateSel->addItem("115200");
	m_pBaudrateSel->setCurrentIndex(2);
	QLabel *pDataBitLab = new QLabel();
	pDataBitLab->setText(tr("Data bit"));
	m_pDataBit = new QComboBox();
	m_pDataBit->addItem("7");
	m_pDataBit->addItem("8");
	m_pDataBit->setCurrentIndex(1);
	QLabel *pStopBitLab = new QLabel();
	pStopBitLab->setText(tr("Stop bit"));
	m_pStopBit = new QComboBox();
	m_pStopBit->addItem("1");
	m_pStopBit->addItem("2");
	m_pStopBit->addItem("1.5");
	m_pStopBit->setCurrentIndex(0);
	QLabel *pParityLab = new QLabel();
	pParityLab->setText(tr("Parity"));
	m_pParity = new QComboBox();
	m_pParity->addItem(tr("None"));
	m_pParity->addItem(tr("Odd"));
	m_pParity->addItem(tr("Even"));
	m_pParity->setCurrentIndex(0);
	m_pOpenSerialPortBtn = new QPushButton();
	m_pOpenSerialPortBtn->setText(tr("Open serial port"));
	connect(m_pOpenSerialPortBtn, SIGNAL(clicked()), this, SLOT(OnSerialPortBtnClicked()));

	pSerialPortLay->addWidget(pSerialPortSelLab,0,0,1,1);
	pSerialPortLay->addWidget(m_pSerialPortSel,0,1,1,1);
	pSerialPortLay->addWidget(pBaudRateSelLab,1,0,1,1);
	pSerialPortLay->addWidget(m_pBaudrateSel,1,1,1,1);
	pSerialPortLay->addWidget(pDataBitLab,2,0,1,1);
	pSerialPortLay->addWidget(m_pDataBit,2,1,1,1);
	pSerialPortLay->addWidget(pStopBitLab,3,0,1,1);
	pSerialPortLay->addWidget(m_pStopBit,3,1,1,1);
	pSerialPortLay->addWidget(pParityLab,4,0,1,1);
	pSerialPortLay->addWidget(m_pParity,4,1,1,1);
	pSerialPortLay->addWidget(m_pOpenSerialPortBtn,5,0,1,2);
	m_pSerialPortSet->setLayout(pSerialPortLay);


	m_pNetworkSet = new QGroupBox(this);
	m_pNetworkSet->setTitle(tr("Network settings"));
	m_pNetworkSet->setMaximumWidth(340);
	m_pNetworkSet->setDisabled(true);
	QGridLayout *pNetworkLay = new QGridLayout();
	QLabel *pIpAdressLab = new QLabel();
	pIpAdressLab->setText(tr("IP"));
	QLineEdit *pIpAdressEdit = new QLineEdit();
	QLabel *pPortLab = new QLabel();
	pPortLab->setText(tr("Port"));
	QLineEdit *pPortEdit = new QLineEdit();
	pPortEdit->setText("502");
	m_pMonitorIpBtn = new QPushButton();
	m_pMonitorIpBtn->setText(tr("Monitor"));
	pNetworkLay->addWidget(pIpAdressLab,0,0,1,1);
	pNetworkLay->addWidget(pIpAdressEdit,0,1,1,1);
	pNetworkLay->addWidget(pPortLab,1,0,1,1);
	pNetworkLay->addWidget(pPortEdit,1,1,1,1);
	pNetworkLay->addWidget(m_pMonitorIpBtn, 2, 0, 1, 2);
	m_pNetworkSet->setLayout(pNetworkLay);


	QGroupBox *pReadWriteDef = new QGroupBox(this);
	QGridLayout *pReadWriteLay = new QGridLayout();
	pReadWriteDef->setTitle(tr("Read/Write Definition"));
	QLabel *pIdLab = new QLabel();
	pIdLab->setText(tr("ID"));
	m_pId = new QSpinBox();
	m_pId->setMinimum(1);
	m_pId->setMaximum(255);
	m_pId->setValue(1);
	QLabel *pFunctionLab = new QLabel();
	pFunctionLab->setText(tr("Function"));
	m_pFunction = new QComboBox();
	m_pFunction->addItem(tr("01 Read Coils"), 0x01);
	m_pFunction->addItem(tr("03 Read Holding Registers"), 0x03);
	m_pFunction->addItem(tr("15 Write Multiple Coils"), 0x0F);
	m_pFunction->addItem(tr("16 Write Multiple Registers"), 0x10);
	QLabel *pAddressLab = new QLabel();
	pAddressLab->setText(tr("Address"));
	m_pAddress = new QSpinBox();
	m_pAddress->setMinimum(0);
	m_pAddress->setMaximum(65535);
	QLabel *pQuantityLab = new QLabel();
	pQuantityLab->setText(tr("Quantity"));
	m_pQuantity = new QSpinBox();
	m_pQuantity->setMinimum(0);
	m_pQuantity->setMaximum(2000);
	m_pQuantity->setValue(1);

	pReadWriteLay->addWidget(pIdLab,0,0,1,1);
	pReadWriteLay->addWidget(m_pId,0,1,1,1);
	pReadWriteLay->addWidget(pFunctionLab,1,0,1,1);
	pReadWriteLay->addWidget(m_pFunction,1,1,1,1);
	pReadWriteLay->addWidget(pAddressLab,2,0,1,1);
	pReadWriteLay->addWidget(m_pAddress,2,1,1,1);
	pReadWriteLay->addWidget(pQuantityLab,3,0,1,1);
	pReadWriteLay->addWidget(m_pQuantity,3,1,1,1);
	pReadWriteDef->setLayout(pReadWriteLay);

	QPushButton *pSendBtn = new QPushButton();
	pSendBtn->setText(tr("Send"));
	connect(pSendBtn, SIGNAL(clicked()), this, SLOT(OnSendBtnClicked()));

	pVlay->addWidget(pModeSel);
	pVlay->addWidget(m_pSerialPortSet);
	pVlay->addWidget(m_pNetworkSet);
	pVlay->addWidget(pReadWriteDef);
	pVlay->addWidget(pSendBtn);

	QPushButton *pRead = new QPushButton();
	pRead->setText(tr("Read"));
	pVlay->addWidget(pRead);
	connect(pRead, SIGNAL(clicked()), this, SLOT(Read()));

	QVBoxLayout *pDisplayContentLay = new QVBoxLayout();

	QGroupBox *pRequestsAndResponses = new QGroupBox(this);
	pRequestsAndResponses->setTitle(tr("Requests and responses"));
	QVBoxLayout *pReqAndResLay = new QVBoxLayout();
	QTextEdit *pReqAndResEdit = new QTextEdit();
	pReqAndResEdit->setMinimumSize(500, 200);

	QHBoxLayout *pClearLay = new QHBoxLayout();
	QPushButton *pClearReqAndResBtn = new QPushButton();
	pClearReqAndResBtn->setText(tr("Clear"));
	pClearLay->addStretch();
	pClearLay->addWidget(pClearReqAndResBtn);


	pReqAndResLay->addWidget(pReqAndResEdit);
	pReqAndResLay->addLayout(pClearLay);
	pRequestsAndResponses->setLayout(pReqAndResLay);

	QGroupBox *pRegisters = new QGroupBox(this);
	pRegisters->setTitle(tr("Registers"));
	QHBoxLayout *pRegistersLay = new QHBoxLayout();
	QTableWidget *pRegistersTable = new QTableWidget();
	pRegistersTable->setMinimumSize(500, 300);
	pRegistersTable->setColumnCount(3);
	pRegistersTable->horizontalHeader()->setStretchLastSection(true);
	pRegistersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	
	QStringList tableTitleList;
	tableTitleList << tr("Data Type") << tr("Register") << tr("Data");
	pRegistersTable->setHorizontalHeaderLabels(tableTitleList);
	

	pRegistersLay->addWidget(pRegistersTable);
	pRegisters->setLayout(pRegistersLay);
	
	pDisplayContentLay->addWidget(pRequestsAndResponses);
	pDisplayContentLay->addWidget(pRegisters);


	pHlay->addLayout(pVlay);
	pHlay->addLayout(pDisplayContentLay);
	setLayout(pHlay);
}

void Master::ModeSelection(int index)
{
	switch (index)
	{
	case 1:
		m_pNetworkSet->setDisabled(true);
		m_pSerialPortSet->setDisabled(false);
		break;
	case 2:
		m_pNetworkSet->setDisabled(false);
		m_pSerialPortSet->setDisabled(true);
		break;
	default:
		break;
	}
}


QStringList Master::getEnableCommPort()
{
	QStringList CommPortList;

	QString strCommPath = tr("HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM");
	QSettings *settings = new QSettings(strCommPath, QSettings::NativeFormat);
	QStringList keyList = settings->allKeys();

	if (!keyList.isEmpty())
	{
		for (int i = 0; i < keyList.count(); i++)
			CommPortList.append(getComm(i, true));
	}

	return CommPortList;
}

// nIndex为索引号 bValue选择返回值为key还是value，默认为value
QString Master::getComm(int nIndex, bool bValue)
{
	QString strCommRet = tr("");
	HKEY hKey;

	// RegOpenKeyEx windowsApi 成功返回0 需要加<qt_windows.h>头文件
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != 0)
	{
		QMessageBox::warning(this, tr("注册表"), tr("无法打开注册表！"), QMessageBox::Ok);
		return tr(""); // 无法打开注册表
	}

	QString strKeyMsg;      // 键名
	QString strValueMsg;    // 键值
	wchar_t keyName[256];   // 键名数组
	char keyValue[256];     // 键值数组

	ulong nKeySize = sizeof(keyName);
	ulong nValueSize = sizeof(keyValue);
	ulong nType;

	if (::RegEnumValue(hKey, nIndex, keyName, &nKeySize, 0, &nType, (BYTE*)keyValue, &nValueSize) == 0) // 列举键名
	{
		// 读取键名
		for (uint i = 0; i < nKeySize; i++)
		{
			if (keyName[i] != 0x00)
				strKeyMsg.append(keyName[i]);
		}

		// 读取键值
		for (uint j = 0; j < nValueSize; j++)
		{
			if (keyValue[j] != 0x00)
				strValueMsg.append(keyValue[j]);
		}

		if (bValue)
			strCommRet = strValueMsg;
		else
			strCommRet = strKeyMsg;
	}//if
	else
	{
		::RegCloseKey(hKey); // 关闭注册表
		return tr("");  // 无可用串口
	}

	return strCommRet;
}

void Master::OnSerialPortBtnClicked()
{
	if (!m_bSerialPort)
	{
		m_pSerialPort = new SttSerialPort();
		std::string portStr = m_pSerialPortSel->currentText().toStdString();
		const char *comStr = portStr.c_str();
		int baudRate = m_pBaudrateSel->currentText().toInt();
		char parity = (char)(m_pParity->currentIndex());
		char dataBit = (char)(m_pDataBit->currentText().toInt());
		char stopBit = (char)(m_pStopBit->currentIndex() + 1);
		if (m_pSerialPort->OpenSerialPort(comStr, baudRate, parity, dataBit, stopBit))
		{
			//QMessageBox::information(this, tr("Modbus Master"), tr("Open serial port successfully!"));
			m_pOpenSerialPortBtn->setText(tr("Shut down serial port"));
			m_bSerialPort = true;
		}
		else
		{
			QMessageBox::warning(this, tr("Modbus Master"), tr("Failed to open serial port!"));
		}
	}
	else
	{
		m_pSerialPort->CloseSerialPort();
		m_pOpenSerialPortBtn->setText(tr("Open serial port"));
		m_bSerialPort = false;
		delete m_pSerialPort;
		m_pSerialPort = nullptr;
	}

}

void Master::OnSendBtnClicked()
{
	if (!m_bSerialPort)
	{
		QMessageBox::warning(this, tr("Modbus Master"), tr("Please open the serial port!"));
		return;
	}
	int id = m_pId->value();
	int function = m_pFunction->currentData().toInt();
	int startAddress = m_pAddress->value();
	int quantity = m_pQuantity->value();
	switch (function)
	{
	case 1:
		m_pSerialPort->ReadCoilAndDisStatus(id, startAddress, quantity);
		break;
	case 3:
		m_pSerialPort->ReadKeepReg(id, startAddress, quantity);
		break;
	case 15:
		break;
	case 16:
		break;
	default:
		break;
	}
	//int id = m_pId->value();
	//int function = m_pFunction->currentData().toInt();
	//int startAddress = m_pAddress->value();
	//int quantity = m_pQuantity->value();

	//uint8_t *pSendCommend = new uint8_t[8];
	//pSendCommend[0] = (uint8_t)id;
	//pSendCommend[1] = (uint8_t)function;
	//pSendCommend[2] = (uint8_t)(startAddress / 256);
	//pSendCommend[3] = (uint8_t)(startAddress % 256);
	//pSendCommend[4] = (uint8_t)(quantity / 256);
	//pSendCommend[5] = (uint8_t)(quantity % 256);

	//uint32_t crc = m_pSerialPort->ModbusCrc16(pSendCommend, 6);
	//pSendCommend[6] = (uint8_t)(crc % 256);
	//pSendCommend[7] = (uint8_t)(crc / 256);

	//int responseLength = 5 + 2 * 3;
	//m_pSerialPort->Send(pSendCommend, 8);
}

void Master::Read()
{
	if (!m_bSerialPort)
	{
		QMessageBox::warning(this, tr("Modbus Master"), tr("Please open the serial port!"));
		return;
	}
	char buf[1024];
//	while (true)
//	{
		memset(buf, 0, 1024);
		m_pSerialPort->Receive(buf, 1024);
		qDebug() << "buf:" << buf;
//	}
}