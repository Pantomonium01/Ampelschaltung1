#define STM32401xE
#include <stm32f4xx.h>
#include <gpio.h>







// Timer
void delay_ms(uint32_t ms)
{
    ms = ms * 16000 / 4;
    for (; ms > 0; ms--)
    {
        __asm volatile("nop");
    }
}


//Globale Variable für abfrage Status
int ampelState = 0;


int main(void)
{
    /* SETUP */

    /* SETUP */
    __disable_irq(); // deaktivieren alle Interrupts global



    // Aktiviere den Clock für die GPIO - Ports
    gpio_enable_port_clock(GPIOA);
    gpio_enable_port_clock(GPIOB);



    // Setze die Pins für die LEDs auf "Output"-Modus
    gpio_set_pin_mode(GPIOA,0,OUTPUT); //Output für Rote LED
    gpio_set_pin_mode(GPIOA,1,OUTPUT); //Output für Gelbe LED
    gpio_set_pin_mode(GPIOA,4,OUTPUT); //Output für Grüne LED

    // Setze den Pin 0 von GPIOB auf "Input"-Modus
    gpio_set_pin_mode(GPIOB, 0, INPUT); // Input für Taster 

    /* Enable the clock for SYSCFG */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* Select gpio port b pin0 as external interrupt */
    // SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;    // Clear EXTI13 bits
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;  // Set EXTI0 bits to PB 
    //exticr[array0weilpin0] |= syscfg_exticr1(weilRegister1)exti0(weilPin0)_PB (weil Port B)

//anpassen
        /* Enable EXTI for line 0 (PB0) */
    EXTI->IMR |= EXTI_IMR_MR0;     // Enable EXTI interrupt
    
    EXTI->RTSR |= EXTI_RTSR_TR0;   // Enable rising edge trigger

    /* Enable and set the EXTI interrupt in NVIC */
    NVIC_EnableIRQ(EXTI0_IRQn);
    // NVIC_SetPriority(EXTI0_IRQn, 0);

    // Enable global interrupts
    __enable_irq();


    // Variable zur Speicherung des vorherigen Zustands des Buttons
    int previousButtonState = 1;  // Annahme: Nicht gedrückt zu Beginn



    // Endlosschleife
    while (1)
    {
        // Überprüfe, ob der Button gedrückt ist. Beachte: Der Button ist low aktiv (deswegen negiert)!
        // int currentButtonState = !(GPIOC->IDR & GPIO_IDR_ID13);

        // Zustandsautomat für die Ampel
        if (ampelState == 0) // Aus
        {
            //Pins setzen/rücketzen zum LED an/ausschalten
            gpio_reset_pin(GPIOA,0); // Rot ausschalten
            gpio_reset_pin(GPIOA,1); // Gelb ausschalten
            gpio_reset_pin(GPIOA,4); // Grün ausschalten
        }
        else if (ampelState == 1) // Rot
        {
            gpio_set_pin(GPIOA,0); // Rot einschalten
            gpio_reset_pin(GPIOA,1); // Gelb ausschalten
            gpio_reset_pin(GPIOA,4); // Grün ausschalten
            delay_ms(2000);
            ampelState = 2; // Rot -> Rot-Gelb
        }
        else if (ampelState == 2) // Rot-Gelb
        {
            gpio_set_pin(GPIOA,0); // Rot einschalten
            gpio_set_pin(GPIOA,1); // Gelb einschalten
            gpio_reset_pin(GPIOA,4); // Grün ausschalten
            delay_ms(1000);
            ampelState = 3; // Rot-Gelb -> Grün
        }
        else if (ampelState == 3) // Grün
        {
            gpio_reset_pin(GPIOA,0); // Rot ausschalten
            gpio_reset_pin(GPIOA,1); // Gelb aussschalten
            gpio_set_pin(GPIOA,4); // Grün einschalten
            delay_ms(5000);
            ampelState = 4; // Grün -> Gelb
        }
        else if (ampelState == 4) // Gelb
        {
            gpio_reset_pin(GPIOA,0); // Rot ausschalten
            gpio_set_pin(GPIOA,1); // Gelb einschalten
            gpio_reset_pin(GPIOA,4); // Grün ausschalten
            delay_ms(1000);
            ampelState = 5; //ampel aus
        }
        else if (ampelState == 5) // Gelb
        {
            gpio_set_pin(GPIOA,0); // Rot einschalten
            gpio_reset_pin(GPIOA,1); // Gelb ausschalten
            gpio_reset_pin(GPIOA,4); // Grün ausschalten
            delay_ms(1000);
            ampelState = 0; //ampel aus
        }

        // Überprüfe, ob der Button von nicht gedrückt auf gedrückt wechselt
        //if (!previousButtonState && currentButtonState)
        // {
            // Drückt der Fußgänger auf einen Taster, springt die Ampel an (Rot).
        //    ampelState = 1;
        //}

        // Speichere den aktuellen Zustand des Buttons als vorherigen Zustand
        //previousButtonState = currentButtonState;
    }
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0)
    {
        //Handle button press
        ampelState = 1;

        //Clear interrupt flag
        EXTI->PR |= EXTI_PR_PR0;

       }


}
