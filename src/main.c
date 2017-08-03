/*******************************************************************************
* DISCLAIMER
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* Copyright holders reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software. By using this 
* software, you agree to the additional terms and conditions found by accessing  
* the following link:
* http://www.renesas.com/disclaimer 
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
* AUTHOR : PORIPSA CHAKRABATY       
*******************************************************************************/
/*******************************************************************************
* File Name     : main.c
* Version       : 3.0
* Device(s)     : RX63N
* Tool-Chain    : Renesas RX Standard Toolchain 1.0.0
* H/W Platform  : YRDKRX63N
* Description   : Main function of the top gun game 
* *******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY     Version     Description
*         : 24.10.2015     3.00        First release              
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "platform.h" 
#include "r_switches.h"
#include "cmt_periodic_multi.h"
#include "timer_adc.h"
#include "flash_led.h"

//Function Initialization;
void Reset_All_LEDs(void);
void All_Red_LEDs_ON(void);
void Block_Until_Switch_Press(void);
void Blink_Red_LEDs(void);
void Blink_Any_LEDs(void);
void Create_Obstacle(void);

/*******************************************************************************
Macro Definitions
*******************************************************************************/
#define TIMER_COUNT_600MS 56250  /* 600mS with 48MHz pclk/512 */
#define MCU_NAME "   RX63N    "
#define CHANNEL_0   0


/*******************************************************************************
External functions and public global variables. 
*******************************************************************************/
/* Statics test replacement variable */
uint8_t g_working_string[13] = "   STATIC   "; /* 12 characters plus NULL terminator. */

/* Statics test const variable */
const uint8_t g_replacement_str[] = "TEST TEST"; /* Must be 12 chars or less. */

volatile bool g_sw1_press = false;
volatile bool g_sw2_press = false;
volatile bool g_sw3_press = false;


/*******************************************************************************
Private function prototypes and global variables
*******************************************************************************/
static void statics_test(void);
static void timer_callback(void);

volatile static bool g_delay_complete = false;
int blink_red_var = 1;
bool blink_red_state = true; //variable to remember red led blink state
int sw3_state = 0; //variable to remmember the switch 3 press
int obs_x = 0;  // variable for obstacle horizontal position
int obs_y = 0;  // variable for obstacle vertical position 
int jani_x = 0; // variable for jet horizontal position
int jani_y = 0; // variable for jet vertical position
bool enemy_flag = 0; //variable for creating enemy plane bool hit_flag = 0; 
bool hit_flag = 0; //variable for tracking hits and hit animation

/******************************************************************************
* Function name: main
* Description  : Main program function. The function first initialises the debug
*                LCD, and displays the splash screen on the LCD. The function
*                then calls the flash_led, timer_adc and statics_test functions. 
*                These start by flashing the LEDs at a fixed rate until a switch is
*                pressed, then the flash rate varies with the position of the   
*                pot, RV1. Finally, statics_test displays Game logo on the    
*                debug LCD, which continues to be displayed until a switch is pressed
*                after which the game starts. 
*                time.
*
* Arguments    : none
* Return value : none
******************************************************************************/
void main(void)
{
    /* Initialize the debug LCD on the RSPI bus. */
    lcd_initialize();
    
    /* Clear LCD */
    lcd_clear();

    cmt_init(); /* Initialize the CMT unit for application timing tasks. */ 
	
	R_SWITCHES_Init(); /* Prepare the board switches for use. */
	
	cmt_callback_set(0, &Blink_Red_LEDs);        /*Blink Red leds at 400ms*/
	cmt_start(0, 37500);
		
	Set_Font_Bitmap();                           /*Display the top gun logo*/
	Set_LCD_Pos(0,0); 
	Set_LCD_Char(0);
	
	Block_Until_Switch_Press();                  /*Call the block until switch press function*/
	cmt_stop(0);                                 /*Stop calling function block until switch press*/
	Reset_All_LEDs();                            /*all Leds should be off*/
	
	Set_Font_8_by_8();
	lcd_clear();
	lcd_display(LCD_LINE3, "PORIPSA");           /*Display names on lcd*/
	lcd_display(LCD_LINE4, "MEGHASHYAM");
	
	cmt_callback_set(0, &Blink_Any_LEDs);        /*Blink random led*/
	cmt_start(0, 18750);                         /*at 200ms*/
	Block_Until_Switch_Press();
	cmt_stop(0);                                 /*Stop calling function when switch is pressed*/
	Reset_All_LEDs();
	

    
    
    /* Executes the initial LED flashing sequence */
    //flash_led();  

    /* Initiates the ADC-varying LED flash sequence */ 
    //timer_adc();

    /* Executes the static variable test */
    //statics_test(); 
	//timer_adc();

    while (1)                                              /*Loop forever*/
    {
		if (g_sw3_press == true && sw3_state ==0)           //check if switch 3 is pressed  
		{
			lcd_clear();
			Set_Font_8_by_8();
			//lcd_clear();
			lcd_display(LCD_LINE1, "PAUSED");              //Pause game
			g_sw3_press = false; 
			sw3_state = 1;	
			All_Red_LEDs_ON();
		}
		if (g_sw3_press == true && sw3_state ==1)        //check state of led 
		{
			lcd_clear();
			g_sw3_press = false;                         //initialize switch 3 
				
			Reset_All_LEDs();                            //Reset leds (make them off)
			
			Set_Font_Bitmap();
			
			timer_adc();                                //call timer_adc function
			
			//Set_LCD_Pos(jani_x,jani_y);
			
			//Set_LCD_Char(3);
			/*while (1)
			{
				//lcd_clear();
			//Create_Obstacle();
			
			Set_LCD_Pos(jani_x,jani_y);
			
			Set_LCD_Char(3);
			
			if (g_sw3_press)
			{
				break;
			}
			}*/
			//sw3_state = 0;
		
		    sw3_state = 0;
 
		}
		
		
        /* All done. Loop here forever. LEDs will continue to flash as 
           at a variable rate as the timer ISR executes. */
	}
	
    
} /* End of function main(). */


