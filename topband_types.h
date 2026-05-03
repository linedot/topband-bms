#ifndef TOPBAND_TYPES_H
#define TOPBAND_TYPES_H

#include <stdint.h>

#define TB_LENBITS 12
#define TB_MAX_LEN (1 << TB_LENBITS) - 1


#define TB_MIN_MSG_SIZE 16 // SOI (1) + ADR(2) + CID1 (2) + CID2 (2) + LENGTH (4) + CHKSUM (4) + EOI (1)

#define TB_PROTOCOL_VERSION 0x21
#define TB_SOI '~'
#define TB_EOI '\r'

#define TB_SPECIAL_SOI (const char)(0xEF)
#define TB_SPECIAL_EOI (const char)(0x16)


// CID1
#define TB_CID1_BAT_DATA 0x46

// CID2

// commands
#define TB_CID2_GET_AVAL_FP         0x42
#define TB_CID2_GET_ALRM_INFO       0x44
#define TB_CID2_GET_SYSPARAM_FP     0x47
#define TB_CID2_GET_HISTORICAL_DATA 0x4B
#define TB_CID2_GET_DATE            0x4D
#define TB_CID2_SET_DATE            0x4E
#define TB_CID2_GET_PROTO_VER       0x4F
#define TB_CID2_GET_MANUF_INFO      0x51
//#define TB_CID2_GET_CHAMAN_INFO     0x92 // Not supported
//#define TB_CID2_GET_SN              0x93 // Not supported
//#define TB_CID2_SET_CHAMAN_INFO     0x94 // Not supported
//#define TB_CID2_SHUTDOWN            0x95 // Not supported
//#define TB_CID2_GET_FW_INFO         0x96 // Not supported

// responses
#define TB_CID2_R_OK          0x00
#define TB_CID2_R_VER_ERR     0x01
#define TB_CID2_R_CHKSUM_ERR  0x02
#define TB_CID2_R_LCHKSUM_ERR 0x03
#define TB_CID2_R_CID2_ERR    0x04
#define TB_CID2_R_FMT_ERR     0x05
#define TB_CID2_R_INFO_ERR    0x06
#define TB_CID2_R_ADR_ERR     0x90
#define TB_CID2_R_COM_ERR     0x91

struct tb_command
{
    uint8_t soi;
    uint8_t version;
    uint8_t adr;
    uint8_t cid1;
    uint8_t cid2;
    uint16_t length;
    uint8_t info[TB_MAX_LEN+1];
    uint16_t chksum;
    uint8_t eoi;
};

enum tb_historical_query_mode
{
    TB_FIRST  = 0x00,
    TB_NEXT   = 0x01,
    TB_RESEND = 0x02
};

struct tb_manufacturer_info
{
    char     hw[21];
    char     sw[5];
    char     id[41];
};


// I think this isn't actually used anywhere
enum tb_infoflags
{
    TB_IFLAG_SW_VALUE_CHANGE = 0x8,
    TB_IFLAG_ALRM_VALUE_CHANGE = 0x1
};

struct tb_analog_values_fixed_point
{
    //uint8_t  flags;
    //uint8_t  cmd;
    uint8_t  id;
    uint8_t  cell_count;
    uint16_t cell_voltages[16];
    uint8_t  temp_count;
    uint16_t cell_temps[16];
    uint16_t balancer_temp;
    uint16_t environment_temp;
    uint16_t mosfet_temp;
    int16_t  current;
    uint16_t module_voltage;
    uint16_t remaining_capacity;
    uint8_t  udi;
    uint16_t total_capacity;
    uint16_t cycle_count;
    uint32_t remaining_capacity2;
    uint32_t total_capacity2;
    uint8_t soc;
    uint8_t soh;
    uint8_t custom_values[2];
};

struct tb_system_parameter
{
    //uint8_t flags;
    uint16_t cell_high_v;
    uint16_t cell_low_v;
    uint16_t cell_under_v;
    uint16_t charge_high_t;
    uint16_t charge_low_t;
    int16_t charge_current_max;
    uint16_t module_high_v;
    uint16_t module_low_v;
    uint16_t module_under_v;
    uint16_t discharge_high_t;
    uint16_t discharge_low_t;
    int16_t discharge_current_max;
};

