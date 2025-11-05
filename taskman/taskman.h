#ifndef INC_TASKMAN_H_
#define INC_TASKMAN_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * @brief The maximum number of tasks. The higher the number, the 
 * more memory is allocated to store the task parameters. 255 is the 
 * maximum number.
 * 
 */
#define MAX_TASKS 10

/**
 * @brief The maximum number of timers. 0 - timers are not activated. 
 * 255 is the maximum number.
 * 
 */
#define MAX_TIMERS 5

/**
 * @brief Task parameter storage structure
 * 
 */
typedef struct {
    void (*taskFunc)(void);
    uint32_t period_ms; 
    uint32_t delay_ms; 
    uint8_t isReady;
} Task_s;

#if MAX_TIMERS
/**
 * @brief The structure of timer parameter storage
 * 
 */
typedef struct {
    uint8_t active;
    uint32_t start_time;
    uint32_t delay;
    void (*callback)(void);
} OneShotTimer_s;
#endif // MAX_TIMERS

/*
 *  Добавление новой задачи
 */

/**
 * @code{c}
 * int8_t tmAddTask(
 *                  void (*func)(void), 
 *                  uint32_t period_ms
 *                  );
 * @endcode
 *
 * The procedure is designed to add a new task to the taskmanager.
 *
 * @param (*func)(void) procedure to add to the procedure startup list
 *
 * @param period_ms the start period of the procedure.
 *
 * @return The returned parameter is the sequential number of the task in the
 * task list, or -1 if it was added unsuccessfully.
 *
 * Example usage:
 * @code{c}
 * void vTaskCount( void ) {
 *  static counter = 0;
 *  counter++;
 * }
 *
 * void vTaskLed( void ) {
 *  led_blink();
 * }
 *
 * void main {
 *  tmAddTask(vTaskCount, 10);
 *  tmAddTask(vTaskLed, 500);
 * 
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */

int8_t tmAddTask(void (*func)(void), uint32_t period_ms);

/**
 * @code{c}
 * int8_t tmUpdateTask(
 *                     void (*func)(void), 
 *                     uint32_t period_ms
 *                     );
 * @endcode
 *
 * Procedure for updating the task call time.
 *
 * @param (*func)(void) procedure to add to the procedure startup list
 *
 * @param period_ms the start period of the procedure.
 *
 * @return The returned parameter for a successful update is 0 or -1 for
 * an unsuccessful addition.
 *
 * Example usage:
 * @code{c}
 * void vTaskLed( void );
 *  led_blink();
 * }
 *
 * void vTaskKey( void ) {
 *  if (key_press) 
 *   tmUpdateTask(vTaskLed, 200);
 * }
 *
 * void main {
 *  tmAddTask(vTaskKey, 100);
 *  tmAddTask(vTaskLed, 500);
 * 
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */
int8_t tmUpdateTask(void (*func)(void), uint32_t period_ms);


/**
 * @code{c}
 * int8_t tmDeleteTask(
 *                     void (*func)(void), 
 *                     );
 * @endcode
 *
 * Procedure for updating the task call time.
 *
 * @param (*func)(void) procedure to add to the procedure startup list
 *
 * @param period_ms the start period of the procedure.
 *
 * @return The returned parameter for a successful delete is 0 or -1 for
 * an unsuccessful delete.
 *
 * Example usage:
 * @code{c}
 * void vTaskKey( void ) {
 *  if (key_press) 
 *   if (tmDeleteTask(vTaskKey) == 0)
 *    printf("Procedure key scan deleted");
 * }
 *
 * void main {
 *  tmAddTask(vTaskKey, 100);
 * 
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */
int8_t tmDeleteTask(void (*func)(void));

