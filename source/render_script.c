/**
 * These functions are called from Lua and are used to hook into the renderer that is
 * implemented in the engine.
 *
 * @file render_script.c
 * @author Team Octal
 * @brief Lua functions for rendering
 */

/**
* @brief Lua injected function which computes the size of text in screen space
*
* This function is called from Lua and is used to pre-compute the size some text
* would take in screen space if rendered as is.
*
* @param l Lua context
*
* @return Execution status
*/
internal_function
int scriptRenderGetTextDimensions (lua_State *l)
{
    lua_getfield(l, 1, "CharacterFirst");
    Char char_first = (Char)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "CharacterCount");
    Char char_num = (Char)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "ScalingFactor");
    F32 scaling_factor = (F32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "XScaling");
    F32 x_scaling = (F32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "CharacterTable");
    stbtt_bakedchar *baked_char = lua_touserdata(l, -1);

    const char *text = luaL_checkstring(l, 2);

    F32 x = 0, y_min = 0, y_max = 0;

    while (*text) {
        if ((*text >= char_first) &&
            (*text < (char_first + char_num))) {

            stbtt_bakedchar *b = baked_char + ((*text) - char_first);

            F32 y_low = -(b->yoff + (b->y1 - b->y0));
            if (y_low < y_min) {
                y_min = y_low;
            }

            F32 y_high = -b->yoff;
            if (y_high > y_max) {
                y_max = y_high;
            }

            x += (b->xadvance);
        }
        ++text;
    }

    lua_pushnumber(l, (lua_Number)x * (lua_Number)scaling_factor * (lua_Number)x_scaling);
    lua_pushnumber(l, (lua_Number)y_min * (lua_Number)scaling_factor);
    lua_pushnumber(l, (lua_Number)y_max * (lua_Number)scaling_factor);

    return 3;
}

/**
* @brief Lua injected function which calls @ref renderText
*
* This function is called from Lua and is used to call into @ref renderText using
* proper parameters.
*
* @param l Lua context
*
* @return Execution status
*/
internal_function
int scriptRenderText (lua_State *l)
{
    lua_getfield(l, 1, "VAO");
    GLuint vao = (GLuint)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "VBO");
    GLuint vbo = (GLuint)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "Texture");
    GLuint texture = (GLuint)luaL_checknumber(l, -1);


    lua_getfield(l, 1, "ScalingFactor");
    F32 scaling_factor = (F32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "XScaling");
    F32 x_scaling = (F32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "FontSize");
    U32 font_size = (U32)luaL_checknumber(l, -1);
    unused_variable(font_size);

    lua_getfield(l, 1, "CharacterFirst");
    U32 char_first = (U32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "CharacterCount");
    U32 char_num = (U32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "BitmapWidth");
    U32 bitmap_width = (U32)luaL_checknumber(l, -1);

    lua_getfield(l, 1, "BitmapHeight");
    U32 bitmap_height = (U32)luaL_checknumber(l, -1);

    stbtt_bakedchar *baked_char = NULL;
    lua_getfield(l, 1, "CharacterTable");
    if (lua_islightuserdata(l, -1)) {
        baked_char = lua_touserdata(l, -1);
    } else {
        logConsole(LOG_LEVEL_CRITICAL,
                   LOG_CHANNEL_ASSETS,
                   "Can't render text: value is not a light user data");
        global_game_is_running = false;
    }

    lua_getfield(l, 1, "Program");
    U32 program = (U32)luaL_checknumber(l, -1);

    char *text = luaL_checkstring(l, 2);

    Vec3 screen_pos;

    lua_pushstring(l, "X");
    lua_gettable(l, 3);
    screen_pos.x = (F32)luaL_checknumber(l, lua_gettop(l));
    lua_pop(l, 1);

    lua_pushstring(l, "Y");
    lua_gettable(l, 3);
    screen_pos.y = (F32)luaL_checknumber(l, lua_gettop(l));
    lua_pop(l, 1);

    lua_pushstring(l, "Z");
    lua_gettable(l, 3);
    screen_pos.z = (F32)luaL_checknumber(l, lua_gettop(l));
    lua_pop(l, 1);

    Vec3 color = {0};

    lua_pushstring(l, "R");
    lua_gettable(l, 4);
    color.x = (F32)luaL_checknumber(l, lua_gettop(l));
    lua_pop(l, 1);

    lua_pushstring(l, "G");
    lua_gettable(l, 4);
    color.y = (F32)luaL_checknumber(l, lua_gettop(l));
    lua_pop(l, 1);

    lua_pushstring(l, "B");
    lua_gettable(l, 4);
    color.z = (F32)luaL_checknumber(l, lua_gettop(l));
    lua_pop(l, 1);

    F32 x = 0, y_min = 0, y_max = 0;
    renderText(vao, vbo, texture, program,
               (Char)char_first, (Char)char_num,
               bitmap_width, bitmap_height,
               baked_char,
               text,
               screen_pos, color,
               scaling_factor, x_scaling,
               &x, &y_min, &y_max);

    lua_pushnumber(l, (lua_Number)x * (lua_Number)scaling_factor * (lua_Number)x_scaling);
    lua_pushnumber(l, (lua_Number)y_min * (lua_Number)scaling_factor);
    lua_pushnumber(l, (lua_Number)y_max * (lua_Number)scaling_factor);

    return 3;
}