enum TB_ALARM_STATUS
{
    // STATUS 0
    TB_ALRMS_CELL_OVER_VOLTAGE_PROTECT       = 1,
    TB_ALRMS_CELL_UNDER_VOLTAGE              = 1ULL << 1,
    TB_ALRMS_CHARGE_OVER_CURRENT_PROTECT     = 1ULL << 2,
    TB_ALRMS_CELL_OVER_VOLTAGE_ALARM         = 1ULL << 3,
    TB_ALRMS_DISCHARGE_OVER_CURRENT1_PROTECT = 1ULL << 4,
    TB_ALRMS_CELL_TEMP_DISCHARGE_PROTECT     = 1ULL << 5,
    TB_ALRMS_CELL_TEMP_CHARGE_PROTECT        = 1ULL << 6,
    TB_ALRMS_MODULE_UNDER_VOLTAGE            = 1ULL << 7,
    // STATUS 1
    TB_ALRMS_OPEN_CURRENT_LIMIT              = 1ULL << 8,
    TB_ALRMS_CHARGE_MOSFET                   = 1ULL << 9,
    TB_ALRMS_DISCHARGE_MOSFET                = 1ULL << 10,
    TB_ALRMS_SHORT_CIRCUIT_PROTECT           = 1ULL << 11,
    TB_ALRMS_CELL_UNDER_VOLTAGE_PROTECT      = 1ULL << 12,
    TB_ALRMS_MODULE_UNDER_VOLTAGE_PROTECT    = 1ULL << 13,
    TB_ALRMS_REVERSE_PROTECT                 = 1ULL << 14,
    TB_ALRMS_SOC_LOW_ALARM                   = 1ULL << 15,
    // STATUS 2
    TB_ALRMS_BUZZER_ON                       = 1ULL << 16,
    TB_ALRMS_CHARGER_FAULT                   = 1ULL << 17,
    TB_ALRMS_2G_MODULE_FAILURE               = 1ULL << 18,
    TB_ALRMS_FULL_STATE                      = 1ULL << 19,
    TB_ALRMS_CHARGER_CONNECTED               = 1ULL << 20,
    TB_ALRMS_HEATER_ON                       = 1ULL << 21,
    TB_ALRMS_DISCHARGING                     = 1ULL << 22,
    TB_ALRMS_CHARGING                        = 1ULL << 23,
    // STATUS 3
    TB_ALRMS_UNDEFINED_3_0                   = 1ULL << 24,
    TB_ALRMS_UNDEFINED_3_1                   = 1ULL << 25,
    TB_ALRMS_UNDEFINED_3_2                   = 1ULL << 26,
    TB_ALRMS_UNDEFINED_3_3                   = 1ULL << 27,
    TB_ALRMS_CELL_VOLTAGE_LOW_FORCE_PROTECT  = 1ULL << 28,
    TB_ALRMS_DISCONNECTOR_STATE              = 1ULL << 29,
    TB_ALRMS_AEROSOL_TRIGGERED               = 1ULL << 30,
    TB_ALRMS_PRECHARGE_ON                    = 1ULL << 31,
    // STATUS 4 (completely missing in the language file)
    TB_ALRMS_UNDEFINED_4_0                   = 1ULL << 32,
    TB_ALRMS_UNDEFINED_4_1                   = 1ULL << 33,
    TB_ALRMS_UNDEFINED_4_2                   = 1ULL << 34,
    TB_ALRMS_UNDEFINED_4_3                   = 1ULL << 35,
    TB_ALRMS_UNDEFINED_4_4                   = 1ULL << 36,
    TB_ALRMS_UNDEFINED_4_5                   = 1ULL << 37,
    TB_ALRMS_UNDEFINED_4_6                   = 1ULL << 38,
    TB_ALRMS_UNDEFINED_4_7                   = 1ULL << 39,
    // STATUS 5
    TB_ALRMS_MODULE_OVER_VOLTAGE_ALARM       = 1ULL << 40,
    TB_ALRMS_MOS_NTC_TEMPERATURE_ALARM       = 1ULL << 41,
    TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_LOW_ALARM  = 1ULL << 42,
    TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_HIGH_ALARM = 1ULL << 43,
    TB_ALRMS_CELL_NTC_TEMPERATURE_LOW_ALARM  = 1ULL << 44,
    TB_ALRMS_CELL_NTC_TEMPERATURE_HIGH_ALARM = 1ULL << 45,
    TB_ALRMS_DISCHARGE_CURRENT_ALARM         = 1ULL << 46,
    TB_ALRMS_CHARGE_CURRENT_ALARM            = 1ULL << 47,
    // STATUS 6
    TB_ALRMS_BALANCE_NTC_TEMPERATURE_ALARM   = 1ULL << 48,
    TB_ALRMS_BALANCE_NTC_TEMPERATURE_PROTECT = 1ULL << 49,
    TB_ALRMS_DISCHARGE_MOSFET_FAULT          = 1ULL << 50,
    TB_ALRMS_CHARGE_MOSFET_FAULT             = 1ULL << 51,
    TB_ALRMS_CURRENT_SENSOR_FAULT            = 1ULL << 52,
    TB_ALRMS_AFE_FAULT                       = 1ULL << 53,
    TB_ALRMS_NTC_FAULT                       = 1ULL << 54,
    TB_ALRMS_CELL_FAULT                      = 1ULL << 55,
    // STATUS 7
    TB_ALRMS_DISCHARGE_OVER_CURRENT2_PROTECT = 1ULL << 56,
    TB_ALRMS_SMART_CHARGING                  = 1ULL << 57,
    TB_ALRMS_MODULE_OVER_VOLTAGE_PROTECT     = 1ULL << 58,
    TB_ALRMS_MOS_NTC_TEMPERATURE_PROTECT     = 1ULL << 59,
    TB_ALRMS_DISCHARGE_MOSFET_FORCED_CLOSE   = 1ULL << 60,
    TB_ALRMS_CHARGE_MOSFET_FORCED_CLOSE      = 1ULL << 61,
    TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_PROTECT_DISCHARGING = 1ULL << 62,
    TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_PROTECT_CHARGING    = 1ULL << 63,
};
// I have only ever seen up to 8 bytes of status,
// but these are defined in the language file
// STATUS 8
// enum TB_ALARM_STATUS8
// {
//     TB_ALRMS_REQUEST_SLEEP_DTU                     = 1ULL << 0,
//     TB_ALRMS_BATTERY_LOW_TEMPERATURE_PROTECT       = 1ULL << 1,
//     TB_ALRMS_BATTERY_HIGH_TEMPERATURE_PROTECT      = 1ULL << 2,
//     TB_ALRMS_LARGE_CELL_VOLTAGE_DIFFERENTIAL_ALARM = 1ULL << 3,
//     TB_ALRMS_MOSFET_HIGH_TEMPERATURE_ALARM         = 1ULL << 4,
//     TB_ALRMS_CELL_ULTRA_HIGH_TEMPERATURE_PROTECT   = 1ULL << 5,
//     TB_ALRMS_DISCHARGE_LIMIT_CURRENT_ON            = 1ULL << 6,
//     TB_ALRMS_SOC_LOW_PROTECTION                    = 1ULL << 7
// };
// STATUS 9
// enum TB_ALARM_STATUS9
// {
//     TB_ALRMS_CELL_LARGE_VOLTAGE_DIFF_PROTECT = 1ULL << 0,
//     TB_ALRMS_ANTI_THEFT_COMM_ENABLED         = 1ULL << 1,
//     TB_ALRMS_ANTI_THEFT_GYRO_ENABLED         = 1ULL << 2,
//     TB_ALRMS_ANTI_THEFT_GYRO_ACTIVE          = 1ULL << 3,
//     TB_ALRMS_ANTI_THEFT_GYRO_LOCKED          = 1ULL << 4,
//     TB_ALRMS_SOH_ABNORMAL_ALARM              = 1ULL << 5,
//     TB_ALRMS_GYRO_FAULT                      = 1ULL << 6,
//     TB_ALRMS_ABNORMAL_CURRRENT_LIMIT_PROTECT = 1ULL << 7
// };

