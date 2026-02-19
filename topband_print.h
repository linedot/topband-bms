#ifndef TOPBAND_PRINT_H
#define TOPBAND_PRINT_H

#include "topband.h"


void tb_print_manufacturer_info(struct tb_manufacturer_info* minfo)
{
    printf("  HW: %s\n",minfo->hw);
    printf("  SW: ZM%02d\n", minfo->sw);
    printf("  ID: %s\n",minfo->id);
}

void tb_print_analog_values_fixed_point(struct tb_analog_values_fixed_point* avfp)
{
    printf("  Flags:          ");
    if(avfp->flags & TB_IFLAG_SW_VALUE_CHANGE)
    {
        printf("SW ");
    }
    if(avfp->flags & TB_IFLAG_ALRM_VALUE_CHANGE)
    {
        printf("ALRM ");
    }
    printf("\n");
    printf("  CMD:            %02X\n",avfp->cmd);
    printf("  Cell count:     %d\n",avfp->cell_count);
    printf("  Cell voltages:  ");
    for(size_t k = 0; k < avfp->cell_count; k++)
    {
        printf("%.2fV",((float)avfp->cell_voltages[k])/1000.0);
        if(k != avfp->cell_count-1)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  Temp. count:    %d\n",avfp->temp_count);
    printf("  BMS temp:       %.2fC\n", ((float)avfp->bms_temp-2731)/10.0);
    printf("  Cell temps.:    ");
    for(size_t k = 0; k < avfp->temp_count-1; k++)
    {
        printf("%.2fC",((float)avfp->cell_temps[k]-2731)/10.0);
        if(k != avfp->temp_count-2)
        {
            printf(", ");
        }
    }
    printf("\n");
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
        printf("%d:%02X", i, ainfo->cell_voltage_status[i]);
        if(i != ainfo->cell_count-1)
        {
            printf(", ");
        }
    }
    printf("\n");

    printf("  BMS temperature status:   %02X\n", ainfo->bms_temp_status);
    printf("  Cell temperature status:  ");
    for (uint16_t i = 0; i < ainfo->temp_count-1; i++)
    {
        printf("%d:%02X", i, ainfo->cell_temp_status[i]);
        if(i != ainfo->cell_count-2)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("  BMS temperature status:   %02X\n", ainfo->bms_temp_status);
    printf("  Charge current status:    %02X\n", ainfo->charge_current_status);
    printf("  Module voltage status:    %02X\n", ainfo->module_voltage_status);
    printf("  Discharge current status: %02X\n", ainfo->discharge_current_status);

    printf("  STATUS:\n");
    if (ainfo->status & TB_ALRMS_MODULE_OVER_VOLTAGE)
    {
        printf("    Module over voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_OVER_VOLTAGE)
    {
        printf("    Cell over voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_OVER_CURRENT)
    {
        printf("    Charge over current\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_OVER_CURRENT)
    {
        printf("    Discharge over current\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_OVER_TEMPERATURE)
    {
        printf("    Discharge over temperature\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_OVER_TEMPERATURE)
    {
        printf("    Charge over temperature\n");
    }
    if (ainfo->status & TB_ALRMS_MODULE_UNDER_VOLTAGE)
    {
        printf("    Module under voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_MOSFET) 
    {
        printf("    Charge MOSFET ON\n");
    }
    else
    {
        printf("    Charge MOSFET OFF\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_MOSFET) 
    {
        printf("    Discharge MOSFET ON\n");
    }
    else 
    {
        printf("    Discharge MOSFET OFF\n");
    }
    if (ainfo->status & TB_ALRMS_USING_BAT_MODULE_POWER)
    {
        printf("    Using battery module power\n");
    }
    if (ainfo->status & TB_ALRMS_BUZZER)
    {
        printf("    Buzzer ON\n");
    }
    else
    {
        printf("    Buzzer OFF\n");
    }
    if (ainfo->status & TB_ALRMS_FULLY_CHARGED)
    {
        printf("    Fully charged\n");
    }
    if (ainfo->status & TB_ALRMS_HEATER)
    {
        printf("    Heater ON\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_CURRENT_UNDER_100MA)
    {
        printf("    Discharge current under 100mA\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_CURRENT_UNDER_100MA)
    {
        printf("    Charge current under 100mA\n");
    }

    for (uint8_t i = 0; i < 16; ++i)
    {
        if (ainfo->status & (TB_ALRMS_CELL1_VOLTAGE_ERR << i))
        { // Starting from TB_ALRMS_CELL1_VOLTAGE_ERR
            printf("    Cell %d voltage error\n", i + 1);
        }
    }
}

#endif // TOPBAND_PRINT_H
