/**
 * These functions are used for logging any data that could be used for debugging, diagnostics
 * or analysis in the future.
 *
 * @file log.c
 * @author Team Octal
 * @brief Functions for logging
 */

/**
* @brief Enumeration of all priority levels of logging
*
* This enum provides various priority (or severity) levels that we can assign to any log diagnostic.
*/
enum Log_Level {
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_COUNT,
};

/**
* @brief Enumeration of all sources of logging messages
*
* This enum provides all the channels (or sources) from which a log diagnostic may arrive.
*/
enum Log_Channel {
    LOG_CHANNEL_UNKNOWN = SDL_LOG_CATEGORY_APPLICATION,
    LOG_CHANNEL_OPENGL,
    LOG_CHANNEL_ASSETS,
    LOG_CHANNEL_LOG,
    LOG_CHANNEL_FILE,
    LOG_CHANNEL_SCRIPT,
    LOG_CHANNEL_TIME,
    LOG_CHANNEL_AUDIO,
    LOG_CHANNEL_RENDER,
    LOG_CHANNEL_INIT,
    LOG_CHANNEL_ARG,
    LOG_CHANNEL_LOOP,
    LOG_CHANNEL_COUNT,
};

/**
* @brief Function to log console data/information
*
* This function is used for logging console information and other errors and warning messages
* produced during the game's executions.
*/
internal_function
B32 logConsole (enum Log_Level level, enum Log_Channel channel,
               const char *text, ...)
{
    va_list args;
    va_start(args, text);

    SDL_LogPriority priority;
    if (level == LOG_LEVEL_VERBOSE) priority = SDL_LOG_PRIORITY_VERBOSE;
    else if (level == LOG_LEVEL_DEBUG) priority = SDL_LOG_PRIORITY_DEBUG;
    else if (level == LOG_LEVEL_INFO) priority = SDL_LOG_PRIORITY_INFO;
    else if (level == LOG_LEVEL_WARN) priority = SDL_LOG_PRIORITY_WARN;
    else if (level == LOG_LEVEL_ERROR) priority = SDL_LOG_PRIORITY_ERROR;
    else if (level == LOG_LEVEL_CRITICAL) priority = SDL_LOG_PRIORITY_CRITICAL;
    else priority = SDL_LOG_PRIORITY_INFO;

    char *channel_name;
    switch (channel) {
    case LOG_CHANNEL_UNKNOWN:
        channel_name = "[...] ";
        break;
    case LOG_CHANNEL_LOG:
        channel_name = "[LOG] ";
        break;
    case LOG_CHANNEL_FILE:
        channel_name = "[FILE] ";
        break;
    case LOG_CHANNEL_SCRIPT:
        channel_name = "[SCRIPT] ";
        break;
    case LOG_CHANNEL_TIME:
        channel_name = "[TIME] ";
        break;
    case LOG_CHANNEL_AUDIO:
        channel_name = "[AUDIO]: ";
        break;
    case LOG_CHANNEL_RENDER:
        channel_name = "[RENDER] ";
        break;
    case LOG_CHANNEL_INIT:
        channel_name = "[INIT] ";
        break;
    case LOG_CHANNEL_ARG:
        channel_name = "[ARG] ";
        break;
    case LOG_CHANNEL_LOOP:
        channel_name = "[LOOP] ";
        break;
    case LOG_CHANNEL_OPENGL:
        channel_name = "[OpenGL] ";
        break;
    case LOG_CHANNEL_ASSETS:
        channel_name = "[Assets] ";
        break;
    case LOG_CHANNEL_COUNT:
        channel_name = "[**INVALID**] ";
        break;
    }

    Size text_length = strlen(channel_name) + strlen(text) + 1;
    Char* real_text = malloc(sizeof(*real_text) * text_length);
    strcpy(real_text, channel_name);
    strcat(real_text, text);

    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, priority, real_text, args);
    free(real_text);
    va_end(args);
    return true;
}

/**
* @brief Function to log OpenGL diagnostics
*
* This function is used for logging 3D rendering debug data and other error and warning messages
* related to 3D OpenGL graphics.
*
* @param source Information on who sent the diagnostic
* @param type Kind of diagnostic
* @param id Unique ID of the diagnostic
* @param severity Denotes how severe it is
* @param length Length of the diagnostic message
* @param message Diagnostic message
* @param user_param Any user parameters, unused
*/
internal_function
void logGLDebugCallback(U32 source, U32 type, U32 id, U32 severity,
                        S32 length, const Char* message,
                        const void* user_param)
{
    unused_variable(user_param);

    switch (id) {
        case 131218: { // NOTE(naman): NVIDIA: "shader will be recompiled due to GL state mismatches"
            return;
        } break;
        default: {
        } break;
    }

    enum Log_Level level;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
        level = LOG_LEVEL_ERROR;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        level= LOG_LEVEL_WARN;
        break;
    case GL_DEBUG_SEVERITY_LOW_ARB:
        level = LOG_LEVEL_VERBOSE;
        break;
    default:
        level = LOG_LEVEL_VERBOSE;
        break;
    }

    char *type_str = NULL;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR_ARB:
        type_str = "(Error) ";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        type_str = "(Deprecated Warning) ";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
        type_str = "(Undefined Behaviour) ";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        type_str = "(Performance Warning) ";
        break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:
        type_str = "(Non-Portable) ";
        break;
    case GL_DEBUG_TYPE_OTHER_ARB:
        type_str = "(Others) ";
        break;
    default:
        type_str = "(Others) ";
        break;
    }

    char *source_str = NULL;
    switch (source) {
    case GL_DEBUG_SOURCE_API_ARB:
        source_str = "{GL API}: ";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
        source_str = "{Shader Compiler}: ";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
        source_str = "{Window System}: ";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
        source_str = "{Third Party}: ";
        break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:
        source_str = "{Application}: ";
        break;
    case GL_DEBUG_SOURCE_OTHER_ARB:
        source_str = "{Others}: ";
        break;
    default:
        source_str = "{Others}: ";
        break;
    }

    Size text_length = strlen(type_str) + strlen(source_str) + (Size)length + 1;
    Char* real_text = malloc(sizeof(*real_text) * text_length);
    strcpy(real_text, source_str);
    strcat(real_text, type_str);
    strcat(real_text, message);

    logConsole(level,
               LOG_CHANNEL_OPENGL,
               real_text);
    free(real_text);

    if ((type == GL_DEBUG_TYPE_ERROR_ARB) ||
        (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB) ||
        (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)) {
        debugPrintCallStackTrace();
        SDL_TriggerBreakpoint();
    }

    return;
}
