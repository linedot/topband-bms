#ifndef TOPBAND_H
#define TOPBAND_H

// The batteries use a protocol similar to Pylontech RS485 low-voltage protocol
// (there are however some differences and some unsupported commands)
// They also use a separate, proprietary protocol that seems to start with 0xEF and end with 0x16

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>

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
#define TB_CID2_GET_AVAL_FP     0x42
#define TB_CID2_GET_ALRM_INFO   0x44
#define TB_CID2_GET_SYSPARAM_FP 0x47
#define TB_CID2_GET_DATE        0x4D
#define TB_CID2_SET_DATE        0x4E
#define TB_CID2_GET_PROTO_VER   0x4F
#define TB_CID2_GET_MANUF_INFO  0x51
//#define TB_CID2_GET_CHAMAN_INFO 0x92 // Not supported
//#define TB_CID2_GET_SN          0x93 // Not supported
//#define TB_CID2_SET_CHAMAN_INFO 0x94 // Not supported
//#define TB_CID2_SHUTDOWN        0x95 // Not supported
//#define TB_CID2_GET_FW_INFO     0x96 // Not supported

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

int tb_write_command(struct tb_command* cmd, char* out, size_t* size)
{
    if (NULL == cmd || NULL == size)
    {
        return -1;
    }
    size_t lenid = cmd->length & 0xFFF;
    size_t msg_size = 2 + // soi and eoi as byte
           8*2          + // rest as ASCII
           lenid        + // lenid is number of bytes after transformation to ASCII
           1;             // \0

    *size = msg_size;
    if (NULL == out)
    {
        return 0;
    }
    if (msg_size > *size)
    {
        return -1;
    }
    // We don't care for the \0, but compiler will complain if we write 13 bytes
    snprintf(out, 14, "%c%02X%02X%02X%02X%04X",
             cmd->soi,
             cmd->version,
             cmd->adr,
             cmd->cid1,
             cmd->cid2,
             cmd->length);
    for(size_t i = 0; i < lenid/2; i++)
    {
        char* ptr = out+13+i*2;
        uint8_t byte = cmd->info[i];
        ptr[0] = "0123456789ABCDEF"[byte >> 4]; // hi
        ptr[1] = "0123456789ABCDEF"[byte & 0xF];// lo
    }

    // CHKSUM
    // 1) Add all ascii values (except SOI, EOI, CHKSUM)
    // 2) modulo 65536
    // 3) bitwise invert
    // 4) add 1
    uint16_t sum = 0;
    for (size_t i = 0; i < (lenid+12); i++)
    {
        sum += (uint32_t)out[1+i];
    }
    cmd->chksum = (~(sum & 0xFFFF) + 1) & 0xFFFF;

    snprintf(out+13+lenid, 5+1, "%04X%c",
             cmd->chksum,
             cmd->eoi);
    return 0;
}

void tb_write_lchksum(struct tb_command* cmd)
{
    // Add the 3 nibbles of lenid, module 16, bit-invert, add 1
    // write to the high 4 bits of length
    uint16_t lenid = (cmd->length & 0xFFF);
    uint16_t sum = (lenid >> 8) + ((lenid >> 4) & 0xF) + (lenid & 0xF);
    cmd->length |= ((~(sum & 0xF) + 1) & 0xF) << 12;
}

int tb_check_lchksum(struct tb_command* cmd)
{
    uint16_t lchksum = (cmd->length & 0xF000) >> 12;
    uint16_t lenid  = (cmd->length & 0xFFF);
    uint16_t sum = (lenid >> 8) + ((lenid >> 4) & 0xF) + (lenid & 0xF);
    uint16_t expected = (~(sum & 0xF) + 1) & 0xF;

    if (lchksum == expected)
    {
        return 0;
    }

    return -1;
}

void tb_set_cmd_common(struct tb_command* cmd)
{
    cmd->soi = TB_SOI;
    cmd->version = TB_PROTOCOL_VERSION;
    cmd->eoi = TB_EOI;
}

struct tb_command tb_cmd_get_bat_values(int bms_id)
{
    struct tb_command cmd;
    tb_set_cmd_common(&cmd);

    cmd.adr     = (uint8_t)bms_id;
    cmd.cid1    = TB_CID1_BAT_DATA;
    cmd.cid2    = TB_CID2_GET_AVAL_FP;
    cmd.length  = 2;
    cmd.info[0] = (uint8_t)bms_id;
    tb_write_lchksum(&cmd);

