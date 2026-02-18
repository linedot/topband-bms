#ifndef TOPBAND_PRINT_H
#define TOPBAND_PRINT_H

#include "topband.h"


void tb_print_manufacturer_info(struct tb_manufacturer_info* minfo)
{
    printf("  Battery name:      %s\n",minfo->battery_name);
    printf("  SW version:        %d.%d\n", minfo->sw_version >> 8, minfo->sw_version & 0xFF);
    printf("  Manufacturer name: %s\n",minfo->manufacturer_name);
}

void tb_print_analog_values_fixed_point(struct tb_analog_values_fixed_point* avfp)
{
    printf("  Flags:         ");
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
        printf("  Capacity2:       %d/%dmAh\n",avfp->remaining_capacity2 * 10, avfp->total_capacity2 * 10);
    }
    else
    {
        printf("  Capacity1:       %d/%dmAh\n",avfp->remaining_capacity * 10, avfp->total_capacity * 10);
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

void tb_print_charge_discharge_management_info(struct tb_charge_discharge_management_info* cdmi)
{
    printf("  CMD:                     %02X\n", cdmi->cmd);
    printf("  Max charge voltage:      %.2fV\n", ((float)cdmi->charge_max_v) / 1000.0f);
    printf("  Max discharge voltage:   %.2fV\n", ((float)cdmi->discharge_max_v) / 1000.0f);

    printf("  Max charge current:      %dmA\n", cdmi->charge_max_a * 10);
    printf("  Max discharge current:   %dmA\n", cdmi->discharge_max_a * 10);

    printf("  Charge/discharge status: ");
    if(cdmi->charge_discharge_status & TB_CHADIS_STATUS_FULL_CHARGE_REQUEST)
    {
        printf("FCR ");
    }
    if(cdmi->charge_discharge_status & TB_CHADIS_STATUS_CHARGE_IMMEDIATELY2)
    {
        printf("CI2 ");
    }
    if(cdmi->charge_discharge_status & TB_CHADIS_STATUS_CHARGE_IMMEDIATELY1)
    {
        printf("CI1 ");
    }
    if(cdmi->charge_discharge_status & TB_CHADIS_STATUS_DISCHARGE_ENABLE)
    {
        printf("DE ");
    }
    if(cdmi->charge_discharge_status & TB_CHADIS_STATUS_CHARGE_ENABLE)
    {
        printf("CE ");
    }
    printf("\n");

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
    if (ainfo->status & TB_ALRMS_MODULE_OVER_VOLTAGE) {
        printf("    Module over voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CELL_OVER_VOLTAGE) {
        printf("    Cell over voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_OVER_CURRENT) {
        printf("    Charge over current\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_OVER_CURRENT) {
        printf("    Discharge over current\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_OVER_TEMPERATURE) {
        printf("    Discharge over temperature\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_OVER_TEMPERATURE) {
        printf("    Charge over temperature\n");
    }
    if (ainfo->status & TB_ALRMS_MODULE_UNDER_VOLTAGE) {
        printf("    Module under voltage\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_MOSFET) {
        printf("    Charge MOSFET\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_MOSFET) {
        printf("    Discharge MOSFET\n");
    }
    if (ainfo->status & TB_ALRMS_USING_BAT_MODULE_POWER) {
        printf("    Using battery module power\n");
    }
    if (ainfo->status & TB_ALRMS_BUZZER) {
        printf("    Buzzer\n");
    }
    if (ainfo->status & TB_ALRMS_FULLY_CHARGED) {
        printf("    Fully charged\n");
    }
    if (ainfo->status & TB_ALRMS_HEATER) {
        printf("    Heater\n");
    }
    if (ainfo->status & TB_ALRMS_DISCHARGE_CURRENT_UNDER_100MA) {
        printf("    Discharge current under 100mA\n");
    }
    if (ainfo->status & TB_ALRMS_CHARGE_CURRENT_UNDER_100MA) {
        printf("    Charge current under 100mA\n");
    }

    for (uint8_t i = 0; i < 16; ++i) {
        if (ainfo->status & (1ULL << (24 + i))) { // Starting from TB_ALRMS_CELL1_VOLTAGE_ERR
            printf("    Cell %d voltage error\n", i + 1);
        }
    }
}

void tb_print_sn(struct tb_sn* sn)
{
    printf(" cmd: %02X\n", sn->cmd);
    printf(" sn:  %s\n", sn->sn);
}

#endif // TOPBAND_PRINT_H