/*******************************************************************************
* Function name: statics_test
* Description  : Static variable test routine. The function replaces the
*                contents of the string g_working_string with that of g_replacement_str, one
*                element at a time.
* Arguments    : none
* Return value : none
*******************************************************************************/
void statics_test(void)
{
    /* Declare loop count variable */
    uint8_t loop_counter;

    /* Write the initial contents of g_working_string to the debug LCD */
    lcd_display(LCD_LINE2, g_working_string);

    /* Set up the callback function on cmt channel 0 */   
    cmt_callback_set(CHANNEL_0, &timer_callback);
    
    /* Replace each element of g_working_string in each loop iteration with the contents 
       of g_replacement_str */
    for (loop_counter = 0; loop_counter < strlen((char*)g_replacement_str); loop_counter++)
    {
        /* Replace character from g_working_string with characer from g_replacement_str */
        g_working_string[loop_counter] = g_replacement_str[loop_counter];

        /* Write current contents of g_working_string to the debug LCD */
        lcd_display(LCD_LINE2, g_working_string);
   
        g_delay_complete = false; /* Clear this flag variable. */
        
        /* Use a timer to create the delay between each loop iteration */         
        cmt_start(CHANNEL_0, TIMER_COUNT_600MS); /* 600mS period. */
        
        while(!g_delay_complete)
        {
            /* wait until the delay has completed. */
        }               
    }

    /* Revert the debug LCD back to displaying the MCU_NAME */    
    lcd_display(LCD_LINE2, MCU_NAME);
    
} /* End of function statics_test(). */


/******************************************************************************
* Function name: timer_callback
* Description  : This function is called from the cmt_timer compare-match ISR.
*              : It sets a global flag that is polled for delay loop control.
* Arguments    : None
* Return value : None
******************************************************************************/
void timer_callback(void)
{
    cmt_stop(CHANNEL_0);
    g_delay_complete = true;
    
} /* End of function timer_callback() */



/******************************************************************************
* Function name: sw1_callback
* Description  : Callback function that is executed when SW1 is pressed.
*                Called by sw1_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw1_callback(void)
{
    g_sw1_press = true; /* The switch was pressed. */
} /* End of function sw1_callback() */


/******************************************************************************
* Function name: sw2_callback
* Description  : Callback function that is executed when SW2 is pressed.
*                Called by sw2_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw2_callback(void)
{
    g_sw2_press = true; /* The switch was pressed. */
} /* End of function sw2_callback() */


/******************************************************************************
* Function name: sw3_callback
* Description  : Callback function that is executed when SW3 is pressed.
*                Called by sw3_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw3_callback(void)
{
    g_sw3_press = true; /* The switch was pressed. */
} /* End of function sw3_callback() */

/******************************************************************************
* Function name: Reset_All_LEDs
* Description  : Turns off all LEDs
*                
* Arguments    : none
* Return value : none
******************************************************************************/
void Reset_All_LEDs(void)
{
	PORTD.PODR.BYTE = 0xFF;       /* Turn off all LEDs on port D.*/
    PORTE.PODR.BYTE = 0xFF;      /* Turn off all LEDs on port E.*/
}
//End of Reset_All_LEDs
/******************************************************************************
* Function name: All_Red_LEDs_ON
* Description  : Turns on only red LEDs
*                
* Arguments    : none
* Return value : none
******************************************************************************/
void All_Red_LEDs_ON(void)
{  
	PORTD.PODR.BIT.B0 = 0;//&= ~LED6+LED7+LED10+LED11;       /* Turn on all red LEDs on port D.*/
	PORTD.PODR.BIT.B1 =0;
	PORTD.PODR.BIT.B2 =0;
	//PORTD.PODR.BIT.B3 =0;
	//PORTD.PODR.BIT.B4 =0;
	//PORTD.PODR.BIT.B5 =0;
	PORTD.PODR.BIT.B6 =0;
	PORTD.PODR.BIT.B7 =0;
    PORTE.PODR.BIT.B0 =0;//LED14+LED15+LED6;      /* Turn on all red LEDs on port E.*/
}
//End of function All_Red_LEDs_ON

