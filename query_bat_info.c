#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "topband.h"
#include "topband_print.h"

#if defined(__linux__)
#include "linux_setup.h"
#elif defined(__WIN32)
#include "windows_setup.h"
#else
#error Only Windows/Linux implemented currently
#endif


int main(int argc, char* argv[]) {

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    serial fd = open_serial(argv[1]);

    // Configure basic serial parameters
    configure_serial_port(fd, BAUD(9600));  // Set baud rate to 9600

    int bms_count = 0;
    int bms_ids[16] = {0};

    /**************************************
     *            WAKE UP BMS             *
     **************************************/
    printf("Waking up BMS\n");
    bms_count = 0;
    for (uint16_t bms_id = 0; bms_id < 16; bms_id++)
    {
        size_t num_responses = 0;
        struct tb_command protover_cmd = tb_cmd_get_protocol_version(bms_id);
        struct tb_command* responses = query_bms(fd, &protover_cmd, &num_responses);

        if (NULL != responses)
            free(responses);
    }

    /**************************************
     *          CHECK BMS ONLINE          *
     **************************************/
    printf("Scanning BMS\n");
    bms_count = 0;
    for (uint16_t bms_id = 0; bms_id < 16; bms_id++)
    {
        size_t num_responses = 0;
        struct tb_command protover_cmd = tb_cmd_get_protocol_version(bms_id);
        struct tb_command* responses = query_bms(fd, &protover_cmd, &num_responses);

        if (num_responses != 0)
        {
            printf("BMS %d responded\n", bms_id);
            bms_ids[bms_count] = bms_id;
            bms_count++;
        }
        if (NULL != responses)
            free(responses);
    }

    /**************************************
     *         QUERY MANUFACTURER         *
     **************************************/
    printf("Querying manufacturer\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        size_t num_responses = 0;
        struct tb_command minfo_cmd = tb_cmd_get_manufacturer_info(bms_ids[i]);
        struct tb_command* responses = query_bms(fd, &minfo_cmd, &num_responses);

        for (size_t j = 0; j < num_responses; j++)
        {
            struct tb_manufacturer_info minfo;
            if (0 == tb_interpret_manufacturer_info(&responses[j], &minfo))
            {
                printf("Manufacturer info (BMS ID %d):\n", bms_ids[i]);
                tb_print_manufacturer_info(&minfo);
            }
        }
        if (NULL != responses)
            free(responses);
    }
    /**************************************
     *               SET DATE             *
     **************************************/
    printf("Setting date\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        size_t num_responses = 0;
        struct tb_command date_cmd = tb_cmd_set_date(bms_ids[i]);
        time_t t = time(NULL);
        struct tm* local_time = localtime(&t);
        tb_set_date_cmd_set_date(&date_cmd,
                local_time->tm_year+1900,
                local_time->tm_mon+1,
                local_time->tm_mday,
                local_time->tm_hour,
                local_time->tm_min,
                local_time->tm_sec);
        printf(" System date: %02d-%02d-%02d %02d:%02d:%02d\n",
                local_time->tm_year+1900,
                local_time->tm_mon+1,
                local_time->tm_mday,
                local_time->tm_hour,
                local_time->tm_min,
                local_time->tm_sec);
        struct tb_command* responses = query_bms(fd, &date_cmd, &num_responses);
        for (size_t j = 0; j < num_responses; j++)
        {
            printf("BMS ID %d return code: %02X\n",bms_ids[i], responses[j].cid2);
        }
        if (NULL != responses)
            free(responses);
    }

    /**************************************
     *             QUERY DATE             *
     **************************************/
    printf("Querying date\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        size_t num_responses = 0;
        struct tb_command date_cmd = tb_cmd_get_date(bms_ids[i]);
        struct tb_command* responses = query_bms(fd, &date_cmd, &num_responses);
        for (size_t j = 0; j < num_responses; j++)
        {
            struct tb_date date;
            if (0 == tb_interpret_date(&responses[j], &date))
            {
                printf("BMS date (BMS ID %d):\n", bms_ids[i]);
                tb_print_date(&date);
            }
        }
        if (NULL != responses)
            free(responses);
    }

    /**************************************
     *          QUERY BAT. VALUES         *
     **************************************/
    printf("Querying battery values\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        size_t num_responses = 0;
        struct tb_command avfp_cmd = tb_cmd_get_bat_values(bms_ids[i]);
        struct tb_command* responses = query_bms(fd, &avfp_cmd, &num_responses);

        for (size_t j = 0; j < num_responses; j++)
        {
            struct tb_analog_values_fixed_point avfp;
            if (0 == tb_interpret_analog_values_fixed_point(&responses[j], &avfp))
            {
                printf("Battery values (BMS ID %d):\n", bms_ids[i]);
                tb_print_analog_values_fixed_point(&avfp);
            }
        }
        if (NULL != responses)
            free(responses);
    }

    /**************************************
     *          QUERY SYS. PARAMS         *
     **************************************/
    printf("Querying system parameters\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        size_t num_responses = 0;
        struct tb_command sp_cmd = tb_cmd_get_system_parameter(bms_ids[i]);
        struct tb_command* responses = query_bms(fd, &sp_cmd, &num_responses);

        for (size_t j = 0; j < num_responses; j++)
        {
            struct tb_system_parameter sp;
            if (0 == tb_interpret_system_parameter(&responses[j], &sp))
            {
                printf("System parameters (BMS ID %d):\n", bms_ids[i]);
                tb_print_system_parameter(&sp);
            }
        }
        if (NULL != responses)
            free(responses);
    }

    /**************************************
     *          QUERY ALARM INFO          *
     **************************************/
    printf("Querying ALARM info\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        size_t num_responses = 0;
        struct tb_command ainfo_cmd = tb_cmd_get_alarm_info(bms_ids[i]);
        struct tb_command* responses = query_bms(fd, &ainfo_cmd, &num_responses);

        for (size_t j = 0; j < num_responses; j++)
        {
            struct tb_alarm_info ainfo;
            if (0 == tb_interpret_alarm_info(&responses[j], &ainfo))
            {
                printf("Alarm info (BMS ID %d):\n", bms_ids[i]);
                tb_print_alarm_info(&ainfo);
            }
        }
        if (NULL != responses)
            free(responses);
    }
    /**************************************
     *        QUERY HISTORICAL DATA       *
     **************************************/
    printf("Querying historical data (first 10 events)\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        for (uint16_t hist_num = 0; hist_num < 10; hist_num++)
        {
            size_t num_responses = 0;
            struct tb_command hist_cmd = tb_cmd_get_historical_data(bms_ids[i]);
            if(hist_num > 0)
            {
                tb_historical_data_cmd_set_mode(&hist_cmd, TB_NEXT);
            }
            struct tb_command* responses = query_bms(fd, &hist_cmd, &num_responses);

            for (size_t j = 0; j < num_responses; j++)
            {
                struct tb_historical_data hist;
                if (0 == tb_interpret_historical_data(&responses[j], &hist))
                {
                    printf("Hist. Event nr. %d (BMS ID %d):\n", hist_num, bms_ids[i]);
                    tb_print_historical_data(&hist);
                }
            }
            if (NULL != responses)
                free(responses);
        }
    }


    /**************************************
     *            SHUTDOWN BMS            *
     **************************************/
    printf("Shutting down BMS\n");
    for (uint16_t i = 0; i < bms_count; i++)
    {
        printf("Shutting down BMS %d\n", bms_ids[i]);
        size_t num_responses = 0;
        uint8_t shutdown_cmd[8];
        memset(shutdown_cmd, 0, 8);
        tb_special_cmd_sleep(bms_ids[i], shutdown_cmd, 8);
        struct tb_command* responses = query_bms_with_buffer(fd, (const char*)shutdown_cmd, 8, &num_responses);

        if (NULL != responses)
            free(responses);
    }

    
    printf("Done, closing serial connection\n");
    close_serial(fd);
    printf("Exiting\n");
    return EXIT_SUCCESS;
}