    return cmd;
}

struct tb_command tb_cmd_get_protocol_version(int bms_id)
{
    struct tb_command cmd;
    tb_set_cmd_common(&cmd);
    cmd.adr = (uint8_t)bms_id;
    cmd.cid1 = TB_CID1_BAT_DATA;
    cmd.cid2 = TB_CID2_GET_PROTO_VER;
    cmd.length = 0;

    tb_write_lchksum(&cmd);

    return cmd;
}

struct tb_command tb_cmd_get_manufacturer_info(int bms_id)
{
    struct tb_command cmd;
    tb_set_cmd_common(&cmd);
    cmd.adr = (uint8_t)bms_id;
    cmd.cid1 = TB_CID1_BAT_DATA;
    cmd.cid2 = TB_CID2_GET_MANUF_INFO;
    cmd.length = 0;

    tb_write_lchksum(&cmd);

    return cmd;
}

struct tb_command tb_cmd_get_date(int bms_id)
{
    struct tb_command cmd;
    tb_set_cmd_common(&cmd);
    cmd.adr = (uint8_t)bms_id;
    cmd.cid1 = TB_CID1_BAT_DATA;
    cmd.cid2 = TB_CID2_GET_DATE;
    cmd.length = 0;

    tb_write_lchksum(&cmd);

    return cmd;
}

struct tb_command tb_cmd_get_system_parameter(int bms_id)
{
    struct tb_command cmd;
    tb_set_cmd_common(&cmd);
    cmd.adr = (uint8_t)bms_id;
    cmd.cid1 = TB_CID1_BAT_DATA;
    cmd.cid2 = TB_CID2_GET_SYSPARAM_FP;
    cmd.length = 0;

    tb_write_lchksum(&cmd);

    return cmd;
}

struct tb_command tb_cmd_get_alarm_info(int bms_id)
{
    struct tb_command cmd;
    tb_set_cmd_common(&cmd);
    cmd.adr = (uint8_t)bms_id;
    cmd.cid1 = TB_CID1_BAT_DATA;
    cmd.cid2 = TB_CID2_GET_ALRM_INFO;
    cmd.length  = 2;
    cmd.info[0] = (uint8_t)bms_id;

    tb_write_lchksum(&cmd);

    return cmd;
}

int tb_special_cmd_sleep(int bms_id, uint8_t* out, size_t size)
{
    if(size < 7)
    {
        return -1;
    }

    out[0] = 0xEF;
    out[1] = bms_id & 0xFF;
    out[2] = 0xA1;
    out[3] = 0x01;
    out[4] = 0xF0;
    out[5] = 0xFE;
    // Byte 6 is probably the checksum. Not sure how it's calculated, but
    // the following seems to hold for this one command at least
    out[6] = 0x6E - (bms_id & 0xFF);
    out[7] = 0x16;

    return 0;
}


int _tb_get_nibble(const char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10; // not sure if necessary, I've only seen capital A-F
    }
    return -1; // broken protocol (happens with manufacturer_info)
}
int _tb_get_byte(const char* data)
{
    int nibble1 = _tb_get_nibble(data[0]);
    int nibble2 = _tb_get_nibble(data[1]);
    if ((-1 == nibble1) || (-1 == nibble2))
    {
        return -1;
    }
    return (nibble1<<4) | nibble2;
}

