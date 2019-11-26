/**
 * These functions are used to wrap around the OpenGL API in order to provide a higher level
 * abstraction for often used operations.
 *
 * @file opengl.c
 * @author Team Octal
 * @brief Functions for OpenGL API
 */

/**
* @brief Function to compile OpenGL shaders
*
* This function takes the source of a vertex shader and a fragment shader, and
* compiles them into a the OpenGL program for graphics rendering.
*
* @param vert_src Vertex shader source
* @param frag_src Fragment shader source
*
* @return Handle to the compiled program
*/
internal_function
GLint openglShaderCreate(const char *const vert_src,
                         const char *const frag_src)
{
    GLuint vert, frag;
    GLint vert_status, frag_status, program_status;

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, NULL);
    glCompileShader(vert);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_status);
    if (vert_status == GL_FALSE) {
        GLint log_length;
        glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* log = malloc(sizeof(*log) * ((Size)log_length+1));
        glGetShaderInfoLog(vert, log_length, NULL, log);
        logConsole(LOG_LEVEL_ERROR,
                   LOG_CHANNEL_RENDER,
                   "Vertex Shader: %s\n",
                   log);
        free(log);
        return -1;
    }

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, NULL);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &frag_status);
    if (frag_status == GL_FALSE) {
        GLint log_length;
        glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* log = malloc(sizeof(*log) * ((Size)log_length+1));
        glGetShaderInfoLog(frag, log_length, NULL, log);
        logConsole(LOG_LEVEL_ERROR,
                   LOG_CHANNEL_RENDER,
                   "Fragment Shader: %s\n",
                   log);
        free(log);
        return -1;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &program_status);
    if (program_status == GL_FALSE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* log = malloc(sizeof(*log) * ((Size)log_length+1));
        glGetProgramInfoLog(program, log_length, NULL, log);
        logConsole(LOG_LEVEL_ERROR,
                   LOG_CHANNEL_RENDER,
                   "Shader Program %d: %s\n",
                   program, log);
        free(log);
        return -1;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    return (GLint)program;
}
