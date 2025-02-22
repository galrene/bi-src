/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Application includes. */
#include "displej.h"
#include "stack.h"
#include "led.h"

#include <stdio.h>

/*-----------------------------------------------------------*/

/* PIC24F config */
/* Run on 16MHz */
#pragma config JTAGEN=OFF, GCP=OFF, GWRP=OFF, FWDTEN=OFF, ICS=PGx2, \
               IESO=OFF, FCKSM=CSDCMD, OSCIOFNC=OFF, POSCMOD=HS, \
               FNOSC=PRIPLL, PLLDIV=DIV3, IOL1WAY=ON

/* Hardware configuration function declaration. */
static void prvSetupHardware ( void );
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char *pcTaskName );

void vWaterMarkTask ( void );

/*-----------------------------------------------------------*/
/**
 * Task1
 *   analyticky <=40B
 *   nahodily pristup 62W
 *   Experimentalny: 62W   
 * Task2
 *  analyticky 76B
 *  nahodily pristup 105W
 *  expereminatalny: 105W
 * 
 * BONUS:
 * Experimentalny: T1: 62, T2: 105
 * Nahodily pristup: rovnako ako exp
 */

TaskHandle_t task1Handle;
TaskHandle_t task2Handle;

/* Create the tasks and start the scheduler. */
int main ( void ) {
    /* Configure hardware. */
    prvSetupHardware();
    
    /* Static Task stacks */
    #define T1_STACK_SIZE 62
    #define T2_STACK_SIZE 105
    StackType_t xStack1 [ T1_STACK_SIZE ];
    StackType_t xStack2 [ T2_STACK_SIZE ];
    
    StaticTask_t xTaskBuffer1;
    StaticTask_t xTaskBuffer2;
    
    /* Create the task. */
    task1Handle = xTaskCreateStatic (
            vStackTask1,
            ( const char * ) "ST 1",
            T1_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY + 1,
            xStack1,
            &xTaskBuffer1 ); 
    // xTaskCreate ( vStackTask1,
    //               ( const char * ) "ST 1",
    //               T1_STACK_SIZE,
    //               NULL,
    //               tskIDLE_PRIORITY + 1,        
    //               &task1Handle );
    task2Handle = xTaskCreateStatic (
            vStackTask2,
            ( const char * ) "ST 2",
            T2_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY + 1,
            xStack2,
            &xTaskBuffer2 );
    // xTaskCreate ( vStackTask2,
    //               ( const char * ) "ST 2",
    //               T2_STACK_SIZE,
    //               NULL,
    //               tskIDLE_PRIORITY + 1,
    //               &task2Handle );


    xTaskCreate ( vDisplayGatekeeperTask,
                  ( const char * ) "DGKT",
                  2*configMINIMAL_STACK_SIZE,
                  NULL,
                  tskIDLE_PRIORITY + 2,
                  NULL );
    
    xTaskCreate( vWaterMarkTask,
                ( const char * ) "DEBUG",
                3*configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL );

    /* Start the scheduler. */
    vTaskStartScheduler();
    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    return 0;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/
/*
 * configCHECK_FOR_STACK_OVERFLOW is set to 1, vApplicationStackOverflowHook
 * implementation required to define what happens when stack overflows
 */
void vApplicationStackOverflowHook ( TaskHandle_t xTask,
                                    char *pcTaskName ) {
    vLightLED ( LED_R, 1 );
    while ( 1 );
}


/* Hardware configuration function definition. */
static void prvSetupHardware ( void ) {
    vInitLED ();
    vDisplayInit();
    vLEDAllOff();
}

void vWaterMarkTask ( void ) {
    while ( 1 ) {
        UBaseType_t waterMark1 = uxTaskGetStackHighWaterMark ( task1Handle );
        UBaseType_t waterMark2 = uxTaskGetStackHighWaterMark ( task2Handle );
        
        char buffer [32]= {0};
        size_t strLen = snprintf ( buffer, sizeof(buffer), "free[T1:%dW T2:%dW]", waterMark1, waterMark2 );
        vDisplayPutString ( buffer, strLen );
        // fill line with whitespace
        if ( strLen < 21 )
            for ( size_t i = strLen; i < 21; i++ )
                vDisplayPutString ( " ", 1 );
        vTaskDelay ( 1000 / portTICK_PERIOD_MS );
    }
}

/*-----------------------------------------------------------*/