int tb_decode(const char* data, size_t size, struct tb_command* cmd, const char** next)
{
    if(NULL == cmd || NULL == data || size < TB_MIN_MSG_SIZE)
    {
        return -1;
    }
    size_t soi_pos = 0;
    for (; soi_pos < size; soi_pos++)
    {
        if ((data[soi_pos] == TB_SOI) || (data[soi_pos] == TB_SPECIAL_SOI))
            break;
    }
    if (soi_pos+TB_MIN_MSG_SIZE > size)
    {
        return -2;
    }
    if(data[soi_pos] == TB_SPECIAL_SOI)
    {
        // Don't know how to decode proprietary commands/responses
        return -7;
    }
    cmd->soi     = data[soi_pos];
    cmd->version = _tb_get_byte(data+soi_pos+1);
    cmd->adr     = _tb_get_byte(data+soi_pos+3);
    cmd->cid1    = _tb_get_byte(data+soi_pos+5);
    cmd->cid2    = _tb_get_byte(data+soi_pos+7);
    cmd->length  = (_tb_get_byte(data+soi_pos+9)<<8) + _tb_get_byte(data+soi_pos+11);

    if (0 != tb_check_lchksum(cmd))
    {
        return -3;
    }
    size_t lenid = (cmd->length & 0xFFF);

    if (size < soi_pos+TB_MIN_MSG_SIZE+lenid)
    {
        return -4;
    }


    int break_protocol = 0;
    for (uint16_t pos = 0; pos < lenid/2; pos++)
    {
        int byte = _tb_get_byte(data+soi_pos+13+pos*2);
        if (byte == -1)
        {
            break_protocol = 1;
            break;
        }
        cmd->info[pos] = (uint8_t)byte;
    }
    // The batteries break protocol on manufacturer info and transmit it in plain ascii
    if (break_protocol)
    {
        for (uint16_t pos = 0; pos < lenid; pos++)
        {
            cmd->info[pos] = data[soi_pos+13+pos];
        }
    }
    cmd->chksum = (_tb_get_byte(data+soi_pos+13+lenid) << 8) + _tb_get_byte(data+soi_pos+13+lenid+2);

    // check chksum
    uint16_t sum = 0;
    for (size_t chk_pos = soi_pos+1; chk_pos < soi_pos+13+lenid; chk_pos++)
    {
        sum += (uint32_t)data[chk_pos];
    }
    uint16_t expected = (~(sum & 0xFFFF) + 1) & 0xFFFF;

    if (cmd->chksum != expected)
    {
        return -5;
    }

    cmd->eoi = data[soi_pos+13+lenid+4];

    // I'm getting \n every time. Not sure if this is valid or something in my setup is messed up
    if (cmd->eoi != TB_EOI && cmd->eoi != '\n')
    {
        return -6;
    }

    *next = data+soi_pos+13+lenid+5;

    return 0;
}

int tb_is_valid_command(struct tb_command* cmd)
{
    if ( cmd->soi == TB_SOI &&
        cmd->cid1 == TB_CID1_BAT_DATA &&
        (cmd->eoi == '\r' || cmd->eoi == '\n'))
        return 1;
    return 0;
}

struct tb_manufacturer_info
{
    char     hw[21];
    char     sw[5];
    char     id[41];
};


int tb_interpret_manufacturer_info(struct tb_command* response, struct tb_manufacturer_info* manufacturer_info)
{
    if (!tb_is_valid_command(response))
        return -1;


    // Bad return code
    if (response->cid2 != 0)
    {
        return -response->cid2;
    }

    if ((response->length & 0xFFF) != 0x40)
        return -2;


    memset(manufacturer_info, 0, sizeof(struct tb_manufacturer_info));

    memcpy(manufacturer_info->hw, response->info, 20);
    memcpy(manufacturer_info->sw, response->info+20, 4);
    memcpy(manufacturer_info->id, response->info+24, 40);

    // I don't really know what I'm doing here, but this
    // results in almost the same string as in the proprietary
    // software
    //for (int i = 0; i < 7; i++)
    //{
    //    snprintf(manufacturer_info->hw+i*2, 3, "%02X", response->info[i]);
    //}
    //manufacturer_info->hw[0] = 'S';
    //manufacturer_info->hw[1] = 'T';
    //manufacturer_info->hw[2] = 'M';
    //manufacturer_info->hw[7] = '_';
    //manufacturer_info->hw[8] = 'T';

    //manufacturer_info->sw = response->info[11];
    //for (int i = 0; i < 10; i++)
    //{
    //    uint8_t byte = response->info[i+12];
    //    if (byte == 0xEF)
    //    {
    //        break;
    //    }
    //    snprintf(manufacturer_info->id+i*2, 3, "%02X", byte);
    //}

    return 0;
}


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

