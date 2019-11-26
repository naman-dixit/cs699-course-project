/**
 * This file contains the functions used for loading various files containing data used in the
 * running of this game. These files are often called assets in the the parlance of game development.
 *
 * @file assets.c
 * @author Team Octal
 * @brief Functions for assets loading
 */

/**
* @brief This function load the GLSL shaders assets for visual effetcs.
*
* It reads the vertex shader stored at @p vertex_path and fragment shader stored at
* @p fragement_path and compiles the into a single shader program.
* Before exiting it frees up the vertex and fragment source.
*
* @param vertex_path Path of vertex shader
* @param fragment_path Path of fragment shader
* @param program Used to return the GLSL program handle
*
* @return success/failure
*/
internal_function
B32 assetLoadShader (char* vertex_path,
                     char* fragment_path,
                     GLuint *program)
{
    char *vertex_src = (Char*)fileRead(vertex_path, NULL);
    if (vertex_src == NULL) {
        return false;
    }

    char *fragment_src = (Char*)fileRead(fragment_path, NULL);
    if (fragment_src == NULL) {
        free(vertex_src);
        return false;
    }

    S32 program_temp = 0;
    if ((program_temp = openglShaderCreate(vertex_src, fragment_src)) < 0) {
        logConsole(LOG_LEVEL_CRITICAL,
                   LOG_CHANNEL_SCRIPT,
                   "Couldn't load shaders %s and %s",
                   vertex_path, fragment_path);

        free(vertex_src);
        free(fragment_src);

        return false;
    }

    *program = (GLuint)program_temp;

    free(vertex_src);
    free(fragment_src);

    return true;

}

/**
* @brief This function load the Lua script for gameplay code.
*
* It reads the Lua script file at @p script_path and compiles it into
* a Lua context @p game. If any errors occur during compilation, it also
* takes care of them.
*
* @param game The Lua context in which the scripts will be run
* @param script_path Path of Lua script source file
*
* @return Execution status
*/

internal_function
B32 assetLoadScript (lua_State *game,
                     char *script_path)
{
    char *script_src = (Char*)fileRead(script_path, NULL);
    if (script_path == NULL) return false;

    if (luaL_loadstring(game, script_src) ||
        lua_pcall(game, 0, 0, 0)) {
        logConsole(LOG_LEVEL_CRITICAL,
                   LOG_CHANNEL_SCRIPT,
                   "Couldn't load script %s: %s",
                   script_path,
                   lua_tostring((lua_State*)game, -1));
        lua_pop((lua_State*)game, 1);
        free(script_src);

        return false;
    }

    free(script_src);

    return true;
}

/**
* @brief This function load a TTF font and bakes it into a bitmap.
*
* It load the font file and using stb_truetype library, bakes the vector
* font into a bitmap that can be rendered using OpenGL's native capabilities
* to render textured quads.
*
* @param font_path Path of the font file
* @param font_size Point size of rendered font
* @param vert_path Path of vertex shader used for text rendering
* @param frag_path Path of fragment shader used for text rendering
* @param bitmap_width Width opf baked bitmap
* @param bitmap_height Heifght of baked bitmap
* @param char_first First renderable character
* @param char_num Total number of renderable characters
* @param vao Vertex Array Object
* @param vbo Vertex Buffer Object
* @param baked_char Baked bitmap
* @param program Returns handle for compiled shader
* @param texture Returns handle for texture stored GPU side
*
* @return Execution status
*/

internal_function
B32 assetLoadTrueTypeFont (Char *font_path, U32 font_size,
                           Char *vert_path, Char *frag_path,
                           U32 bitmap_width, U32 bitmap_height,
                           U32 char_first, U32 char_num,
                           GLuint vao, GLuint vbo,
                           stbtt_bakedchar **baked_char,
                           GLuint *program, GLuint *texture)
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    assetLoadShader(vert_path, frag_path, program);
    glUseProgram(*program);

    GLint pos_attrib = glGetAttribLocation(*program, "position");
    glVertexAttribPointer((GLuint)pos_attrib,
                          2,
                          GL_FLOAT, GL_FALSE,
                          4 * sizeof(float),
                          0);
    glEnableVertexAttribArray((GLuint)pos_attrib);

    GLint tex_attrib = glGetAttribLocation(*program, "tex_coord");
    glVertexAttribPointer((GLuint)tex_attrib,
                          2,
                          GL_FLOAT, GL_FALSE,
                          4 * sizeof(float),
                          (void*)(2*sizeof(GLfloat)));
    glEnableVertexAttribArray((GLuint)tex_attrib);

    U8 *ttf_buffer = fileRead(font_path, NULL);

    U8 *bitmap = malloc(sizeof(*bitmap) * bitmap_width * bitmap_height);
    *baked_char = malloc(sizeof(**baked_char) * char_num);

    stbtt_BakeFontBitmap(ttf_buffer, 0,
                         font_size,
                         bitmap,
                         (int)bitmap_width, (int)bitmap_height,
                         (int)char_first, (int)char_num,
                         *baked_char);

    free(ttf_buffer);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                 (GLsizei)bitmap_width, (GLsizei)bitmap_height,
                 0, GL_RED, GL_UNSIGNED_BYTE,
                 bitmap);
    glGenerateMipmap(GL_TEXTURE_2D);

    glUniform1i(glGetUniformLocation(*program, "font_bitmap"), 0);
    free(bitmap);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}