/******************************************************************************
* Function name: Block_Until_Switch_Press
* Description  : Goes to an endless loop when any of the switches are pressed
*                
* Arguments    : none
* Return value : none
******************************************************************************/
void Block_Until_Switch_Press(void)
{  

g_sw1_press = false;
g_sw2_press = false;                                  //Initialise all switchs
g_sw3_press = false;
	while(1)
	{
		//lcd_display(LCD_LINE6, "in while block");
		//if ((PORT4.PIDR.BIT.B0==0) || (PORT4.PIDR.BIT.B1==0) || (PORT4.PIDR.BIT.B4 == 0))
		if ((g_sw1_press==true)||(g_sw2_press==true)||(g_sw3_press==true))
		/* if any switch is pressed*/
		{
			g_sw1_press = false;
			g_sw2_press = false;
			g_sw3_press = false;
			break;                                         /*break loop*/
		}
		//lcd_display(LCD_LINE7, "blocking");
	}
	//lcd_display(LCD_LINE7, "");
}
//End of Block_Until_Switch_Press

/******************************************************************************
* Function name: Blink_Red_LEDs
* Description  : Keeps track of the red leds's state and blinks it
*                
* Arguments    : none
* Return value : none
******************************************************************************/
void Blink_Red_LEDs(void)
{
	//blink_red_var = 1;
    
	if (blink_red_var == 1)                  //check global variable for state of blinking
	{
		if (blink_red_state)                 //check global variable for toggling
		{
			All_Red_LEDs_ON();                //call function to glow red leds
			blink_red_state = false;
		}
		else if (!(blink_red_state))          //check global variable to toggle
		{
		Reset_All_LEDs();                     //switch off all leds
		blink_red_state = true;
		}
	}
	else if (blink_red_var == 0)
	{
		Reset_All_LEDs();
	}
}
//End of function Blink_Red_LED

/******************************************************************************
* Function name: Blink_Any_LEDs
* Description  : Blinks Random Leds
*                
* Arguments    : none
* Return value : none
******************************************************************************/
void Blink_Any_LEDs(void)
{
	bool blink_leds=true;   //local variable to toggle       

	if (blink_leds)         //check state to toggle 
		{
			PORTD.PODR.BYTE ^= 0XB1;   //glow random led
			blink_leds = false;
		}
		else if (!(blink_leds))        //toggle
		{
		PORTE.PODR.BYTE ^= 0XB1;        //glow random led
		blink_leds = true;
		}
}

/******************************************************************************
* Function name: Create_Obstacle
* Description  : Creates obstacle images on the lcd
*                
* Arguments    : none
* Return value : none
******************************************************************************/

void Create_Obstacle(void)
{ 
	//if (sw3_state = 1)
	//{
	lcd_clear();
	/*g_sw1_press = false;
	g_sw2_press = false;
	//while (1)
	//{*/
	if (g_sw1_press)
	{
		//lcd_clear();
		jani_y = jani_y - 8;
			if (jani_y < 0)
			{
				jani_y = 0;
			}
		//	break;
		g_sw1_press = false;
	}
	//else if (g_sw3_press)
	//{break;}
		
	
	if (g_sw2_press)
	{
		//lcd_clear();
			jani_y = jani_y + 8;
			if (jani_y > 50)
			{
				jani_y = 50;
			}
	//	break;
	g_sw2_press = false;
	}
	/*else if (g_sw3_press)
	{
		break;
	}
	}*/
	//while(1)
	//	{
		   			if(!enemy_flag)   
   			{ //create enemy plane    
   			obs_x = 80;  //spawn on the other side of the screen    
   			obs_y = 8*(rand() % 8); 
   			    enemy_flag = 1; 
     			}
     			else
     			{    
     			if(obs_x <= 23) //check for collision 
     			{     
     			if((obs_y <= (jani_y + 16)) && (obs_y >= jani_y)) 
     			{ 
     			enemy_flag = 0; //respawn enemy
    			 hit_flag = 1;
     			}    
     			}    
     			if(obs_x == 0) 
     			enemy_flag = 0;    
     			else    
     			{
     			obs_x = obs_x - 5;     
      			if(hit_flag ==0)
     			{
    			 Set_LCD_Pos(obs_x, obs_y);      
    			 Set_LCD_Char(3); //draw jet
				 Set_LCD_Pos(jani_x,jani_y);
     			Set_LCD_Char(1); 
				      
     			}     
     			else     
     			{      
     			Set_LCD_Pos(obs_x,obs_y);
     			Set_LCD_Char(3); 
    			 Set_LCD_Pos(jani_x, jani_y);  
    			 Set_LCD_Char(7);   
    			 hit_flag = 0; 
     			}  
     			}   
     			} 
			//	if  (g_sw3_press)
			//	{break;}
	//	}
	
	
	
}

