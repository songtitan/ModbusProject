#pragma once

#include <QtWidgets/QWidget>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
class SttSerialPort;
class Master : public QWidget
{
    Q_OBJECT
public:
    Master(QWidget *parent = Q_NULLPTR);
	void iniUi();
	QStringList getEnableCommPort();
	QString getComm(int nIndex, bool bValue);
private slots:
	void ModeSelection(int index);
	void OnSerialPortBtnClicked();
	void OnSendBtnClicked();

	void Read();
private:
	QGroupBox *m_pSerialPortSet = nullptr;
	QGroupBox *m_pNetworkSet = nullptr;
	QComboBox *m_pSerialPortSel = nullptr;
	QComboBox *m_pBaudrateSel = nullptr;
	QComboBox *m_pParity = nullptr;
	QComboBox *m_pDataBit = nullptr;
	QComboBox *m_pStopBit = nullptr;
	QPushButton *m_pOpenSerialPortBtn = nullptr;
	QSpinBox *m_pId = nullptr;
	QComboBox *m_pFunction = nullptr;
	QSpinBox *m_pAddress = nullptr;
	QSpinBox *m_pQuantity = nullptr;
	QPushButton *m_pMonitorIpBtn = nullptr;

	bool m_bSerialPort = false;
	SttSerialPort *m_pSerialPort = nullptr;

};
