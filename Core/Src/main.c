#include"main.h"
#include "stm32f103xb.h"
#include "stm32f1xx_it.h"
#include "gpio_task.h"
#include "app_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_task.h"
#include "Can.h"
#include "m3_cmsis_serial.h"


Dashbord_Data_t Dashbord_Data =
{
    .ui_state            = UI_MAIN_MENU,
    .menu_selection      = PARA_SPEED,
    .active_parameter    = PARA_NONE,

    .speed               = 0,
    .rpm                 = 0,
    .temp = 0,
    .throttle_position   = 0
};


int main()
{
	Button_EXTI_Init();


	xTaskCreate(
	    vDisplayTask,
	    "Display",
	    256,
	    NULL,
	    2,
	    NULL
	);



	xTaskCreate(
	    vOBDTask,
	    "OBD",
	    256,
	    NULL,
	    2,
	    NULL
	);

	vTaskStartScheduler();

	while(1);
}