/**
 * @code{c}
 * void tmTick(void);
 * @endcode
 *
 * The tick processing procedure is called from the SysTick_Handler. 
 * You need to put the procedure in the SysTick_Handler in the stm32fxxx_it
 * file. Or create a timer with a 1 ms call and place a tmTick call in the 
 * timer's callback.
 * This task starts on an interrupt, so it doesn't start anything and doesn't
 * waste time. If timers are activated, they are started from this task, so 
 * there is no need to place code in the timers that delays the execution of 
 * the program. Activate the flags in the timers
 *
 * @param The parameters do not need to be transmitted.
 *
 * @return The function returns nothing.
 *
 * Example usage:
 * @code{c}
 * for CH32V**
 * void SysTick_Handler(void) __attribute__((interrupt));
 * void SysTick_Handler(void) {
 *  SysTick->SR = 0;
 *  tmTick();
 * }
 *
 * for STM32
 * void SysTick_Handler(void) {
 *  HAL_IncTick();
 *  tmTick();
 * }
 *
 * void main {
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */
void tmTick(void);

/**
 * @code{c}
 * void tmUpdate(void);
 * @endcode
 *
 * All tasks are started from this function if their time has come to 
 * completion. If you don't need to do anything, sIdleTask starts. This 
 * way you can track the workload or execute other code while there are 
 * no tasks to complete.
 *
 * @param The parameters do not need to be transmitted.
 *
 * @return The function returns nothing.
 *
 * Example usage:
 * @code{c}
 * void main {
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */
void tmUpdate(void);

/**
 * @code{c}
 * bool tmDelay_ms(
 *                 uint32_t* timestamp, 
 *                 uint32_t delay
 *                 );
 * @endcode
 *
 * Non-blocking time delay. It does not block the code and is triggered
 * only when the set time has arrived.
 *
 * @param timestamp The variable in which the countdown will be stored.
 *
 * @param delay The time delay after which the trigger is required.
 *
 * @return The trigger parameter. A signal that time is up and something 
 * can be done.
 *
 * Example usage:
 * @code{c}
 * void main {
 *  uint32_t tmp = 0;
 * 
 *  for ( ; ; ) {
 *   if (tmDelay_ms(&tmp, 300)) ToggleBlink();
 *  }
 * }
 * @endcode
 */
bool tmDelay_ms(uint32_t* timestamp, uint32_t delay);

#if MAX_TIMERS
/**
 * @code{c}
 * void tmTimerStartOnce(
 *                       uint32_t delay_ms, 
 *                       void (*func)(void)
 *                       );
 * @endcode
 *
 * One-time timer start. The timer will start once, work by starting 
 * the task after a set time, and turn off.
 * Calling the procedure again for the created timer will update the 
 * procedure call time, with the countdown updated.
 *
 * @param delay_ms The time after which the procedure will start after
 * the timer is started
 *
 * @param (*func)(void) A task that will be run once
 *
 * @return If the timer is successfully created or updated, the function
 * returns 0, if the error is -1.
 *
 * Example usage:
 * @code{c}
 * void vTaskLedOff( void );
 *  led_off();
 * }
 *
 * void vTaskLedFlash( void ) {
 *  led_on();
 *  tmTimerStartOnce(20, vTaskLedOff);
 * }
 *
 * void main {
 *  tmAddTask(vTaskLedFlash, 500);
 * 
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */
int8_t tmTimerStartOnce(uint32_t delay_ms, void (*func)(void));

/**
 * @code{c}
 * void tmTimerDelete(
 *                       void (*func)(void)
 *                       );
 * @endcode
 *
 * Removing a timer from the timer list.
 *
 *
 * @param (*func)(void) A task that will be run once
 *
 * @return If the timer is successfully deleted, the function
 * returns 0, if the error is -1.
 *
 * Example usage:
 * @code{c}
 * void vTaskLed( void ) {
 *  ToggleBlink();
 * }
 * 
 * void ProgrammEnd( void );
 *  tmTimerDelete()
 *  ...
 * }
 *
 * void main {
 *  tmAddTask(vTaskLed, 500);
 *  tmTimerStartOnce(1400, vTaskLed);
 * 
 *  for ( ; ; ) {
 *   tmUpdate();
 *  }
 * }
 * @endcode
 */
int8_t tmTimerDelete(void (*func)(void));

/**
 * @brief Internal timer processing function
 * 
 */
void tmTimerProcess(void);
#endif // MAX_TIMERS

/**
 * @brief Taking the current millisecond parmeter
 * 
 * @return uint32_t 
 */
uint32_t get_millis (void);



#endif // INC_TASKMAN_H_
