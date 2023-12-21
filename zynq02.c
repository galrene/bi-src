/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"

#define SA 1 << 0
#define SB 1 << 1
#define SC 1 << 2
#define SD 1 << 3
#define SE 1 << 4
#define SF 1 << 5
#define SG 1 << 6
#define DP 1 << 7
#define D0 1 << 8
#define D1 1 << 9
#define D2 1 << 10
#define D3 1 << 11


unsigned int digit_masks[] = {
    SA | SB | SC | SD | SE | SF,      // 0
    SB | SC,                          // 1
    SA | SB | SD | SE | SG,           // 2
    SA | SB | SC | SD | SG,           // 3
    SB | SC | SF | SG,                // 4
    SA | SC | SD | SF | SG,           // 5
    SA | SC | SD | SE | SF | SG,      // 6
    SA | SB | SC,                     // 7
    SA | SB | SC | SD | SE | SF | SG, // 8
    SA | SB | SC | SD | SF | SG,      // 9
    SA | SB | SC | SE | SF | SG,      // A
    SC | SD | SE | SF | SG,           // B
    SA | SD | SE | SF,                // C
    SB | SC | SD | SE | SG,           // D
    SA | SD | SE | SF | SG,           // E
    SA | SE | SF | SG                 // F
};

void display( int num ) {
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, digit_masks[num]);
}

void disp_nums ( int pos0, int pos1, int pos2, int pos3 ) {
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D3 | digit_masks[pos0]);
	usleep(5000);
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D2 | digit_masks[pos1]);
	usleep(5000);
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D1 | digit_masks[pos2]);
	usleep(5000);
	XGpio_DiscreteWrite(&disp_gpio, DISP_CHANNEL, D0 | digit_masks[pos3]);
	usleep(5000);
}

/* The Tx and Rx tasks as described at the top of this file. */
static void prvTxTask(void *pvParameters);
static void prvRxTask(void *pvParameters);
/*-----------------------------------------------------------*/

/* The queue used by the Tx and Rx tasks, as described at the top of this
file. */
static QueueHandle_t xQueue = NULL;

#define QUEUE_LENGTH 5
#define QUEUE_ITEM_SIZE 32

int main ( void )
{
    xil_printf("Hello from Freertos example main\r\n");

    xTaskCreate( display_task,               /* The function that implements the task. */
                 (const char *) "DISP",      /* Text name for the task, provided to assist debugging only. */
                 configMINIMAL_STACK_SIZE,   /* The stack allocated to the task. */
                 NULL,                       /* The task parameter is not used, so set to NULL. */
                 configMAX_PRIORITIES + 1,   /* The task runs at the idle priority. */
                 NULL );

    /* Create the queue used by the tasks.  The Rx task has a higher priority
    than the Tx task, so will preempt the Tx task and remove values from the
    queue as soon as the Tx task writes to the queue - therefore the queue can
    never have more than one item in it. */
    xQueue = xQueueCreate( QUEUE_LENGTH,
                           QUEUE_ITEM_SIZE);
    /* Check the queue was created. */
    configASSERT(xQueue);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;)
        ;
}

/*-----------------------------------------------------------*/
void display_task ( void )
{
    
    for (;;)
    {
        unsigned int rec_item;
        xQueueReceive ( xQueue,          /* The queue being read. */
                        rec_item,        /* Data is read into this address. */
                        portMAX_DELAY ); /* Wait time for data data. */
        unsigned int recNumLo = rec_item & 0xF;        // low 4 bits
        unsigned int recNumHi = (rec_item >> 4) & 0xF; // high 4 bits

        disp_nums ( 0x00, 0x00, recNumHi, recNumLo );
        xil_printf("%02X\n", rec_item );
    }
}

/*-----------------------------------------------------------*/

void button_handler ( void )
{
    XGpio_InterruptClear(&but_gpio, BUT_CHANNEL);
    unsigned int readVal = XGpio_DiscreteRead(&but_gpio, BUT_CHANNEL);
    xQueueSend( xQueue,   /* The queue being written to. */
                &readVal, /* The address of the data being sent. */
                0UL);     /* The block time. */
}