int tb_interpret_analog_values_fixed_point(struct tb_command* response, struct tb_analog_values_fixed_point* avfp)
{
    if (!tb_is_valid_command(response))
        return -1;

    // Bad return code
    if (response->cid2 != 0)
    {
        return -response->cid2;
    }

    // at least 1 cell and temp + the rest
    if ((response->length & 0xFFF) < 42)
        return -2;


    memset(avfp, 0, sizeof(struct tb_analog_values_fixed_point));

    // This seems to be ADR instead of flags
    avfp->id      = response->info[0];
    //avfp->flags      = response->info[0];
    //avfp->cmd        = response->info[1]; // This seems to differ from Pylontech
    avfp->cell_count = response->info[1];
    for (uint16_t i = 0; i < avfp->cell_count; i++)
    {
        avfp->cell_voltages[i] = (response->info[2+i*2] << 8) | (response->info[2+i*2+1]);
    }
    uint16_t offset = 2+2*avfp->cell_count;
    avfp->temp_count = response->info[offset];
    offset += 1;
    // last 3 temps are balancer, environment, mosfet
    for (uint16_t i = 0; i < avfp->temp_count-3; i++)
    {
        avfp->cell_temps[i]  = (response->info[offset+i*2] << 8) + (response->info[offset+i*2+1]);
    }
    offset += (avfp->temp_count-3)*2;
    avfp->balancer_temp      = (response->info[offset+0] << 8) | (response->info[offset+1]);
    avfp->environment_temp   = (response->info[offset+2] << 8) | (response->info[offset+3]);
    avfp->mosfet_temp        = (response->info[offset+4] << 8) | (response->info[offset+5]);
    offset += 6;
    avfp->current            = (response->info[offset] << 8) | (response->info[offset+1]);
    avfp->module_voltage     = (response->info[offset+2] << 8) | (response->info[offset+3]);
    avfp->remaining_capacity = (response->info[offset+4] << 8) | (response->info[offset+5]);
    avfp->udi                = response->info[offset+6];
    avfp->total_capacity     = (response->info[offset+7] << 8) | (response->info[offset+8]);
    avfp->cycle_count        = (response->info[offset+9] << 8) | (response->info[offset+10]);
    offset += 11;
    avfp->soc = response->info[offset];
    avfp->soh = response->info[offset+1];
    avfp->custom_values[0] = response->info[offset+2];
    avfp->custom_values[1] = response->info[offset+3];
    return 0;
}


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

int tb_interpret_system_parameter(struct tb_command* response, struct tb_system_parameter* sp)
{
    if (!tb_is_valid_command(response))
        return -1;


    // Bad return code
    if (response->cid2 != 0)
    {
        return -response->cid2;
    }

    // at least 1 cell and temp + the rest
    if ((response->length & 0xFFF) < 48)
    {
        return -2;
    }

    memset(sp, 0, sizeof(struct tb_system_parameter));

    // There doesn't seem to be a flags byte
    //sp->flags                 = response->info[0];
    sp->cell_high_v           = (response->info[0]  << 8) | (response->info[1]);
    sp->cell_low_v            = (response->info[2]  << 8) | (response->info[3]);
    sp->cell_under_v          = (response->info[4]  << 8) | (response->info[5]);
    sp->charge_high_t         = (response->info[6]  << 8) | (response->info[7]);
    sp->charge_low_t          = (response->info[8]  << 8) | (response->info[9]);
    sp->charge_current_max    = (response->info[10] << 8) | (response->info[11]);
    sp->module_high_v         = (response->info[12] << 8) | (response->info[13]);
    sp->module_low_v          = (response->info[14] << 8) | (response->info[15]);
    sp->module_under_v        = (response->info[16] << 8) | (response->info[17]);
    sp->discharge_high_t      = (response->info[18] << 8) | (response->info[19]);
    sp->discharge_low_t       = (response->info[20] << 8) | (response->info[21]);
    sp->discharge_current_max = (response->info[22] << 8) | (response->info[23]);
    return 0;
}

