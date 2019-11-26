/**
 * These functions are used in debugging any runtime errors that maybe otherwise
 * be hard to fix. Usually, these functions are used in tandem with logging system.
 *
 * @file debug.c
 * @author Team Octal
 * @brief Functions for debugging
 */

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_FRAMES 64

internal_function
int debug_AddressToLine(void *addr)
{
    char addr2line_cmd[512] = {0};
    sprintf(addr2line_cmd,"addr2line -f -p -e %.256s %p", global_program_name, addr);

    /* This will print a nicely formatted string specifying the
       function and source line of the address */
    return system(addr2line_cmd);
}

/**
* @brief Function to print stack trace for debugging
*
* This function print the stack trace for the current execution state of program. It skip the
* first couple of stack frames and also skip the last frame as it usually contains junk.
*/
internal_function
void debugPrintCallStackTrace()
{
    S32 trace_size = 0;
    char **messages = (char **)NULL;
    void *stack_traces[MAX_STACK_FRAMES] = {0};

    trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
    messages = backtrace_symbols(stack_traces, trace_size);

    if (!messages) {
        printf("Couldn't print stack trace\n");
        exit(1);
    }

    /* skip the first couple stack frames (as they are this function and
       our handler) and also skip the last frame as it's (always?) junk. */
    // for (i = 3; i < (trace_size - 1); ++i)
    // we'll use this for now so you can see what's going on
    for (S32 i = 0; i < trace_size; ++i) {
        if (debug_AddressToLine(stack_traces[i]) != 0) {
            printf("  error determining line # for: %s\n", messages[i]);
        }
    }

    free(messages);
}