void switch_handler ( void )
{
    XGpio_InterruptClear(&sw_gpio, SWITCH_CHANNEL);
	unsigned int readVal = XGpio_DiscreteRead(&sw_gpio, SWITCH_CHANNEL);
    xQueueSend( xQueue,   /* The queue being written to. */
                &readVal, /* The address of the data being sent. */
                0UL);     /* The block time. */
}

void init_peripherals() {
	// init leds
    XGpio_Initialize(&led_gpio, XPAR_AXI_GPIO_LED_DISP_DEVICE_ID);
	XGpio_SetDataDirection(&led_gpio, LED_CHANNEL,  0x00 );
    // init display
    XGpio_Initialize(&disp_gpio, XPAR_AXI_GPIO_LED_DISP_DEVICE_ID);
	XGpio_SetDataDirection(&disp_gpio, DISP_CHANNEL, 0x00 );
    // init buttons
	XGpio_Initialize(&but_gpio, XPAR_AXI_GPIO_BTNS_8BITS_DEVICE_ID);
	XGpio_SetDataDirection(&but_gpio, BUT_CHANNEL, 0xFF );
    // init switches
	XGpio_Initialize(&sw_gpio, XPAR_AXI_GPIO_SWS_12BITS_DEVICE_ID);
	XGpio_SetDataDirection(&sw_gpio, SWITCH_CHANNEL, 0xFFF );
}


/*globální proměnná XScuGic kterou používá RTOS*/
extern XScuGic xInterruptController;

/*Funkce která nastaví přerušení a následně přidá k přerušení RTOSu*/
static int prvSetupHardware(void)
{
    int Status = XST_FAILURE;
    init_peripherals();
    // Disable the interrupts
    portDISABLE_INTERRUPTS();

    XScuGic_Config *pxGicConfig = NULL;
    xil_printf("ScuGic device setup!\r\n");
    pxGicConfig = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);

    configASSERT(pxGicConfig);

    Status = XScuGic_CfgInitialize(&xInterruptController, pxGicConfig, pxGicConfig->CpuBaseAddress);

    if (Status != XST_SUCCESS)
    {
        xil_printf("Setup scugic device failure!\r\n");
        return XST_FAILURE;
    }

    XScuGic_Connect(&xInterruptController, XPAR_FABRIC_AXI_GPIO_BTNS_8BITS_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler)button_handler, NULL);
    XScuGic_Enable(&xInterruptController, XPAR_FABRIC_AXI_GPIO_BTNS_8BITS_IP2INTC_IRPT_INTR);

    XScuGic_Connect(&xInterruptController, XPAR_FABRIC_AXI_GPIO_SWS_12BITS_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler)switch_handler, NULL);
    XScuGic_Enable(&xInterruptController, XPAR_FABRIC_AXI_GPIO_SWS_12BITS_IP2INTC_IRPT_INTR);

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 &xInterruptController);
    Xil_ExceptionEnable();

    vPortInstallFreeRTOSVectorTable();

    return XST_SUCCESS;
}
/**
 * TODO:
 * Vymysliet ako rozhodovat ci bolo stlacene tlacitko alebo prepinac
 * a podla toho kde na displeji zobrazit hodnotu.
 */


/**
   - Vytvořte obsluhu přerušení pro tlačítka a přepínače.
   - Z předchozí úlohy předělejte obsluhu 7-segmentového displeje pro RTOS.
     Pro komunikaci s taskem který bude obsluhovat displej použijte frontu.
   - Při přepnutí přepínačů SW7-SW0 zobrazte jejich aktuální stav na spodních dvou
     7-segmentech displeje.
   - Při stisknutí tlačítka zobrazte na zbývajících dvou 7-segmentech displeje
     jeho bitovou hodnotu (BTN0 = 1, BTN1 = 2 …​).
   - Vždy při stisknutí tlačítka vypište na sériovou linku, které tlačítko
     bylo zmáčknuto. Výpis je pomalá funkce, proto ji nesmíte provádět přímo
     v obsluze přerušení! Informaci o stavu tlačítek si ukládejte do
     fronty a samotný výpis provádějte taskem na této frontě blokujícím.
 */