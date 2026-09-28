#include "stm32f103xb.h"
#include "I2C driver.h"
#include "oled.h"
#include "main.h"

void Move_Buffer_ptr(uint8_t page, uint8_t column);
void OLED_Write(char *str);
void OLED_DrawFilledSquare(void);
void OLED_DrawHollowSquare(void);
void OLED_UpdateScreen(void);



void print_mainmenu_speed(void)
{
    
        Move_Buffer_ptr(0, 7);
        OLED_Write("* MAIN MENU *");

        Move_Buffer_ptr(1, 0);
        OLED_DrawFilledSquare();
        Move_Buffer_ptr(1,8);
        OLED_Write("SPEED");


        Move_Buffer_ptr(3, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(3,8);
        OLED_Write("RPM");

        Move_Buffer_ptr(5, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(5,8);
        OLED_Write("TEMP");

        Move_Buffer_ptr(7, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(7,8);
        OLED_Write("THROTTLE");

        OLED_UpdateScreen();
}

void print_mainmenu_rpm(void)
{
    
        Move_Buffer_ptr(0, 7);
        OLED_Write("* MAIN MENU *");

        Move_Buffer_ptr(1, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(1,8);
        OLED_Write("SPEED");


        Move_Buffer_ptr(3, 0);
        OLED_DrawFilledSquare();
        Move_Buffer_ptr(3,8);
        OLED_Write("RPM");

        Move_Buffer_ptr(5, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(5,8);
        OLED_Write("TEMP");

        Move_Buffer_ptr(7, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(7,8);
        OLED_Write("THROTTLE");

        OLED_UpdateScreen();
}

void print_mainmenu_temp(void)
{
    
        Move_Buffer_ptr(0, 7);
        OLED_Write("* MAIN MENU *");

        Move_Buffer_ptr(1, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(1,8);
        OLED_Write("SPEED");


        Move_Buffer_ptr(3, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(3,8);
        OLED_Write("RPM");

        Move_Buffer_ptr(5, 0);
        OLED_DrawFilledSquare();
        Move_Buffer_ptr(5,8);
        OLED_Write("TEMP");

        Move_Buffer_ptr(7, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(7,8);
        OLED_Write("THROTTLE");

        OLED_UpdateScreen();
}

void print_mainmenu_throttle(void)
{
    
        Move_Buffer_ptr(0, 7);
        OLED_Write("* MAIN MENU *");

        Move_Buffer_ptr(1, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(1,8);
        OLED_Write("SPEED");


        Move_Buffer_ptr(3, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(3,8);
        OLED_Write("RPM");

        Move_Buffer_ptr(5, 0);
        OLED_DrawHollowSquare();
        Move_Buffer_ptr(5,8);
        OLED_Write("TEMP");

        Move_Buffer_ptr(7, 0);
        OLED_DrawFilledSquare();
        Move_Buffer_ptr(7,8);
        OLED_Write("THROTTLE");

        OLED_UpdateScreen();
}

void Int_To_String(uint16_t value, char *buffer)
{
    char temp[6];
    uint8_t i = 0;
    uint8_t j = 0;

    /* Special case for 0 */
    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    /* Extract digits in reverse order */
    while (value > 0)
    {
        temp[i] = (value % 10) + '0';
        value = value / 10;
        i++;
    }

    /* Reverse digits into output buffer */
    while (i > 0)
    {
        i--;
        buffer[j] = temp[i];
        j++;
    }

    /* Null terminate the string */
    buffer[j] = '\0';
}

void print_parameter_speed(void)
{
    char value[6];

    Int_To_String(Dashbord_Data.speed, value);

    /* Parameter name at top */
    Move_Buffer_ptr(0, 50);
    OLED_Write("SPEED");

    /* Current value near center */
    Move_Buffer_ptr(4, 50);
    OLED_Write(value);

    OLED_UpdateScreen();
}

void print_parameter_rpm(void)
{
    char value[6];

    Int_To_String(Dashbord_Data.rpm, value);

    /* Parameter name at top */
    Move_Buffer_ptr(0, 55);
    OLED_Write("RPM");

    /* Current value near center */
    Move_Buffer_ptr(4, 50);
    OLED_Write(value);

    OLED_UpdateScreen();
}

void print_parameter_temp(void)
{
    char value[6];

    Int_To_String(Dashbord_Data.temp, value);

    /* Parameter name at top */
    Move_Buffer_ptr(0, 35);
    OLED_Write("TEMPERATURE");

    /* Current value near center */
    Move_Buffer_ptr(4, 50);
    OLED_Write(value);

    OLED_UpdateScreen();
}

void print_parameter_throttle(void)
{
    char value[6];

    Int_To_String(Dashbord_Data.throttle_position, value);

    /* Parameter name at top */
    Move_Buffer_ptr(0, 40);
    OLED_Write("THROTTLE");

    /* Current value near center */
    Move_Buffer_ptr(4, 50);
    OLED_Write(value);

    OLED_UpdateScreen();
}