enum ALARM_STATUS
{
    // STATUS 1
    TB_ALRMS_MODULE_OVER_VOLTAGE        = 1,
    TB_ALRMS_CELL_OVER_VOLTAGE          = 1ULL << 1,
    TB_ALRMS_CHARGE_OVER_CURRENT        = 1ULL << 2,
    TB_ALRMS_DISCHARGE_OVER_CURRENT     = 1ULL << 4,
    TB_ALRMS_DISCHARGE_OVER_TEMPERATURE = 1ULL << 5,
    TB_ALRMS_CHARGE_OVER_TEMPERATURE    = 1ULL << 6,
    TB_ALRMS_MODULE_UNDER_VOLTAGE       = 1ULL << 7,
    // STATUS 2
    // bit 8 documentation: Pre MOSFET（reserve, function not using）
    TB_ALRMS_CHARGE_MOSFET          = 1ULL << 9,
    TB_ALRMS_DISCHARGE_MOSFET       = 1ULL << 10,
    TB_ALRMS_USING_BAT_MODULE_POWER = 1ULL << 11,
    // STATUS 3
    TB_ALRMS_BUZZER                        = 1ULL << 16,
    TB_ALRMS_FULLY_CHARGED                 = 1ULL << 19,
    TB_ALRMS_HEATER                        = 1ULL << 21,
    TB_ALRMS_DISCHARGE_CURRENT_UNDER_100MA = 1ULL << 22,
    TB_ALRMS_CHARGE_CURRENT_UNDER_100MA    = 1ULL << 23,
    // STATUS 4
    TB_ALRMS_CELL1_VOLTAGE_ERR  = 1ULL << 24,
    TB_ALRMS_CELL2_VOLTAGE_ERR  = 1ULL << 25,
    TB_ALRMS_CELL3_VOLTAGE_ERR  = 1ULL << 26,
    TB_ALRMS_CELL4_VOLTAGE_ERR  = 1ULL << 27,
    TB_ALRMS_CELL5_VOLTAGE_ERR  = 1ULL << 28,
    TB_ALRMS_CELL6_VOLTAGE_ERR  = 1ULL << 29,
    TB_ALRMS_CELL7_VOLTAGE_ERR  = 1ULL << 30,
    TB_ALRMS_CELL8_VOLTAGE_ERR  = 1ULL << 31,
    // STATUS 5
    TB_ALRMS_CELL9_VOLTAGE_ERR  = 1ULL << 32,
    TB_ALRMS_CELL10_VOLTAGE_ERR = 1ULL << 33,
    TB_ALRMS_CELL11_VOLTAGE_ERR = 1ULL << 34,
    TB_ALRMS_CELL12_VOLTAGE_ERR = 1ULL << 35,
    TB_ALRMS_CELL13_VOLTAGE_ERR = 1ULL << 36,
    TB_ALRMS_CELL14_VOLTAGE_ERR = 1ULL << 37,
    TB_ALRMS_CELL15_VOLTAGE_ERR = 1ULL << 38,
    TB_ALRMS_CELL16_VOLTAGE_ERR = 1ULL << 39,
};

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

int tb_interpret_alarm_info(struct tb_command* response, struct tb_alarm_info* ainfo)
{
    if (!tb_is_valid_command(response))
        return -1;

    // Bad return code
    if (response->cid2 != 0)
    {
        return -response->cid2;
    }

    // at least 1 cell and temp + the rest
    if ((response->length & 0xFFF) < 42)
        return -2;


    memset(ainfo, 0, sizeof(struct tb_alarm_info));

    uint16_t offset = 1;
    ainfo->cell_count = response->info[offset];
    for (uint16_t i = 0; i < ainfo->cell_count; i++)
    {
        ainfo->cell_voltage_status[i] = response->info[offset+1+i];
    }
    offset += 1+ainfo->cell_count;
    ainfo->temp_count = response->info[offset];
    offset += 1;
    for (uint16_t i = 0; i < ainfo->temp_count-3; i++)
    {
        ainfo->cell_temp_status[i] = response->info[offset+i];
    }
    offset += (ainfo->temp_count -3);
    ainfo->balancer_temp_status = response->info[offset];
    ainfo->environment_temp_status = response->info[offset+1];
    ainfo->mosfet_temp_status = response->info[offset+2];
    offset += 3;
    ainfo->charge_current_status = response->info[offset];
    ainfo->module_voltage_status = response->info[offset+1];
    // I think this byte is the number of statuses
    //ainfo->discharge_current_status = response->info[offset+2];
    ainfo->status_count = response->info[offset+2];
    offset += 3;
    ainfo->status = 0;
    for (uint16_t i = 0; i < ainfo->status_count; i++)
    {
        ainfo->status |= (((uint64_t)response->info[offset+i]) << 8*i);
    }
    return 0;
}


struct tb_date
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
};

int tb_interpret_date(struct tb_command* response, struct tb_date* date)
{
    if (!tb_is_valid_command(response))
        return -1;

    // Bad return code
    if (response->cid2 != 0)
    {
        return -response->cid2;
    }

    // 2 bytes (year) + 5 bytes (month,day,hour,minute,second)
    if ((response->length & 0xFFF) < 14)
        return -2;


    memset(date, 0, sizeof(struct tb_date));

    date->year   = (response->info[0] << 8) | (response->info[1]);
    date->month  =  response->info[2];
    date->day    =  response->info[3];
    date->hour   =  response->info[4];
    date->minute =  response->info[5];
    date->second =  response->info[6];

    return 0;
}


#endif // TOPBAND_H
