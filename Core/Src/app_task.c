#include "main.h"
#include "display.h"
#include "FreeRTOS.h"
#include "task.h"          // This is FreeRTOS task.h
#include "app_task.h"     // This is YOUR task header
#include "Can.h"

#define OBD_PID_ENGINE_COOLANT_TEMP       0x05U

#define OBD_PID_ENGINE_RPM                0x0CU

#define OBD_PID_VEHICLE_SPEED             0x0DU

#define OBD_PID_THROTTLE_POSITION         0x11U


void vDisplayTask(void *pvParameters)
{
    while (1)
    {
        if (Dashbord_Data.ui_state == UI_MAIN_MENU)
        {
            switch (Dashbord_Data.menu_selection)
            {
                case PARA_SPEED:
                    print_mainmenu_speed();
                    break;

                case PARA_RPM:
                    print_mainmenu_rpm();
                    break;

                case PARA_TEMP:
                    print_mainmenu_temp();
                    break;

                case PARA_THROTTLE:
                    print_mainmenu_throttle();
                    break;

                default:
                    break;
            }
        }

        else if (Dashbord_Data.ui_state == UI_PARAMETER_VIEW)
        {
            switch (Dashbord_Data.active_parameter)
            {
                case PARA_SPEED:
                    print_parameter_speed();
                    break;

                case PARA_RPM:
                    print_parameter_rpm();
                    break;

                case PARA_TEMP:
                    print_parameter_temp();
                    break;

                case PARA_THROTTLE:
                    print_parameter_throttle();
                    break;

                default:
                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


void vOBDTask(void *pvParameters)
{
    CAN_Message_t TxMsg;
    CAN_Message_t RxMsg;

    while (1)
    {
        switch (Dashbord_Data.active_parameter)
        {
            case PARA_SPEED:

                if (OBD_RequestPID(&TxMsg, OBD_PID_VEHICLE_SPEED))
                {
                    if (OBD_ReadResponse(&RxMsg))
                    {
                        Dashbord_Data.speed =
                            OBD_GetVehicleSpeed(&RxMsg);
                    }
                }

                break;


            case PARA_RPM:

                if (OBD_RequestPID(&TxMsg, OBD_PID_ENGINE_RPM))
                {
                    if (OBD_ReadResponse(&RxMsg))
                    {
                        Dashbord_Data.rpm =
                            OBD_GetRPM(&RxMsg);
                    }
                }

                break;


            case PARA_TEMP:

                if (OBD_RequestPID(&TxMsg, OBD_PID_ENGINE_COOLANT_TEMP))
                {
                    if (OBD_ReadResponse(&RxMsg))
                    {
                        Dashbord_Data.temp =
                            OBD_GetCoolantTemperature(&RxMsg);
                    }
                }

                break;


            case PARA_THROTTLE:

                if (OBD_RequestPID(&TxMsg, OBD_PID_THROTTLE_POSITION))
                {
                    if (OBD_ReadResponse(&RxMsg))
                    {
                        Dashbord_Data.throttle_position =
                            OBD_GetThrottlePosition(&RxMsg);
                    }
                }

                break;


            case PARA_NONE:

                /* Main menu: No OBD request */
                break;


            default:
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
