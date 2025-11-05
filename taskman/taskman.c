#include "taskman.h"

// Array with tasks
static Task_s 			tasks[MAX_TASKS];

#if MAX_TIMERS
// Array with timers
static OneShotTimer_s 	timers[MAX_TIMERS];
#endif // MAX_TIMERS

static volatile uint32_t millis;


/*
 * Custom idle function
 * It can be redefined in the right place.
 */
__attribute__((weak)) void sIdleTask(void) {
    ///__WFI(); 														//Switching to sleep until the next SysTick interrupt (optimization)
}

uint32_t get_millis (void) {
    return millis;
};

int8_t tmAddTask(void (*func)(void), uint32_t period_ms) {
    for (int i = 0; i < MAX_TASKS; i++) {
        //Search for a free slot in the array
        if (tasks[i].taskFunc == 0) {
            tasks[i].taskFunc = func;
            tasks[i].period_ms = period_ms;
            tasks[i].delay_ms = period_ms;
            tasks[i].isReady = 0;
            return i;
        }
    }
    return -1;
}

int8_t tmUpdateTask(void (*func)(void), uint32_t period_ms) {
    for (int i = 0; i < MAX_TASKS; i++) {
        //Search for a free slot in the array
        if (tasks[i].taskFunc == func) {
            tasks[i].period_ms = period_ms;
            tasks[i].delay_ms = period_ms;
            tasks[i].isReady = 0;
            return 0;
        }
    }
    return -1;
}

int8_t tmDeleteTask(void (*func)(void)) {
    for (int i = 0; i < MAX_TASKS; i++) {
        //Search for a func slot in the array
        if (tasks[i].taskFunc == func) {
            tasks[i].taskFunc = 0;
            return 0;
        }
    }
    return -1;
}

void tmTick(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].taskFunc) {
            if (tasks[i].delay_ms > 0) {
                tasks[i].delay_ms--;
                if (tasks[i].delay_ms == 0) {
                    tasks[i].isReady = 1;
                    tasks[i].delay_ms = tasks[i].period_ms;
                }
            }
        }
    }

#if MAX_TIMERS
    tmTimerProcess();
#endif // MAX_TIMERS

    millis++;
}

void tmUpdate(void) {
	static uint8_t taskExecuted = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (tasks[i].taskFunc && tasks[i].isReady) {
			tasks[i].isReady = 0;
			tasks[i].taskFunc();
			taskExecuted = 1;
		}
	}
	if (!taskExecuted) {
        // nothing needs to be done — we go into idle mode
		sIdleTask();
	}
}

/**
 * @brief Non-blocking delay (similar to delay_ms, but does not slow down the CPU)
 * 
 * @param timestamp 
 * @param delay 
 * @return true 
 * @return false 
 */
bool tmDelay_ms(uint32_t* timestamp, uint32_t delay) {
    if (millis - *timestamp >= delay) {
        *timestamp = millis;
        return true;
    }
    return false;
}

#if MAX_TIMERS
/**
 * @brief The timer will start once, work by starting the task after a set time, and turn off.
 * 1. Iterating through all timers
 * 2. Checking whether this timer was created earlier
 * 3. If a timer with this function has been created, we adjust the time regardless of the timer 
 * status, because it is possible that a different time period was set.
 * 4. If created, check whether it is active
 * 5. If not active, start the timer,
 * 6. If the timer is already active, exit the function
 */
int8_t tmTimerStartOnce(uint32_t delay_ms, void (*func)(void)) {
	for (int i = 0; i < MAX_TIMERS; i++) {
		if (timers[i].callback == func)	{
			timers[i].delay = delay_ms;
			if (!timers[i].active) {
				timers[i].active = 1;
				timers[i].start_time = millis;
			}
			return 0;
		}
	}
/**
 * 7. If the timer has not been created yet, then create a new timer.
 * 
 */
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].callback == 0) {
            timers[i].active = 1;
            timers[i].start_time = millis;
            timers[i].delay = delay_ms;
            timers[i].callback = func;
            return 0;
        }
    }
    return -1;
}

int8_t tmTimerDelete(void (*func)(void)) {
	for (int i = 0; i < MAX_TIMERS; i++) {
		if (timers[i].callback == func)	{
			timers[i].callback = 0;
			return 0;
		}
	}
    return -1;
}

void tmTimerProcess(void) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].active && (millis - timers[i].start_time >= timers[i].delay)) {
            timers[i].active = 0;
            if (timers[i].callback) timers[i].callback();
        }
    }
}
#endif // MAX_TIMERS

