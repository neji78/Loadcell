#ifndef READTHREAD_H
#define READTHREAD_H

#include<QThread>
#include <QMutex>
#include<QSerialPort>

#define FIRST_START_BYTE 0x55
#define FIRST_CHECKSUM_BYTE 115
#define SECOND_CHECKSUM_BYTE 116
#define PACKET_LENGTH 120


class ReadThread:public QThread
{
public:
    Q_OBJECT
public:
    explicit ReadThread(QObject *parent = nullptr);
    ~ReadThread() override;
    void Transaction (const QString &portName, qint32 buadRate);
    bool triggerQuit = false;
    QSerialPort serialPort;
    QMutex spDoor;
    QMutex door;
    bool flag;
    bool sync;
    int data_size;
signals:
    void ISReady(QByteArray buffer);

private:
    void run() override;
    QString m_portName;
    QString m_request;
    qint32 m_buadRate;
    QByteArray m_buffer;
    quint8* m_data;
    qint64 m_bufSize;
    bool trigger = false;
    int m_waitTimeout = 0;


};

#endif // READTHREAD_H
