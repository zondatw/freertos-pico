#include <FreeRTOS.h>
#include <stdbool.h>
#include <stdio.h>
#include <task.h>
#include "pico/stdlib.h"

void led_task(void *p);
void msg_task(void *p);
void delay_task(void *p);
void monitor_task(void *p);

void vApplicationIdleHook(void);
void init_timer1_for_runtime_state(void);

void preSleepProcessing(uint32_t ulExpectedTime);
void postSleepProcessing(uint32_t ulExpectedTime);

#define MAX_TASK_NUM 10
TaskStatus_t pxTaskStatusArray[MAX_TASK_NUM];
volatile unsigned long ulHighFrequencyTimerTicks = 1000;

struct led_params {
    int gpio_pin;
    int on_delay;
    int off_delay;
};

void get_task_state(void)
{
    const char task_state[] = {'r', 'R', 'B', 'S', 'D'};
    UBaseType_t uxArraySize, x;
    unsigned portSHORT ulStatsAsPercentage;
    uint32_t ulTotalRunTime;

    /* 获取任务总数目 */
    uxArraySize = uxTaskGetNumberOfTasks();
    if (uxArraySize > MAX_TASK_NUM) {
        printf("Too many task！\n");
    }

    uxArraySize =
        uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

    printf(
        "Task            status       ID       Pirority       stack        CPU\n");

    /* 避免除零错误 */
    if (ulTotalRunTime > 0) {
        for (x = 0; x < uxArraySize; x++) {
            char tmp[256];

            ulStatsAsPercentage =
                (uint16_t) (pxTaskStatusArray[x].ulRunTimeCounter) * 100 /
                ulTotalRunTime;

            if (ulStatsAsPercentage > 0UL) {
                sprintf(tmp, "%-25s%-10c%-10lu%-12lu%-12d%d%%",
                        pxTaskStatusArray[x].pcTaskName,
                        task_state[pxTaskStatusArray[x].eCurrentState],
                        pxTaskStatusArray[x].xTaskNumber,
                        pxTaskStatusArray[x].uxCurrentPriority,
                        pxTaskStatusArray[x].usStackHighWaterMark,
                        ulStatsAsPercentage);
            } else {
                /* 任务运行时间不足总运行时间的1%*/
                sprintf(tmp, "%-25s%-10c%-10lu%-12lu%-12dt<1%%",
                        pxTaskStatusArray[x].pcTaskName,
                        task_state[pxTaskStatusArray[x].eCurrentState],
                        pxTaskStatusArray[x].xTaskNumber,
                        pxTaskStatusArray[x].uxCurrentPriority,
                        pxTaskStatusArray[x].usStackHighWaterMark);
            }
            printf("%s\n", tmp);
        }
    }
    printf("Task state:   r-running  R-ready  B-block  S-suspend  D-delete\n");
}

int main()
{
    stdio_init_all();
    struct led_params params = {
        .gpio_pin = 9,
        .on_delay = 100,
        .off_delay = 100,
    };

    xTaskCreate(led_task, "LED Task 1", 256, &params, 1, NULL);
    xTaskCreate(msg_task, "msg Task 1", 256, NULL, 1, NULL);
    xTaskCreate(delay_task, "delay Task 1", 256, NULL, 1, NULL);
    xTaskCreate(monitor_task, "monitor Task 1", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (true)
        ;
}

void led_task(void *p)
{
    struct led_params *params = (struct led_params *) p;

    gpio_init(params->gpio_pin);
    gpio_set_dir(params->gpio_pin, GPIO_OUT);

    while (true) {
        gpio_put(params->gpio_pin, true);
        printf("[LED] GPIO PIN %d ON\n", params->gpio_pin);
        vTaskDelay(pdMS_TO_TICKS(params->on_delay));

        gpio_put(params->gpio_pin, false);
        printf("[LED] GPIO PIN %d OFF\n", params->gpio_pin);
        vTaskDelay(pdMS_TO_TICKS(params->off_delay));
    }
}


void vApplicationIdleHook(void)
{
    /*
    set configUSE_IDLE_HOOK to 1
    */
    static int32_t count = 0;
    count++;
    if (count == 1000) {
        printf("[IDLE] HOOK\n");
        count = 0;
    }
}

void preSleepProcessing(uint32_t ulExpectedTime)
{
    /*
    set configUSE_TICKLESS_IDLE to 1
    */
    static int32_t count = 0;
    count++;
    if (count == 1000) {
        printf("[SLEEP] pre: %d\n", ulExpectedTime);
        count = 0;
    }
}

void postSleepProcessing(uint32_t ulExpectedTime)
{
    /*
    set configUSE_TICKLESS_IDLE to 1
    */
    static int32_t count = 0;
    count++;
    if (count == 1000) {
        printf("[SLEEP] post: %d\n", ulExpectedTime);
        count = 0;
    }
}


void msg_task(void *p)
{
    while (true) {
        printf("[MSG] start\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void delay_task(void *p)
{
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void monitor_task(void *p)
{
    static int32_t count = 0;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        count++;
        if (count == 5) {
            printf("[LED] HOOK\n");
            get_task_state();
            count = 0;
        }
    }
}