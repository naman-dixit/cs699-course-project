/**
 * These functions are called from Lua and are used to hook into the logger that is
 * implemented in the engine.
 *
 * @file log_script.c
 * @author Team Octal
 * @brief Lua functions for logging
 */

/**
* @brief Lua injected function which calls @ref logConsole
*
* This function is called from Lua and is used to call into @ref logConsole using
* proper parameters.
*
* @param l Lua context
*
* @return Execution status
*/
internal_function
int scriptLog (lua_State *l)
{
    enum Log_Level level = luaL_checknumber(l, 1);
    enum Log_Channel channel = luaL_checknumber(l, 2);
    const char *text = luaL_checkstring(l, 3);

    logConsole(level, channel, text);

    return 0;
}
