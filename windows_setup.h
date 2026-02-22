#ifndef WINDOWS_SETUP_H
#define WINDOWS_SETUP_H

#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>

#include <conio.h>
#include <windows.h>

#include "topband.h"

#define serial HANDLE

#define BAUD(X) CBR_ ## X

serial open_serial(const char* port)
{
    serial handle;

    handle = CreateFile(
            port,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open serial port");
        exit(EXIT_FAILURE);
    }

    printf("Opened serial device: %s\n", port);

    return handle;
}

void close_serial(serial handle)
{
    if(!CloseHandle(handle))
    {
        fprintf(stderr, "Failed to close serial port\n");
    }
}

void configure_serial_port(serial hSerial, DWORD baudrate) {
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    // Get the current DCB settings
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error getting serial state: %d\n", GetLastError());
        return;
    }

    // Set basic parameters
    dcbSerialParams.BaudRate = baudrate; // Set baud rate
    dcbSerialParams.ByteSize = 8;         // Set character size (8 bits)
    dcbSerialParams.StopBits = ONESTOPBIT; // Set stop bits (1 stop bit)
    dcbSerialParams.Parity = NOPARITY;    // Set parity (none)

    // Set flow control (none)
    dcbSerialParams.fOutxCtsFlow = FALSE; // Disable CTS flow control
    dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE; // Disable RTS control
    dcbSerialParams.fDsrSensitivity = FALSE; // Disable DSR sensitivity

    // Apply the configuration
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error setting serial parameters: %d\n", GetLastError());
        return;
    }

    // Initial timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 300; // Timeout for read operations
    timeouts.ReadTotalTimeoutConstant = 300; // Total timeout for read operations
    timeouts.ReadTotalTimeoutMultiplier = 10; // Multiplier for timeout
    timeouts.WriteTotalTimeoutConstant = 2000; // Total timeout for write operations
    timeouts.WriteTotalTimeoutMultiplier = 500; // Multiplier for timeout

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        fprintf(stderr, "Error setting timeouts: %d\n", GetLastError());
        return;
    }
}


struct tb_command* query_bms_with_buffer(serial handle, const char* query_buffer,
        size_t query_buffer_size, size_t* num_responses)
{

    struct tb_command* responses = NULL;
    *num_responses = 0;
    

    DWORD bytes_sent = 0;
    if (!WriteFile(handle, query_buffer, query_buffer_size, &bytes_sent, NULL)) {
        fprintf(stderr, "Failed to send query_cmd");
    }
    //else {
    //    printf("Sent %d bytes: %s\n", bytes_sent, query_buffer);
    //}


    COMMTIMEOUTS timeouts;

    GetCommTimeouts(handle, &timeouts);
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 300;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    DWORD manual_timeout_ms = 300;

    const size_t max_buf_size = 256*16;
    char buffer[max_buf_size];

    int read_something = 1;
    int msg_size = 0;

    memset(buffer, 0, max_buf_size);
    while (read_something)
    {
        read_something = 0;
        DWORD bytes_read = 0;
        
        // Not sure if this is the right way to do this, but I'm not
        // seeing a direct windows equivalent to select with timeout
        DWORD errors;
        COMSTAT com_stat;

        DWORD waited_for_ms = 0;

        while(waited_for_ms < manual_timeout_ms)
        {
            ClearCommError(handle, &errors, &com_stat);
            if (com_stat.cbInQue > 0)
                break;
            Sleep(10);
            waited_for_ms += 10;
        }

        if(com_stat.cbInQue == 0)
        {
            break;
        }

        if(!ReadFile(handle, buffer+msg_size, max_buf_size-msg_size, &bytes_read, NULL))
        {
            fprintf(stderr, "Failed to receive data: %d\n", GetLastError());
            break;
        }
        if (bytes_read > 0) {
            read_something = 1;
            msg_size += bytes_read;
        }
    }

    if (0 == msg_size)
        return NULL;


    const char* next = buffer;
    while(next < buffer+msg_size)
    {
        if (*num_responses == 0)
        {
            (*num_responses)++;
            responses = (struct tb_command*) malloc(sizeof(struct tb_command));
        }
        else
        {
            (*num_responses)++;
            responses = (struct tb_command*) realloc(responses, sizeof(struct tb_command)*(*num_responses));
        }
        int res = tb_decode(next, max_buf_size-(next-buffer), &responses[*num_responses-1], &next);
        if (res < 0)
        {
            if(res == -7)
            {
                // This is proprietary encoding
                break;
            }
            printf("Decode error: %d\n",res);
            break;
        }
        //printf("response:\n");
        //printf("  VER:        %02X\n", responses[*num_responses-1].version);
        //printf("  ADR:        %02X\n", responses[*num_responses-1].adr);
        //printf("  CID1:       %02X\n", responses[*num_responses-1].cid1);
        //printf("  CID2 (RTN): %02X\n", responses[*num_responses-1].cid2);
        //uint16_t lenid = responses[*num_responses-1].length & 0xFFF;
        //printf("  LENID:      %d\n", responses[*num_responses-1].length & 0xFFF);
        //printf("  INFO:       ");
        //for(size_t j = 0; j < lenid/2; j++)
        //{
        //    printf("%02X", responses[*num_responses-1].info[j]);
        //}
        //printf("\n");
        //printf("  CHKSUM:     %04X\n", responses[*num_responses-1].chksum);
    }

    return responses;

}

struct tb_command* query_bms(serial handle, struct tb_command* cmd, size_t* num_responses)
{
    char query_cmd[256] = {0};
    size_t size = 256;
    tb_write_command(cmd, query_cmd, &size);

    return query_bms_with_buffer(handle, query_cmd, strlen(query_cmd), num_responses);
}


#endif // WINDOWS_SETUP_H
