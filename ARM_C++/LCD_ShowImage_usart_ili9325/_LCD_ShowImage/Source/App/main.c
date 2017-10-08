#include "stm32f10x.h"
#include "GLCD.h"
#include "USART.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
extern u8 Image_Table[];

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void RCC_Configuration(void);
void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/
int main(void)
{  	
    char c;
	RCC_Configuration();	
   	/* LCD Module init */
   	GLCD_init();
	GPIO_Configuration();
	NVIC_Configuration();
	while(SysTick_Config(SystemFrequency / 1000));	  
   	GLCD_clear(White);
   	GLCD_setTextColor(Blue);
   	GLCD_displayStringLn(Line1, "     MEISTER");
   	GLCD_displayStringLn(Line2, "Fire Warning System");
   	GLCD_setTextColor(Red);
	USART1_Configuration();
//	USART1Write((u8*)"    Picture_example",sizeof("    Picture_example"));
	LCD_WriteBMP(68, 64, 144, 192, Image_Table);
 	while(1)
  	{
    	/* Turn on LD2 and LD3 */
    	GPIO_SetBits(GPIOD, GPIO_Pin_8);
    	/* Turn off LD1 */
    	GPIO_ResetBits(GPIOD, GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
    	/* Insert delay */
 
 	   c =(unsigned char)USART_ReceiveData(USART1);
	   GLCD_setTextColor(Red);
	   GLCD_displayChar(Line3, 60, c); 
	   //USART1_SendByte(c);
	   if (c == 'f') 
	   {

		 


    	Delay(200);	 
		GLCD_clear(White);
		GLCD_setTextColor(Blue);
   	    GLCD_displayStringLn(Line1, "     Warning");
   	    GLCD_displayStringLn(Line2, "     HOUSE"); 

		Delay(200);	
		Delay(200);	
		Delay(200);	

		GLCD_setTextColor(Red);
   	    GLCD_displayStringLn(Line1, "     Warning");
   	    GLCD_displayStringLn(Line2, "     HOUSE"); 
		 }
	  /*
       else{

	   	GLCD_setTextColor(Blue);
	   GLCD_displayChar(Line4, 50, c); 
	  // GLCD_displayStringLn(Line4, "    RESET");
        }
		*/
		 
		else if (c=='s')   
		  {
		GLCD_setTextColor(Blue);
   	    GLCD_displayStringLn(Line1, "     MEISTER");
   	    GLCD_displayStringLn(Line2, "   Picture house"); 
	  
		  } 
		  
		                           
    	/* Turn on LD2 and LD3 */
    	GPIO_SetBits(GPIOD, GPIO_Pin_9);
    	/* Turn off LD1 */
    	GPIO_ResetBits(GPIOD, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11);
    	/* Insert delay */
    	Delay(200);	  

    	/* Turn on LD4 */
    	GPIO_SetBits(GPIOD, GPIO_Pin_10);
    	/* Turn off LD2 and LD3 */
    	GPIO_ResetBits(GPIOD, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11);
    	/* Insert delay */
    	Delay(200);	  

    	/* Turn on LD4 */
    	GPIO_SetBits(GPIOD, GPIO_Pin_11);
    	/* Turn off LD2 and LD3 */
    	GPIO_ResetBits(GPIOD, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
    	/* Insert delay */
    	Delay(200);		
  	}
}
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure IO connected to LD1, LD2, LD3 and LD4 leds *********************/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);

   	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  	/* Configure USART1 Rx (PA.10) as input floating */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void NVIC_Configuration(void)
{ 
	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
}


void RCC_Configuration(void)
{
	SystemInit();	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA 
                           |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
                           ,ENABLE );
}
/**
  * @brief  Inserts a delay time.
  * @param nTime: specifies the delay time length, in milliseconds.
  * @retval : None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval : None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
