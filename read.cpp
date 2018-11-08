#include <windows.h>
#include <iostream>
#include <fstream>
//��� ������ � ���������� � ������
using namespace std;

//���������� ����������� com �����:
HANDLE hSerial;
//HANDLE - ����������, �.�. �����, � ������� �������� ����� ���������������� ������.

//������� ������ 
void
ReadCOM() {
    DWORD iSize;
    char sReceivedChar; // ����� ������

    ofstream data1;
    data1.open("stream.txt");
    while(true) {
        ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);  // �������� 1 ����
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
// _TCHAR* -��� ������ Tchar.h
// _t - ������
{
    LPCTSTR sPortName = "COM7";

    //��������� ������ com ��� ������ ��� ������
    hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


    //�������� �� �����������������:
    if (hSerial == INVALID_HANDLE_VALUE) {
        int error_code = GetLastError();
        cout << "Unable to access port, error code: " << error_code << ".\n"; //������� ������� �� ������������ ����
        return 1;
    }


    //��������� ���������� ����������:
    DCB dcbSerialParams = {0};  //��������� ��� �����
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams); //������� ���������

    if (!::GetCommState(hSerial, &dcbSerialParams)) {
        int error_code = GetLastError();
        cout << "Getting state error, error code: " << error_code << ".\n";
        return 1;
    }
    dcbSerialParams.BaudRate = 460800;  //�������� ������
    dcbSerialParams.ByteSize = 8;           //���������� ����� �������������� ��� � ������������ � ����������� ������
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;      //���������� ���� ��������

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        int error_code = GetLastError();
        cout << "Error setting serial port state, error code: " << error_code << ".\n";
        return 1;
    }


    ReadCOM();
    return 0;
}

