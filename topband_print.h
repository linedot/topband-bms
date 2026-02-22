#ifndef TOPBAND_PRINT_H
#define TOPBAND_PRINT_H

#include "topband.h"


void tb_print_manufacturer_info(struct tb_manufacturer_info* minfo)
{
    printf("  HW: %s\n", minfo->hw);
    printf("  SW: %s\n", minfo->sw);
    printf("  ID: %s\n", minfo->id);
}

void tb_print_analog_values_fixed_point(struct tb_analog_values_fixed_point* avfp)
{
    //printf("  Flags:          ");
    //if(avfp->flags & TB_IFLAG_SW_VALUE_CHANGE)
    //{
    //    printf("SW ");
    //}
    //if(avfp->flags & TB_IFLAG_ALRM_VALUE_CHANGE)
    //{
    //    printf("ALRM ");
    //}
    //printf("\n");
    //printf("  CMD:            %02X\n",avfp->cmd);
    printf("  ID:             %02X\n",avfp->id);
    printf("  Cell count:     %d\n",avfp->cell_count);
    printf("  Cell voltages:  ");
    for(uint16_t k = 0; k < avfp->cell_count; k++)
    {
        printf("%d:%.2fV",k+1,((float)avfp->cell_voltages[k])/1000.0);
        if(k != avfp->cell_count-1)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  Temp. count:    %d\n",avfp->temp_count);
    printf("  Cell temps.:    ");
    for(uint16_t k = 0; k < avfp->temp_count-3; k++)
    {
        printf("(%d-%d):%.2fC", k*4+1, k*4+4, ((float)avfp->cell_temps[k]-2731)/10.0);
        if(k != avfp->temp_count-4)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  Balancer temp:  %.2fC\n", ((float)avfp->balancer_temp-2731)/10.0);
    printf("  Env. temp:      %.2fC\n", ((float)avfp->environment_temp-2731)/10.0);
    printf("  MOSFET temp:    %.2fC\n", ((float)avfp->mosfet_temp-2731)/10.0);
    printf("  Current:        %dmA\n",avfp->current*10);
    printf("  Module voltage: %.2fV\n",((float)avfp->module_voltage)/100.0);
    if(avfp->udi == 4 && avfp->remaining_capacity == 0xFFFF && avfp->total_capacity == 0xFFFF)
    {
        printf("  Capacity2:      %d/%dmAh\n",avfp->remaining_capacity2 * 10, avfp->total_capacity2 * 10);
    }
    else
    {
        printf("  Capacity1:      %d/%dmAh\n",avfp->remaining_capacity * 10, avfp->total_capacity * 10);
    }
    printf("  Cycle count:    %d\n",avfp->cycle_count);
    printf("  SOC:            %d%%\n",avfp->soc);
    printf("  SOH:            %d%%\n",avfp->soh);
    printf("  Custom Value 1: 0x%02X\n",avfp->custom_values[0]);
    printf("  Custom Value 2: 0x%02X\n",avfp->custom_values[1]);
}

void tb_print_system_parameter(struct tb_system_parameter* sp)
{
    printf("  Cell high voltage:     %.2fV\n", ((float)sp->cell_high_v) / 1000.0f);
    printf("  Cell low voltage:      %.2fV\n", ((float)sp->cell_low_v) / 1000.0f);
    printf("  Cell under voltage:    %.2fV\n", ((float)sp->cell_under_v) / 1000.0f);

    printf("  Charge high temp:      %.2f°C\n", ((float)sp->charge_high_t - 2731) / 10.0f);
    printf("  Charge low temp:       %.2f°C\n", ((float)sp->charge_low_t - 2731) / 10.0f);

    printf("  Discharge high temp:   %.2f°C\n", ((float)sp->discharge_high_t - 2731) / 10.0f);
    printf("  Discharge low temp:    %.2f°C\n", ((float)sp->discharge_low_t - 2731) / 10.0f);

    printf("  Charge current max:    %dmA\n", sp->charge_current_max * 10);
    printf("  Discharge current max: %dmA\n", sp->discharge_current_max * 10);

    printf("  Module high voltage:   %.2fV\n", ((float)sp->module_high_v) / 100.0f);
    printf("  Module low voltage:    %.2fV\n", ((float)sp->module_low_v) / 100.0f);
    printf("  Module under voltage:  %.2fV\n", ((float)sp->module_under_v) / 100.0f);
}

void tb_print_status(uint64_t status)
{
    if (status & TB_ALRMS_CELL_OVER_VOLTAGE_PROTECT)
    {
        printf("    Cell over voltage protect\n");
    }
    if (status & TB_ALRMS_CELL_UNDER_VOLTAGE)
    {
        printf("    Cell under voltage\n");
    }
    if (status & TB_ALRMS_CHARGE_OVER_CURRENT_PROTECT)
    {
        printf("    Charge over current protect\n");
    }
    if (status & TB_ALRMS_CELL_OVER_VOLTAGE_ALARM)
    {
        printf("    Cell over voltage alarm\n");
    }
    if (status & TB_ALRMS_DISCHARGE_OVER_CURRENT1_PROTECT)
    {
        printf("    Discharge over current 1 protect\n");
    }
    if (status & TB_ALRMS_CELL_TEMP_DISCHARGE_PROTECT)
    {
        printf("    Cell temperature discharge protect\n");
    }
    if (status & TB_ALRMS_CELL_TEMP_CHARGE_PROTECT)
    {
        printf("    Cell temperature charge protect\n");
    }
    if (status & TB_ALRMS_MODULE_UNDER_VOLTAGE)
    {
        printf("    Pack under voltage alarm\n");
    }
    if (status & TB_ALRMS_OPEN_CURRENT_LIMIT)
    {
        printf("    Open current limit\n");
    }
    if (status & TB_ALRMS_CHARGE_MOSFET)
    {
        printf("    Charge MOSFET on\n");
    }
    if (status & TB_ALRMS_DISCHARGE_MOSFET)
    {
        printf("    Discharge MOSFET on\n");
    }
    if (status & TB_ALRMS_SHORT_CIRCUIT_PROTECT)
    {
        printf("    Short circuit protect\n");
    }
    if (status & TB_ALRMS_CELL_UNDER_VOLTAGE_PROTECT)
    {
        printf("    Cell under voltage protect\n");
    }
    if (status & TB_ALRMS_MODULE_UNDER_VOLTAGE_PROTECT)
    {
        printf("    Pack under voltage protect\n");
    }
    if (status & TB_ALRMS_REVERSE_PROTECT)
    {
        printf("    Reverse protect\n");
    }
    if (status & TB_ALRMS_SOC_LOW_ALARM)
    {
        printf("    SOC low alarm\n");
    }
    if (status & TB_ALRMS_BUZZER_ON)
    {
        printf("    Buzzer on\n");
    }
    if (status & TB_ALRMS_CHARGER_FAULT)
    {
        printf("    Charger fault\n");
    }
    if (status & TB_ALRMS_2G_MODULE_FAILURE)
    {
        printf("    2G module failure\n");
    }
    if (status & TB_ALRMS_FULL_STATE)
    {
        printf("    Full charge state\n");
    }
    if (status & TB_ALRMS_CHARGER_CONNECTED)
    {
        printf("    Charger connected\n");
    }
    if (status & TB_ALRMS_HEATER_ON)
    {
        printf("    Heating element on\n");
    }
    if (status & TB_ALRMS_DISCHARGING)
    {
        printf("    Currently discharging\n");
    }
    if (status & TB_ALRMS_CHARGING)
    {
        printf("    Currently charging\n");
    }
    if (status & TB_ALRMS_UNDEFINED_3_0)
    {
        printf("    Undefined status 3.0\n");
    }
    if (status & TB_ALRMS_UNDEFINED_3_1)
    {
        printf("    Undefined status 3.1\n");
    }
    if (status & TB_ALRMS_UNDEFINED_3_2)
    {
        printf("    Undefined status 3.2\n");
    }
    if (status & TB_ALRMS_UNDEFINED_3_3)
    {
        printf("    Undefined status 3.3\n");
    }
    if (status & TB_ALRMS_CELL_VOLTAGE_LOW_FORCE_PROTECT)
    {
        printf("    Cell voltage low force protect\n");
    }
    if (status & TB_ALRMS_DISCONNECTOR_STATE)
    {
        printf("    Disconnect state\n");
    }
    if (status & TB_ALRMS_AEROSOL_TRIGGERED)
    {
        printf("    Fire suppression triggered\n");
    }
    if (status & TB_ALRMS_PRECHARGE_ON)
    {
        printf("    Precharge on\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_0)
    {
        printf("    Undefined status 4.0\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_1)
    {
        printf("    Undefined status 4.1\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_2)
    {
        printf("    Undefined status 4.2\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_3)
    {
        printf("    Undefined status 4.3\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_4)
    {
        printf("    Undefined status 4.4\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_5)
    {
        printf("    Undefined status 4.5\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_6)
    {
        printf("    Undefined status 4.6\n");
    }
    if (status & TB_ALRMS_UNDEFINED_4_7)
    {
        printf("    Undefined status 4.7\n");
    }
    if (status & TB_ALRMS_MODULE_OVER_VOLTAGE_ALARM)
    {
        printf("    Pack over voltage alarm\n");
    }
    if (status & TB_ALRMS_MOS_NTC_TEMPERATURE_ALARM)
    {
        printf("    MOS NTC temperature alarm\n");
    }
    if (status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_LOW_ALARM)
    {
        printf("    Environment NTC temperature low alarm\n");
    }
    if (status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_HIGH_ALARM)
    {
        printf("    Environment NTC temperature high alarm\n");
    }
    if (status & TB_ALRMS_CELL_NTC_TEMPERATURE_LOW_ALARM)
    {
        printf("    Cell NTC temperature low alarm\n");
    }
    if (status & TB_ALRMS_CELL_NTC_TEMPERATURE_HIGH_ALARM)
    {
        printf("    Cell NTC temperature high alarm\n");
    }
    if (status & TB_ALRMS_DISCHARGE_CURRENT_ALARM)
    {
        printf("    Discharge current alarm\n");
    }
    if (status & TB_ALRMS_CHARGE_CURRENT_ALARM)
    {
        printf("    Charge current alarm\n");
    }
    if (status & TB_ALRMS_BALANCE_NTC_TEMPERATURE_ALARM)
    {
        printf("    Balance NTC temperature alarm\n");
    }
    if (status & TB_ALRMS_BALANCE_NTC_TEMPERATURE_PROTECT)
    {
        printf("    Balance NTC temperature protect\n");
    }
    if (status & TB_ALRMS_DISCHARGE_MOSFET_FAULT)
    {
        printf("    Discharge MOSFET fault\n");
    }
    if (status & TB_ALRMS_CHARGE_MOSFET_FAULT)
    {
        printf("    Charge MOSFET fault\n");
    }
    if (status & TB_ALRMS_CURRENT_SENSOR_FAULT)
    {
        printf("    Current sensor fault\n");
    }
    if (status & TB_ALRMS_AFE_FAULT)
    {
        printf("    AFE fault\n");
    }
    if (status & TB_ALRMS_NTC_FAULT)
    {
        printf("    NTC fault\n");
    }
    if (status & TB_ALRMS_CELL_FAULT)
    {
        printf("    Cell fault\n");
    }
    if (status & TB_ALRMS_DISCHARGE_OVER_CURRENT2_PROTECT)
    {
        printf("    Discharge over current 2 protect\n");
    }
    if (status & TB_ALRMS_SMART_CHARGING)
    {
        printf("    Smart charging\n");
    }
    if (status & TB_ALRMS_MODULE_OVER_VOLTAGE_PROTECT)
    {
        printf("    Pack over voltage protect\n");
    }
    if (status & TB_ALRMS_MOS_NTC_TEMPERATURE_PROTECT)
    {
        printf("    MOS NTC temperature protect\n");
    }
    if (status & TB_ALRMS_DISCHARGE_MOSFET_FORCED_CLOSE)
    {
        printf("    Discharge MOSFET forced close\n");
    }
    if (status & TB_ALRMS_CHARGE_MOSFET_FORCED_CLOSE)
    {
        printf("    Charge MOSFET forced close\n");
    }
    if (status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_PROTECT_DISCHARGING)
    {
        printf("    Environment NTC temperature protect in discharging\n");
    }
    if (status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_PROTECT_CHARGING)
    {
        printf("    Environment NTC temperature protect in charging\n");
    }

}

void tb_print_alarm_info(struct tb_alarm_info* ainfo)
{
    printf("  Cell voltage status:      ");
    for (uint16_t i = 0; i < ainfo->cell_count; i++)
    {
        printf("%d:%02X", i+1, ainfo->cell_voltage_status[i]);
        if(i != ainfo->cell_count-1)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  Cell temperature status:  ");
    for (uint16_t i = 0; i < ainfo->temp_count-3; i++)
    {
        printf("(%d-%d):%02X", i*4+1, i*4+4, ainfo->cell_temp_status[i]);
        if(i != ainfo->cell_count-4)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  Balancer temp. status:    %02X\n", ainfo->balancer_temp_status);
    printf("  Environment temp. status: %02X\n", ainfo->environment_temp_status);
    printf("  MOSFET temp. status:      %02X\n", ainfo->mosfet_temp_status);
    printf("  Charge current status:    %02X\n", ainfo->charge_current_status);
    printf("  Module voltage status:    %02X\n", ainfo->module_voltage_status);
    printf("  Status byte count:        %d\n", ainfo->status_count);

    printf("  STATUS:\n");
    tb_print_status(ainfo->status);
}

void tb_print_date(struct tb_date* date)
{
    printf("  BMS date: %04d-%02d-%02d %02d:%02d:%02d\n",
            date->year, date->month, date->day,
            date->hour, date->minute, date->second);
}

const char* tb_historical_data_get_event_description(uint8_t event_type) {
    switch (event_type) {
        case 0:  return "Timing records";
        case 1:  return "Charging started";
        case 2:  return "Charging end";
        case 3:  return "Discharging started";
        case 4:  return "Discharging end";
        case 5:  return "Cell over voltage alarm";
        case 6:  return "Cell under voltage alarm";
        case 7:  return "Pack over voltage alarm";
        case 8:  return "Pack under voltage alarm";
        case 9:  return "Charge current alarm";
        case 10: return "Discharge current alarm";
        case 11: return "Cell NTC temperature high alarm";
        case 12: return "Cell NTC temperature low alarm";
        case 13: return "Cell over voltage alarm recovery";
        case 14: return "Cell under voltage alarm recovery";
        case 15: return "Pack over voltage alarm recovery";
        case 16: return "Pack under voltage alarm recovery";
        case 17: return "Charge current alarm recovery";
        case 18: return "Discharge current alarm recovery";
        case 19: return "Cell NTC temperature high alarm recovery";
        case 20: return "Cell NTC temperature low alarm recovery";
        case 21: return "Cell over voltage protect";
        case 22: return "Cell under voltage protect";
        case 23: return "Pack over voltage protect";
        case 24: return "Pack under voltage protect";
        case 25: return "Charge current protect";
        case 26: return "Discharge current protect";
        case 27: return "Cell NTC temperature protect";
        case 28: return "Undefined 28";
        case 29: return "Cell over voltage protect recovery";
        case 30: return "Cell under voltage protect recovery";
        case 31: return "Pack over voltage protect recovery";
        case 32: return "Pack under voltage protect recovery";
        case 33: return "Charge current protect recovery";
        case 34: return "Discharge current protect recovery";
        case 35: return "Cell NTC temperature protect recovery";
        case 36: return "Undefined 36";
        case 37: return "Environment NTC temperature high alarm";
        case 38: return "Environment NTC temperature low alarm";
        case 39: return "MOS NTC temperature alarm";
        case 40: return "Balance NTC temperature alarm";
        case 41: return "Short protect";
        case 42: return "Reverse protect";
        case 43: return "Cell fault alarm";
        case 44: return "Cell fault alarm recovery";
        case 45: return "Environment NTC temperature high alarm recovery";
        case 46: return "Environment NTC temperature low alarm recovery";
        case 47: return "MOS NTC temperature alarm recovery";
        case 48: return "Balance NTC temperature alarm recovery";
        case 49: return "AFE fault";
        case 50: return "Current limiting module fault";
        case 51: return "Charging MOS fault";
        case 52: return "Discharging MOS fault";
        case 53: return "Environment NTC temperature protect";
        case 54: return "Smart charge mode start";
        case 55: return "MOS NTC temperature protect";
        case 56: return "Balance NTC temperature high protect";
        case 57: return "NTC fault";
        case 58: return "NTC fault recovery";
        case 59: return "Heater start";
        case 60: return "Heater stop";
        case 61: return "Environment NTC temperature protect recovery";
        case 62: return "Smart charge mode stop";
        case 63: return "MOS NTC temperature protect recovery";
        case 64: return "Balance NTC temperature high protect recovery";
        case 65: return "Charger fault";
        case 66: return "LCD fault";
        case 67: return "System fault";
        case 68: return "Low capacity alarm";
        case 69: return "Low capacity alarm recovery";
        case 70: return "Short protect recovery";
        case 71: return "Reverse protect recovery";
        case 72: return "Current sensor fault";
        case 73: return "Current sensor fault recovery";
        case 74: return "AFE fault recovery";
        case 75: return "Mode_AC_Fault";
        case 76: return "Mode_AC_Fault_RLS";
        case 77: return "Current limiting module on";
        case 78: return "Current limiting module off";
        case 79: return "Undefined 79";
        case 80: return "Undefined 80";
        case 81: return "Undefined 81";
        case 82: return "Static sleep";
        case 83: return "Soft switch sleep";
        case 84: return "Under voltage protect sleep";
        case 85: return "Reset key sleep";
        case 86: return "Cell voltage too low forced sleep";
        case 87: return "Undefined 87";
        case 88: return "BMS start";
        case 89: return "Fire-suppression aerosol deployed";
        case 90: return "Release device start";
        case 91: return "Full charge calculation enable";
        case 92: return "Full charge";
        case 93: return "Undefined 93";
        default: return "Undefined";
    }
}
void tb_print_historical_data(struct tb_historical_data* hist)
{
    tb_print_date(&hist->date);
    printf("  event_type:         %s\n",
            tb_historical_data_get_event_description(hist->event_type));
    printf("  STATUS:\n");
    tb_print_status(hist->status);
    printf("  Current:            %.2fA\n", hist->current / 100.0);
    printf("  Pack voltage:       %.2fV\n", hist->pack_voltage / 100.0);
    printf("  Remaining capacity: %dmAh\n", hist->remaining_capacity*10);
    printf("  Cell voltages:      ");
    for(uint16_t k = 0; k < 16; k++)
    {
        printf("%d:%.2fV",k+1,((float)hist->cell_voltages[k])/1000.0);
        if(k != 15)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  Cell temps.:        ");
    for(uint16_t k = 0; k < 4; k++)
    {
        printf("(%d-%d):%.2fC", k*4+1, k*4+4, ((float)hist->cell_temps[k]-2731)/10.0);
        if(k != 3)
        {
            printf(", ");
        }
    }
    printf("\n");
    //printf("  Balancer temp:      %.2fC\n", ((float)hist->balancer_temp-2731)/10.0);
    printf("  Env. temp:          %.2fC\n", ((float)hist->environment_temp-2731)/10.0);
    printf("  MOSFET temp:        %.2fC\n", ((float)hist->mosfet_temp-2731)/10.0);

}

#endif // TOPBAND_PRINT_H
