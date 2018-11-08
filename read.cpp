#include <windows.h>
#include <iostream>
#include <fstream>
//для работы с элементами в строке
using namespace std;

//обьявление обработчика com порта:
HANDLE hSerial;
//HANDLE - дескриптор, т.е. число, с помощью которого можно идентифицировать ресурс.

//функция чтения 
void
ReadCOM() {
    DWORD iSize;
    char sReceivedChar; // буфер данных

    ofstream data1;
    data1.open("stream.txt");
    while(true) {
        ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);  // получаем 1 байт
        if (iSize != 1) {
            int error_code = GetLastError();
            cout << "Read failed, error code: " << error_code << ".\n";
            break;
        }

        cout << sReceivedChar;
        data1 << sReceivedChar;
        data1.flush();
    }
    data1.close();

}


int
main(int argc, TCHAR *argv[])
// _TCHAR* -тип данных Tchar.h
// _t - юникод
{
    LPCTSTR sPortName = "COM7";

    //Открываем первый com для чтения или записи
    hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


    //проверка на работоспособность:
    if (hSerial == INVALID_HANDLE_VALUE) {
        int error_code = GetLastError();
        cout << "Unable to access port, error code: " << error_code << ".\n"; //Попытка открыть не существующий порт
        return 1;
    }


    //настройка параметров соединения:
    DCB dcbSerialParams = {0};  //структура ком порта
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams); //раземер структуры

    if (!::GetCommState(hSerial, &dcbSerialParams)) {
        int error_code = GetLastError();
        cout << "Getting state error, error code: " << error_code << ".\n";
        return 1;
    }
    dcbSerialParams.BaudRate = 460800;  //скорость потока
    dcbSerialParams.ByteSize = 8;           //определяет число информационных бит в передаваемых и принимаемых байтах
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;      //отсутствие бита четности

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        int error_code = GetLastError();
        cout << "Error setting serial port state, error code: " << error_code << ".\n";
        return 1;
    }


    ReadCOM();
    return 0;
}

