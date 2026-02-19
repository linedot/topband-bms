#ifndef LINUX_SETUP_H
#define LINUX_SETUP_H

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#define SERIAL_DEVICE "/dev/ttyUSB0"  // Replace with your serial device

#include "topband.h"

void configure_serial_port(int fd, speed_t baudrate) {
    struct termios tio;
    tcgetattr(fd, &tio);

    // Set basic parameters
    cfsetispeed(&tio, baudrate);
    cfsetospeed(&tio, baudrate);

    // Set character size (8 bits)
    tio.c_cflag &= ~CSIZE; // Clear the character size bits
    tio.c_cflag |= CS8;     // Set 8 data bits

    // Set stop bits (1 stop bit)
    tio.c_cflag &= ~CSTOPB; // Clear the stop bit selection

    // Set parity (none)
    tio.c_cflag &= ~PARENB;  // Disable parity

    // Set hardware flow control
    tio.c_cflag &= ~CRTSCTS;  // Disable hardware flow control

    // Set local mode and enable receiver
    tio.c_cflag |= (CLOCAL | CREAD);

    // Set raw input/output modes
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Non-canonical mode
    tio.c_oflag &= ~OPOST; // Raw output

    // Set time and read interval
    tio.c_cc[VMIN] = 1; // Minimum number of characters to read
    tio.c_cc[VTIME] = 0; // Timeout in deciseconds (0 for no timeout)

    // Apply the configuration
    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        printf("Error setting serial attributes: %d\n", errno);
        return;
    }
}

struct tb_command* query_bms_with_buffer(int fd, const char* query_buffer,
        size_t query_buffer_size, size_t* num_responses)
{
    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(fd, &read_fds);



    struct tb_command* responses = NULL;
    *num_responses = 0;
    
    int bytes_sent = write(fd, query_buffer, query_buffer_size);
    if (bytes_sent != query_buffer_size) {
        perror("Failed to send query_cmd");
    }
    //else {
    //    printf("Sent %d bytes: %s\n", bytes_sent, query_buffer);
    //}


    const size_t max_buf_size = 256*16;

    char buffer[max_buf_size];
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;

    int read_something = 1;

    int msg_size = 0;

    memset(buffer, 0, max_buf_size);
    while (read_something)
    {
        read_something = 0;
        if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout) == 1)
        {
            int bytes_read = read(fd, buffer+msg_size, sizeof(buffer)-msg_size);
            if (bytes_read > 0) {
                read_something = 1;
                msg_size += bytes_read;
            } else if (bytes_read == -1) {
                perror("Failed to receive data");
            }
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
        printf("response:\n");
        printf("  VER:        %02X\n", responses[*num_responses-1].version);
        printf("  ADR:        %02X\n", responses[*num_responses-1].adr);
        printf("  CID1:       %02X\n", responses[*num_responses-1].cid1);
        printf("  CID2 (RTN): %02X\n", responses[*num_responses-1].cid2);
        uint16_t lenid = responses[*num_responses-1].length & 0xFFF;
        printf("  LENID:      %d\n", responses[*num_responses-1].length & 0xFFF);
        printf("  INFO:       ");
        for(size_t j = 0; j < lenid/2; j++)
        {
            printf("%02X", responses[*num_responses-1].info[j]);
        }
        printf("\n");
        printf("  CHKSUM:     %04X\n", responses[*num_responses-1].chksum);
    }

    return responses;

}

struct tb_command* query_bms(int fd, struct tb_command* cmd, size_t* num_responses)
{
    char query_cmd[256] = {0};
    size_t size = 256;
    tb_write_command(cmd, query_cmd, &size);

    return query_bms_with_buffer(fd, query_cmd, strlen(query_cmd), num_responses);
}


int kbhit(void) {
    struct termios oldt, newt;
    int oldf;
    int ch;

    // Get the terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new settings
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0); // Get current flags
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // Set non-blocking mode

    // Check if a character is available to read
    ch = getchar();
    
    // Restore original settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf); // Restore old flags

    if(ch != EOF) {
        ungetc(ch, stdin); // Put the character back if read
        return 1; // A key was pressed
    }

    return 0; // No key was pressed
}

#endif // LINUX_SETUP_H
