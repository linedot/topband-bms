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
    if (ainfo->status & TB_ALRMS_CELL_OVER_VOLTAGE_PROTECT)
    {
        printf("    Cell over voltage protect\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_UNDER_VOLTAGE)
    {
        printf("    Cell under voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_OVER_CURRENT)
    {
        printf("    Charge over current\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_OVER_VOLTAGE_ALARM)
    {
        printf("    Cell over voltage alarm\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_OVER_CURRENT1_PROTECT)
    {
        printf("    Discharge over current 1 protect\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_TEMP_DISCHARGE_PROTECT)
    {
        printf("    Cell temperature discharge protect\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_TEMP_CHARGE_PROTECT)
    {
        printf("    Cell temperature charge protect\n");
    }
    if (ainfo->status & TB_ALRMS_MODULE_UNDER_VOLTAGE)
    {
        printf("    Pack under voltage alarm\n");
    }
    if (ainfo->status & TB_ALRMS_OPEN_CURRENT_LIMIT)
    {
        printf("    Open current limit\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_MOSFET)
    {
        printf("    Charge MOSFET on\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_MOSFET)
    {
        printf("    Discharge MOSFET on\n");
    }
    if (ainfo->status & TB_ALRMS_SHORT_CIRCUIT_PROTECT)
    {
        printf("    Short circuit protect\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_UNDER_VOLTAGE_PROTECT)
    {
        printf("    Cell under voltage protect\n");
    }
    if (ainfo->status & TB_ALRMS_PACK_UNDER_VOLTAGE_PROTECT)
    {
        printf("    Pack under voltage protect\n");
    }
    if (ainfo->status & TB_ALRMS_REVERSE_PROTECT)
    {
        printf("    Reverse protect\n");
    }
    if (ainfo->status & TB_ALRMS_SOC_LOW_ALARM)
    {
        printf("    SOC low alarm\n");
    }
    if (ainfo->status & TB_ALRMS_BUZZER_ON)
    {
        printf("    Buzzer on\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGER_FAULT)
    {
        printf("    Charger fault\n");
    }
    if (ainfo->status & TB_ALRMS_2G_MODULE_FAILURE)
    {
        printf("    2G module failure\n");
    }
    if (ainfo->status & TB_ALRMS_FULL_STATE)
    {
        printf("    Full charge state\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGER_CONNECTED)
    {
        printf("    Charger connected\n");
    }
    if (ainfo->status & TB_ALRMS_HEATER_ON)
    {
        printf("    Heating element on\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGING)
    {
        printf("    Currently discharging\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGING)
    {
        printf("    Currently charging\n");
    }
    if (ainfo->status & TB_ALRMS_UNDEFINED_3_0)
    {
        printf("    Undefined status 3.0\n");
    }
    if (ainfo->status & TB_ALRMS_UNDEFINED_3_1)
    {
        printf("    Undefined status 3.1\n");
    }
    if (ainfo->status & TB_ALRMS_UNDEFINED_3_2)
    {
        printf("    Undefined status 3.2\n");
    }
    if (ainfo->status & TB_ALRMS_UNDEFINED_3_3)
    {
        printf("    Undefined status 3.3\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_VOLTAGE_LOW_FORCE_PROTECT)
    {
        printf("    Cell voltage low force protect\n");
    }
    if (ainfo->status & TB_ALRMS_DISCONNECTOR_STATE)
    {
        printf("    Disconnect state\n");
    }
    if (ainfo->status & TB_ALRMS_AEROSOL_TRIGGERED)
    {
        printf("    Fire suppression triggered\n");
    }
    if (ainfo->status & TB_ALRMS_PRECHARGE_ON)
    {
        printf("    Precharge on\n");
    }
    if (ainfo->status & TB_ALRMS_PACK_OVER_VOLTAGE_ALARM)
    {
        printf("    Pack over voltage alarm\n");
    }
    if (ainfo->status & TB_ALRMS_MOS_NTC_TEMPERATURE_ALARM)
    {
        printf("    MOS NTC temperature alarm\n");
    }
    if (ainfo->status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_LOW_ALARM)
    {
        printf("    Environment NTC temperature low alarm\n");
    }
    if (ainfo->status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_HIGH_ALARM)
    {
        printf("    Environment NTC temperature high alarm\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_NTC_TEMPERATURE_LOW_ALARM)
    {
        printf("    Cell NTC temperature low alarm\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_NTC_TEMPERATURE_HIGH_ALARM)
    {
        printf("    Cell NTC temperature high alarm\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_CURRENT_ALARM)
    {
        printf("    Discharge current alarm\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_CURRENT_ALARM)
    {
        printf("    Charge current alarm\n");
    }
    if (ainfo->status & TB_ALRMS_BALANCE_NTC_TEMPERATURE_ALARM)
    {
        printf("    Balance NTC temperature alarm\n");
    }
    if (ainfo->status & TB_ALRMS_BALANCE_NTC_TEMPERATURE_PROTECT)
    {
        printf("    Balance NTC temperature protect\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_MOSFET_FAULT)
    {
        printf("    Discharge MOSFET fault\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_MOSFET_FAULT)
    {
        printf("    Charge MOSFET fault\n");
    }
    if (ainfo->status & TB_ALRMS_CURRENT_SENSOR_FAULT)
    {
        printf("    Current sensor fault\n");
    }
    if (ainfo->status & TB_ALRMS_AFE_FAULT)
    {
        printf("    AFE fault\n");
    }
    if (ainfo->status & TB_ALRMS_NTC_FAULT)
    {
        printf("    NTC fault\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_FAULT)
    {
        printf("    Cell fault\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_OVER_CURRENT2_PROTECT)
    {
        printf("    Discharge over current 2 protect\n");
    }
    if (ainfo->status & TB_ALRMS_SMART_CHARGING)
    {
        printf("    Smart charging\n");
    }
    if (ainfo->status & TB_ALRMS_PACK_OVER_VOLTAGE_PROTECT)
    {
        printf("    Pack over voltage protect\n");
    }
    if (ainfo->status & TB_ALRMS_MOS_NTC_TEMPERATURE_PROTECT)
    {
        printf("    MOS NTC temperature protect\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_MOSFET_FORCED_CLOSE)
    {
        printf("    Discharge MOSFET forced close\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_MOSFET_FORCED_CLOSE)
    {
        printf("    Charge MOSFET forced close\n");
    }
    if (ainfo->status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_PROTECT_DISCHARGING)
    {
        printf("    Environment NTC temperature protect in discharging\n");
    }
    if (ainfo->status & TB_ALRMS_ENVIRONMENT_NTC_TEMPERATURE_PROTECT_CHARGING)
    {
        printf("    Environment NTC temperature protect in charging\n");
    }
    if (ainfo->status & TB_ALRMS_REQUEST_SLEEP_DTU)
    {
        printf("    Request sleep DTU\n");
    }
    if (ainfo->status & TB_ALRMS_BATTERY_LOW_TEMPERATURE_PROTECT)
    {
        printf("    Battery low temperature protect\n");
    }
    if (ainfo->status & TB_ALRMS_BATTERY_HIGH_TEMPERATURE_PROTECT)
    {
        printf("    Battery high temperature protect\n");
    }
    if (ainfo->status & TB_ALRMS_LARGE_CELL_VOLTAGE_DIFFERENTIAL_ALARM)
    {
        printf("    Large cell voltage differential alarm\n");
    }
    if (ainfo->status & TB_ALRMS_MOSFET_HIGH_TEMPERATURE_ALARM)
    {
        printf("    MOSFET high temperature alarm\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_ULTRA_HIGH_TEMPERATURE_PROTECT)
    {
        printf("    Cell ultra high temperature protect\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_LIMIT_CURRENT_ON)
    {
        printf("    Discharge limit current on\n");
    }
    if (ainfo->status & TB_ALRMS_SOC_LOW_PROTECTION)
    {
        printf("    SOC low protection\n");
    }
}

void tb_print_date(struct tb_date* date)
{
    printf("  BMS date: %d-%d-%d %d:%d:%d\n",
            date->year, date->month, date->day,
            date->hour, date->minute, date->second);
}

void tb_print_historical_data(struct tb_historical_data* hist)
{
    printf("  location:  %02X\n", hist->location);
    printf("  cmd type:  %02X\n", hist->cmd_type);
    tb_print_date(&hist->date);
    printf("  sys. mode: %02X\n", hist->system_mode);
    printf("  alarm_byte_count:  %d\n", hist->alarm_byte_count);
    printf("  cell_status_event: %02X\n", hist->cell_status_event);
    printf("  temp. event:       %04X\n", hist->temp_event);
    printf("  current event:     %02X\n", hist->current_event);
}

struct tb_historical_data
{
    uint8_t location;
    uint8_t cmd_type;
    struct tb_date date;
    uint8_t system_mode;
    uint8_t alarm_byte_count;
    uint8_t cell_status_event;
    uint8_t single_voltage_event;
    uint16_t temp_event;
    uint8_t current_event;
};

#endif // TOPBAND_PRINT_H
