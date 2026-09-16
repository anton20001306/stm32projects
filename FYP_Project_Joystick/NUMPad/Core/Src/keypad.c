#include "stm32f446xx.h"
#include "keypad.h"
#include "uart.h"
#include "delay.h"

const unsigned char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/* Forward declaration: decode_keypad is defined further down but used above. */
static char decode_keypad(uint8_t row, uint8_t col_bits);

void keypad_init(void)
{

	/*Enable clock access to GPIOC*/
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;

	/*Set PC0 to PC3 as input*/
	GPIOC->MODER &= ~(GPIO_MODER_MODE0|GPIO_MODER_MODE1|GPIO_MODER_MODE2|GPIO_MODER_MODE3);

	/* Activate internal pullup resistor for PC0 to PC3*/
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD0_0|GPIO_PUPDR_PUPD1_0|GPIO_PUPDR_PUPD2_0|GPIO_PUPDR_PUPD3_0;

	/*Set PC4 to PC7 as output*/
	GPIOC->MODER |= GPIO_MODER_MODE4_0|GPIO_MODER_MODE5_0|GPIO_MODER_MODE6_0|GPIO_MODER_MODE7_0;

	GPIOC->MODER &=~( GPIO_MODER_MODE4_1|GPIO_MODER_MODE5_1|GPIO_MODER_MODE6_1|GPIO_MODER_MODE7_1);

	/*Set PC4 to PC7 as high*/

	GPIOC->BSRR = GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BS6|GPIO_BSRR_BS7;
}

const uint32_t clo_state[4]={	(GPIO_BSRR_BR4|GPIO_BSRR_BS5|GPIO_BSRR_BS6|GPIO_BSRR_BS7),
						(GPIO_BSRR_BS4|GPIO_BSRR_BR5|GPIO_BSRR_BS6|GPIO_BSRR_BS7),
						(GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BR6|GPIO_BSRR_BS7),
						(GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BS6|GPIO_BSRR_BR7)
};

char keypad_read(void)
{

	unsigned char key=0,data=0;

	for (int i=0;i<4;i++)
	{

		GPIOC->BSRR=clo_state[i];

        delay_ms(30); /* wait for the row to settle */  

		data=(GPIOC->IDR)&0xF; /*Get rid of data from bit 5 to bit31*/

		if(data != 0xF)
		{
			key=decode_keypad(i,data);
			return key; /* first key found -> done */
		}

	}

	return key;
}

static char decode_keypad(uint8_t row, uint8_t col_bits)
{
	/* row = which row is driven low; col_bits = column reading (active-low). */
	if (col_bits == 0xE) return keymap[row][0]; /* PC0 low -> column 0 */
	if (col_bits == 0xD) return keymap[row][1]; /* PC1 low -> column 1 */
	if (col_bits == 0xB) return keymap[row][2]; /* PC2 low -> column 2 */
	if (col_bits == 0x7) return keymap[row][3]; /* PC3 low -> column 3 */
	return 0;
}

void keypad_send_command(char key){
	switch(key){
        case '1':
            Send_String("mode guided\r\n");
            break;

        case '2':
            Send_String("arm throttle\r\n");
            break;

        case '3':
            Send_String("takeoff 5\r\n");
            break;

        case '4':
            Send_String("mode land\r\n");
            break;

        case '5':
            Send_String("disarm\r\n");
            break;

        case 'C':
            Send_String("mode rtl\r\n");
            break;

        default:
            break;
	}
}

