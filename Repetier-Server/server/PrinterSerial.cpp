/*
 Copyright 2012-2013 Hot-World GmbH & Co. KG
 Author: Roland Littwin (repetier) repetierdev@gmail.com
 Homepage: http://www.repetier.com
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 */

#define _CRT_SECURE_NO_WARNINGS // Disable deprecation warning in VS2005
#define _CRT_SECURE_NO_DEPRECATE 
#define _SCL_SECURE_NO_DEPRECATE 

#include "PrinterSerial.h"
#include "printer.h"
#include "PrinterConfigiration.h"
#ifdef __APPLE__
#include <sys/ioctl.h>
#include <termios.h>
#include <IOKit/serial/ioss.h>
#endif
#ifdef __linux
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/serial.h>
#endif
#include "RLog.h"

using namespace boost;
using namespace std;

void PrinterSerialPort::set_baudrate(int baud) {
    try {
#ifdef __APPLE__
        termios ios;
        asio::detail::reactive_descriptor_service::implementation_type &rs = get_implementation();
        int handle = get_service().native_handle(rs);
        ::tcgetattr(handle, &ios);
        ::cfsetspeed(&ios, baud);
        speed_t newSpeed = baud;
        ioctl(handle, IOSSIOSPEED, &newSpeed);
        ::tcsetattr(handle, TCSANOW, &ios);
#else
        try {
            boost::asio::serial_port_base::baud_rate baudrate = boost::asio::serial_port_base::baud_rate(baud);
            set_option(baudrate);
        } catch(std::exception e) {
#ifdef __linux
            termios ios;
            asio::detail::reactive_descriptor_service::implementation_type &rs = get_implementation();
            int handle = get_service().native_handle(rs);

            ::tcgetattr(handle, &ios);
            ::cfsetispeed(&ios, B38400);
            ::cfsetospeed(&ios, B38400);
            ::tcflush(handle, TCIFLUSH);
            ::tcsetattr(handle, TCSANOW, &ios);

            struct serial_struct ss;
            ioctl(handle, TIOCGSERIAL, &ss);
            ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
            ss.custom_divisor = (ss.baud_base + (baud / 2)) / baud;
            //cout << "bbase " << ss.baud_base << " div " << ss.custom_divisor;
            long closestSpeed = ss.baud_base / ss.custom_divisor;
            //cout << " Closest speed " << closestSpeed << endl;
            ss.reserved_char[0] = 0;
            if (closestSpeed < baud * 98 / 100 || closestSpeed > baud * 102 / 100) {
                RLog::log("error: couldn't set desired baud rate @",baud);
                throw e;
            }
            
            ioctl(handle, TIOCSSERIAL, &ss);
            
#else
            RLog::log("Setting baudrate @ failed",baud);
            throw e;
#endif
        }
#endif
    } catch(std::exception e) {
        RLog::log("Setting baudrate @ failed",baud,true);
        throw e;
    }
    
}
void PrinterSerialPort::setDTR(bool on) {
#if defined(_WIN32) && !defined(__SYMBIAN32__) // Windows specific
	asio::basic_io_object<asio::serial_port_service>::implementation_type &rs = get_implementation();
	boost::asio::serial_port_service::native_handle_type handle = get_service().native_handle(rs); //m_port.native();
    if(on)
        EscapeCommFunction( handle, SETDTR );
    else
        EscapeCommFunction( handle, CLRDTR );
#else
    asio::detail::reactive_descriptor_service::implementation_type &rs = get_implementation();
    int handle = get_service().native_handle(rs);
    int status;
    ioctl(handle, TIOCMGET, &status);
    if(on)
        status |= TIOCM_DTR;
    else
        status &= ~TIOCM_DTR;
    ioctl(handle, TIOCMSET, &status);
#endif
}
void PrinterSerialPort::debugTermios() {
#ifdef __APPLE__
#ifdef DEBUGTERM
    termios ios;
    asio::detail::reactive_descriptor_service::implementation_type &rs = get_implementation();
    int handle = get_service().native_handle(rs);
    ::tcgetattr(handle, &ios);
    cout << "Termios speed: " << ios.c_ispeed << " flags:" << ios.c_cflag << "," << ios.c_iflag << ","
    << ios.c_lflag << "," << ios.c_oflag << "," << ios.c_cc << "," << ios.c_ospeed << endl;
#endif
#endif
}

PrinterSerial::PrinterSerial(Printer &prt):io(),port(io) {
    open = error = false;
    printer = &prt;
    flowControl = boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none);
    stopBits = boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one);
    parity = asio::serial_port_base::parity(asio::serial_port_base::parity::none);
    characterSize = asio::serial_port_base::character_size(8);
    readString = "";
}