struct tb_alarm_info
{
    uint8_t dataflag;
    uint8_t cmd;
    uint8_t cell_count;
    uint8_t cell_voltage_status[16];
    uint8_t temp_count;
    uint8_t cell_temp_status[16];
    uint8_t balancer_temp_status;
    uint8_t environment_temp_status;
    uint8_t mosfet_temp_status;
    uint8_t charge_current_status;
    uint8_t module_voltage_status;
    //uint8_t discharge_current_status;
    uint8_t status_count;
    uint64_t status;
};


struct tb_date
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
};

// ASCII command: ~2100464BC0040000FCC6
// ASCII reply:   ~210046000088 0000 07E70C150A1F04 61 06 000000000000 19 0000 12C0 079E 0BA5 0BA5 0BA5 0BA5 0B55 0B4B 0C80 0C80 0C80 0C80 0C80 0C80 0C81 0C80 0C80 0C80 0C80 0C80 0C7F 0C7F 0C80 0000 E0C5
//                    start->|   ?        ^        ^    ^     status   ?       ^    ^     ^----^----^---^----^-----^    ^----^----^----^----^----^----^----^----^----^----^----^----^----^----^  ^
//                                    date        event |                     V(P)  C(P)      temps (1 missing?)                               Cell voltages                                     |
//                                                     status count                                                                                                               might be cell 16 (this pack has only 15)


// ASCII command: ~2100464BC0040100FCC5
// ASCII reply:   ~210046000088 0001 07E70C150A1F0A 01 06 000000000000 19 0320 12C0 079E 0BA5 0BA5 0BA5 0BA5 0B55 0B4B 0C80 0C7F 0C80 0C80 0C81 0C80 0C81 0C80 0C80 0C80 0C80 0C80 0C7F 0C7F 0C80 0000 E0A2
//                                                                         ^
//                                                                        current (8A*100)
struct tb_historical_data
{
    uint8_t adr;
    uint8_t cmd;
    struct tb_date date;
    uint8_t event_type;
    uint8_t status_count;
    uint64_t status;
    int16_t  current;
    uint16_t pack_voltage;
    uint16_t remaining_capacity;
    uint16_t cell_temps[4];
    //uint16_t balancer_temp;    // Balancer temp is missing (it's there in the proprietary version of the history command)
    uint16_t environment_temp;
    uint16_t mosfet_temp;
    uint16_t cell_voltages[16];
};

#endif // TOPBAND_TYPES_H
