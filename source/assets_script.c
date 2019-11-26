/**
 * These functions are called from Lua and are used to hook into the asset loader that is
 * implemented in the engine.
 *
 * @file assets_script.c
 * @author Team Octal
 * @brief Lua functions for asset loading
 */

/**
* @brief Lua injected function which calls @ref assetLoadScript
*
* This function is called from Lua and is used to call into @ref assetLoadScript using
* proper parameters.
*
* @param l Lua context
*
* @return Execution status
*/
internal_function
Sint scriptAssetLoadScript (lua_State *l)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
    lua_State *game = lua_topointer(l, lua_upvalueindex(1));
    char *script_path = luaL_checkstring(l, 2);
#pragma clang diagnostic pop

    if (assetLoadScript(game, script_path) == false) {
        logConsole(LOG_LEVEL_CRITICAL,
                   LOG_CHANNEL_ASSETS,
                   "Couldn't load script %s",
                   script_path);
        // TODO(naman): Quit gracefully
        global_game_is_running = false;
    }

    lua_pushstring(l, script_path);
    lua_setfield(l, 1, "ScriptPath");

    return 0;
}

/**
* @brief Lua injected function which calls @ref assetLoadTrueTypeFont
*
* This function is called from Lua and is used to call into @ref assetLoadTrueTypeFont using
* proper parameters.
*
* @param l Lua context
*
* @return Execution status
*/
internal_function
int scriptAssetLoadTrueTypeFont (lua_State *l)
{
    System *system = lua_topointer(l, lua_upvalueindex(1));

    Char *font_path = luaL_checkstring(l, 2);
    U32 font_size = (U32)luaL_checknumber(l, 3);
    F32 scaling_factor = (F32)luaL_checknumber(l, 4);
    Char *vert_path = luaL_checkstring(l, 5);
    Char *frag_path = luaL_checkstring(l, 6);

    F32 x_scaling = (F32)system->window.height/(F32)system->window.width;

    // x_scaling = 1;
    // TODO: Is there any way to automate this to prevent manually
    // checking if the bitmap fits?
    U32 bitmap_height = 512;
    U32 bitmap_width = 512;

    U32 char_first = 32;
    U32 char_num = 95;  // ASCII 32..126 is 95 glyphs

    stbtt_bakedchar *baked_char = NULL;

    U32 vao = 0, vbo = 0, texture = 0, program = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    if (assetLoadTrueTypeFont(font_path, font_size,
                              vert_path, frag_path,
                              bitmap_width, bitmap_height,
                              char_first, char_num,
                              vao, vbo,
                              &baked_char,
                              &program, &texture) == false) {
        logConsole(LOG_LEVEL_CRITICAL,
                   LOG_CHANNEL_ASSETS,
                   "Couldn't load TTF %s",
                   font_path);
        global_game_is_running = false;
    }


    lua_pushstring(l, font_path);
    lua_setfield(l, 1, "FontPath");

    lua_pushnumber(l, font_size);
    lua_setfield(l, 1, "FontSize");

    lua_pushnumber(l, (F64)scaling_factor);
    lua_setfield(l, 1, "ScalingFactor");

    lua_pushnumber(l, (F64)x_scaling);
    lua_setfield(l, 1, "XScaling");

    lua_pushstring(l, vert_path);
    lua_setfield(l, 1, "VertexPath");

    lua_pushstring(l, frag_path);
    lua_setfield(l, 1, "FragmentPath");


    lua_pushnumber(l, bitmap_width);
    lua_setfield(l, 1, "BitmapWidth");

    lua_pushnumber(l, bitmap_height);
    lua_setfield(l, 1, "BitmapHeight");

    lua_pushnumber(l, char_first);
    lua_setfield(l, 1, "CharacterFirst");

    lua_pushnumber(l, char_num);
    lua_setfield(l, 1, "CharacterCount");


    lua_pushlightuserdata(l, baked_char);
    lua_setfield(l, 1, "CharacterTable");

    lua_pushnumber(l, vao);
    lua_setfield(l, 1, "VAO");

    lua_pushnumber(l, vbo);
    lua_setfield(l, 1, "VBO");

    lua_pushnumber(l, program);
    lua_setfield(l, 1, "Program");

    lua_pushnumber(l, texture);
    lua_setfield(l, 1, "Texture");

    return 0;
}
