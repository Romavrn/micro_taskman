# micro_taskman
It is a very simple and small task scheduler.

This scheduler was designed for very simple tasks. In order not to write code every time if (millis > ...) then.... Everything is already written here and it is completely easy to work with.

The scheduler supports several functions:
* Adding/removing tasks
* Non-blocking time exposures
* Single timers add/remove

For normal operation, the tmUpdate function must be placed in the main function loop. To count the ticks, call the tmTick function with a frequency of 1 ms.
