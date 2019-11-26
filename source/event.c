/**
 * These functions are used in handling operating system events generated
 * due to user interactionss with the program. Since the actual event processing happens
 * in Lua code, we just transfer these events to a Lua context.
 *
 * @file event.c
 * @author Team Octal
 * @brief Functions for event handling
 */

/**
* @brief Function to send the key being pressed to Lua
*
* This fucntion sends the "Key Pressed" events to Lua game code using neccessary lua function
* to perform corresponding action.
*
* @param l Lua context
* @param key Name of pressed key
* @param state The state of key (up/down/held)
*/
internal_function
void eventKeyboard (lua_State *l,
                    char *key, char *state)
{
    // ... Events
    lua_newtable(l);
    // ... Events <TABLE>

    lua_pushstring(l, "Keyboard");
    // ... Events <TABLE> "Keyboard"
    lua_setfield(l, -2, "Device");

    // ... Events <TABLE>

    lua_pushstring(l, key);
    // ... Events <TABLE> key
    lua_setfield(l, -2, "Key");

    // ... Events <Table>

    lua_pushstring(l, state);
    // ... Events <TABLE> state
    lua_setfield(l, -2, "State");

    // ... Events <Table>

    Size events_size = lua_objlen(l, -2);
    lua_rawseti(l, -2, (Sint)events_size + 1);
}

/**
* @brief Function to send the text to Lua after it has been typed
*
* This function gets the text which was typed and call neccessary lua function to
* send it to the Lua context..
*
* @param l Lua context
* @param text Typed text
*/
internal_function
void eventText (lua_State *l,
                const char *const text)
{
    // ... Events
    lua_newtable(l);
    // ... Events <TABLE>

    lua_pushstring(l, "Text");
    // ... Events <TABLE> "Text"
    lua_setfield(l, -2, "Device");
    // ... Events <TABLE>

    lua_pushstring(l, "Text");
    // ... Events <TABLE> "Text"
    lua_setfield(l, -2, "Type");
    // ... Events <Table>

    lua_pushstring(l, text);
    // ... Events <TABLE> text
    lua_setfield(l, -2, "Text");
    // ... Events <Table>

    Size events_size = lua_objlen(l, -2);
    lua_rawseti(l, -2, (Sint)events_size + 1);
}
/**
* @brief Function to swend the control characters beeing pressed to Lua
*
* This function sends control characters (e.g. ENTER, BACKSPACE, etc.) to Lua which are
* used to perform special actions in game.
*
* @param l Lua context
* @param control Name of special character
*/
internal_function
void eventTextControl (lua_State *l,
                       const char *const control)
{
    // ... Events
    lua_newtable(l);
    // ... Events <TABLE>

    lua_pushstring(l, "Text");
    // ... Events <TABLE> "Text"
    lua_setfield(l, -2, "Device");
    // ... Events <TABLE>

    lua_pushstring(l, "Control");
    // ... Events <TABLE> "Control"
    lua_setfield(l, -2, "Type");
    // ... Events <Table>

    lua_pushstring(l, control);
    // ... Events <TABLE> control
    lua_setfield(l, -2, "Control");
    // ... Events <Table>

    Size events_size = lua_objlen(l, -2);
    lua_rawseti(l, -2, (Sint)events_size + 1);
}