PrinterSerial::~PrinterSerial()
{
    if(isOpen())
    {
        try {
            close();
        } catch(...)
        {
            //Don't throw from a destructor
        }
    }
}
// Returns true if printer is connected
bool PrinterSerial::isConnected() {
    return isOpen();
}
// Tries to connect to printer
bool PrinterSerial::tryConnect() {
    try {
        if(port.is_open()) port.close();
        if(io.stopped()) {
            io.reset();
            writeBuffer.reset();
            writeBufferSize=0;
            writeQueue.clear();
        }
        setErrorStatus(true);//If an exception is thrown, error_ remains true
        baudrate = asio::serial_port_base::baud_rate(printer->config->serialBaudrate);
        port.open(printer->config->serialPort);
        port.debugTermios();
        port.set_option(parity);
        port.debugTermios();
        port.set_option(characterSize);
        port.debugTermios();
        port.set_option(flowControl);
        port.debugTermios();
        port.set_option(stopBits);
        port.debugTermios();
        port.set_baudrate(printer->config->serialBaudrate);
        port.debugTermios();
        //This gives some work to the io_service before it is started
        io.post(boost::bind(&PrinterSerial::doRead, this));
        thread t(boost::bind(&asio::io_service::run, &io));
        backgroundThread.swap(t);
        setErrorStatus(false);//If we get here, no error
        open=true; //Port is now open
        RLog::log("Connection started:@",printer->config->name);
        resetPrinter();
    } catch (std::exception& e)
    {
#ifdef DEBUG
        //  cerr << "Exception: " << e.what() << "\n";
#endif
        return false;
    }
    catch(...) {
        
        return false;
    }
    return true;
}
void PrinterSerial::close() {
    if(!isOpen()) return;
    io.post(boost::bind(&PrinterSerial::doClose, this));
    backgroundThread.join();
    //io.reset();
    if(errorStatus())
    {
        throw(boost::system::system_error(boost::system::error_code(),
                                          "Error while closing the device"));
    }
}
bool PrinterSerial::isOpen() {
    return open;
}
bool PrinterSerial::errorStatus() const
{
    lock_guard<mutex> l(errorMutex);
    return error;
}

void PrinterSerial::setErrorStatus(bool e) {
    lock_guard<mutex> l(errorMutex);
    error = e;
}
void PrinterSerial::writeString(const std::string& s)
{
    {
        lock_guard<mutex> l(writeQueueMutex);
        writeQueue.insert(writeQueue.end(),s.begin(),s.end());
    }
    io.post(boost::bind(&PrinterSerial::doWrite, this));
}
void PrinterSerial::writeBytes(const uint8_t* data,size_t len) {
    {
        lock_guard<mutex> l(writeQueueMutex);
        writeQueue.insert(writeQueue.end(),data,data+len);
    }
    io.post(boost::bind(&PrinterSerial::doWrite, this));    
}

void PrinterSerial::doRead() {
    port.async_read_some(asio::buffer(readBuffer,READ_BUFFER_SIZE),
                                boost::bind(&PrinterSerial::readEnd,
                                            this,
                                            asio::placeholders::error,
                                            asio::placeholders::bytes_transferred));
}
void PrinterSerial::readEnd(const boost::system::error_code& error,
                          size_t bytes_transferred)
{
    if(error)
    {
#ifdef __APPLE__
        if(error.value()==45)
        {
            //Bug on OS X, it might be necessary to repeat the setup
            //http://osdir.com/ml/lib.boost.asio.user/2008-08/msg00004.html
            doRead();
            return;
        }
#endif //__APPLE__
        //error can be true even because the serial port was closed.
        //In this case it is not a real error, so ignore
        if(isOpen())
        {
            RLog::log("error: Reading serial conection failed: @. Closing connection.",error.message());
            doClose();
            setErrorStatus(true);
        }
    } else {
        size_t lstart = 0;
        for(size_t i=0;i<bytes_transferred;i++) {
            char c = readBuffer[i];
            if(c=='\n' || c=='\r') {
                readString.append(&readBuffer[lstart],i-lstart);
                lstart = i+1;
                if(readString.length()>0)
                    printer->analyseResponse(readString);
                readString = "";
            }
        }
        readString.append(&readBuffer[lstart],bytes_transferred-lstart);
        doRead(); // Continue reading serial port
    }
}
void PrinterSerial::doWrite()
{
    //If a write operation is already in progress, do nothing
    if(writeBuffer==0)
    {
        lock_guard<mutex> l(writeQueueMutex);
        writeBufferSize=writeQueue.size();
        writeBuffer.reset(new char[writeQueue.size()]);
        copy(writeQueue.begin(),writeQueue.end(),
             writeBuffer.get());
        writeQueue.clear();
        async_write(port,asio::buffer(writeBuffer.get(),
                                             writeBufferSize),
                    boost::bind(&PrinterSerial::writeEnd, this, asio::placeholders::error));
    }
}
void PrinterSerial::writeEnd(const boost::system::error_code& error)
{
    if(!error)
    {
        lock_guard<mutex> l(writeQueueMutex);
        if(writeQueue.empty())
        {
            writeBuffer.reset();
            writeBufferSize=0;
            return;
        }
        writeBufferSize=writeQueue.size();
        writeBuffer.reset(new char[writeQueue.size()]);
        copy(writeQueue.begin(),writeQueue.end(),
             writeBuffer.get());
        writeQueue.clear();
        async_write(port,asio::buffer(writeBuffer.get(),
                                             writeBufferSize),
                    boost::bind(&PrinterSerial::writeEnd, this, asio::placeholders::error));
    } else {
        setErrorStatus(true);
        doClose();
    }
}
void PrinterSerial::doClose()
{
    if(!open) return;
    open = false;
    boost::system::error_code ec;
    port.cancel(ec);
    if(ec) setErrorStatus(true);
    port.close(ec);
    if(ec) setErrorStatus(true);
    printer->connectionClosed();
    RLog::log("Connection closed: @",printer->config->name);
}

// Send reset to the printer by toggling DTR line
void PrinterSerial::resetPrinter() {
	RLog::log("Reset printer @",printer->config->name);
    port.setDTR(false);
    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    port.setDTR(true);
    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    port.setDTR(false);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));    
}
