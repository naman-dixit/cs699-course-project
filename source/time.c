/**
 * These functions are used for various timing related operations, the output of which is
 * then used in various synchronized actrivities such as animation. In the current program,
 * the only place we make use of timing is during the blinking of cursor.
 *
 * @file time.c
 * @author Team Octal
 * @brief Functions for timing
 */

/**
* @brief Function to compute elapsed time
*
* Given start time, this function count the total elapsed time from the start time.
*
* @param last_counter From when to start counting
*
* @return Time passed since @p last_counter
*/
internal_function
F64 timeMicrosecondsElapsed (U64 *last_counter)
{
    U64 freq = SDL_GetPerformanceFrequency();
    U64 new_counter = SDL_GetPerformanceCounter();
    U64 counter_gap = new_counter - (*last_counter);
    F64 time_gap = ((1000000.0 * (F64)counter_gap) / (F64)freq);
    *last_counter = new_counter;
    return time_gap;
}
