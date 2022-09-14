#include "readthread.h"
#include <QDebug>


ReadThread::ReadThread(QObject *parent):
    QThread(parent)
{
}

void ReadThread::Transaction(const QString &portName, qint32 buadRate)
{
    //const QMutexLocker locker ( &door);
    m_portName = portName;
    triggerQuit = false;
    m_buadRate = buadRate;
    if ( !isRunning())
        start();
}

void ReadThread::run()
{ 
    flag = false;
    sync = false;
    data_size = PACKET_LENGTH;
    serialPort.setPortName(m_portName);
    serialPort.open(QIODevice::ReadWrite);
    serialPort.setBaudRate(m_buadRate,QSerialPort::AllDirections);
    while(!triggerQuit){
        if(serialPort.waitForReadyRead(100))
        {
            m_buffer.append(serialPort.read(data_size));
            if (!sync) {
                qDebug()<<"unsync";
                //Check start & checksum bytes
                if (!flag) {
                    if(m_buffer.indexOf((char)FIRST_START_BYTE,0) < 0) {
                        m_buffer.clear();
                        data_size = PACKET_LENGTH;
                    } else {
                        m_buffer.remove(0,m_buffer.indexOf((char)FIRST_START_BYTE,0));
                        data_size = PACKET_LENGTH - m_buffer.size();
                        flag = true;
                    }
                } else if(m_buffer.size() >= PACKET_LENGTH) {
                    if((int)m_buffer[PACKET_LENGTH - 2]== FIRST_CHECKSUM_BYTE) {
                        sync = true;
                        emit ISReady(m_buffer.left(PACKET_LENGTH));
                        m_buffer.remove(0,PACKET_LENGTH);
                        data_size = PACKET_LENGTH;
                    } else {
                        m_buffer.remove(0,1);
                        data_size = 1;
                    }
                    flag = false;
                }
            } else if(m_buffer.size() >= PACKET_LENGTH) {
                if((int)m_buffer[0] == FIRST_START_BYTE &&
                        (int)m_buffer[PACKET_LENGTH - 2] == FIRST_CHECKSUM_BYTE) {
                    emit ISReady(m_buffer.left(PACKET_LENGTH));
                    m_buffer.remove(0,PACKET_LENGTH);
                } else {
                    sync = false;
                    m_buffer.remove(0,PACKET_LENGTH);
                }
            }
        }
    }
}
ReadThread::~ReadThread()
{
    door.lock();
    triggerQuit = true;
    door.unlock();
    if(wait())
        serialPort.close();
}
