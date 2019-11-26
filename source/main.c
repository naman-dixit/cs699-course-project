/**
 * This file implements the core engine and uses all the various other subsystems in an orderly
 * manner.
 *
 * @file main.c
 * @author Team Octal
 * @brief Main engine source
 */

#include "nlib/nlib.h"
#include "nlib/linear_algebra.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#include "external/glad/glad.h"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wsign-conversion"
#include "external/glad/glad.c"
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wpadded"
#include "external/SDL2/SDL.h"
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wbad-function-cast"
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb/stb_truetype.h"
#pragma clang diagnostic pop

#include "external/lua/lua.h"
#include "external/lua/lauxlib.h"
#include "external/lua/lualib.h"

global_variable char *global_program_name;
global_variable B32 global_game_is_running;

typedef struct System_Window System_Window;
typedef struct System_Audio System_Audio;
typedef struct System_Time System_Time;
typedef struct System_Controls System_Controls;


/**
 * @brief Structure that contains all the state of the engine
 *
 * This structure is used to maintain and communicate the state of the engine to its various
 * subsystems. This struct, as a whole, can be thought to incorporate the global state of the
 * program at any given moment.
 */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct System {
/**
 * @brief Structure that contains the state of the windowing subsystem
 *
 * This contains all the handles for the window, framebuffers, etc. that are used for the display
 * of the game.
 */
    struct System_Window {
        SDL_Window *window; /**< Handle to the window */
        SDL_GLContext gl_context; /**< Handle to OpenGL context */
        GLuint quad_vao; /**< Handle to the vertex array object associated with a quad */
        GLuint luabuffer; /**< Framebuffer into which the Lua code renders */
        GLuint luabuffer_texture; /**< Texture associated with the @ref luabuffer */
        GLuint xbloombuffer; /**< Intermediate framebuffer with horizontal bloom effect */
        GLuint xbloombuffer_texture;/**< Texture associated with the @ref xbloombuffer */
        GLuint xbloom_shader; /**< Shader associated with the @ref xbloombuffer */
        GLuint crt_shader; /**< Shader used to finally render to the screen */
        S32 width; /**< Width of the window */
        S32 height; /**< Height of the window */
    } window;
/**
 * @brief Structure that contains the state of the audio subsystem
 *
 * This contains all the handles for the audio device, etc. as well as the parameters of
 * audio such as sampling rate that are used for the playing of audio.
 */
    struct System_Audio {
        SDL_AudioDeviceID audio_device; /**< Handle to audio device */
        SDL_AudioSpec audio_spec; /**< Specifications of the audio device */
        // TODO(naman): How do we generalize it to all formats (instead of just S16)?
        S16 *audio_buffer; /**< Memory buffer to store sound sample data */
        U32 bytes_per_sample; /**< Size of each sample in bytes */
        U32 target_queue_bytes; /**< Latency of audio stream in bytes */
        U32 volume; /**< Sound's loudness level */
    } audio;
/**
 * @brief Structure that contains the state of the timing subsystem
 *
 * This contains the last computed value of time, to be used to calculate the elapse of time.
 */
    struct System_Time {
        U64 last_counter; /**< Last computed value of time */
    } time;
/**
 * @brief Structure that contains the state of the control subsystem
 *
 * This contains all the data regarding the input devices through which the users interact with the
 * program.
 */
    struct System_Controls {
        U8 keyboard[SDL_NUM_SCANCODES]; /**< State of each ley of keyboard */
        struct {
            S32 x; /**< Horizontal osition of the mouse cursor */
            S32 y; /**< Vertical osition of the mouse cursor */
            B32 left; /**< State of left mouse button */
            B32 right; /**< State of right mouse button */
            B32 middle; /**< State of middle mouse button */
        } mouse;  /**< State of mouse input device */
    } controls;
} System;
#pragma clang diagnostic pop


#include "debug.c"
#include "log.c"
#include "time.c"
#include "opengl.c"
#include "render.c"
#include "file.c"
#include "assets.c"
#include "event.c"

#include "log_script.c"
#include "assets_script.c"
#include "render_script.c"

/**
* @brief Entry point of the program
*
* This function is the entry point into the program and brings together all the parts of the
* engine to make a coherent whole. It implements program initialization, main loop and destruction.
*
* @param argc Number of command line parameters
* @param argv Array of command line parameters
*
* @return Return value of the program
*/
Sint main (Sint argc, Char *argv[])
{
    unused_variable(argc);
    global_program_name = argv[0];

    { // Initialize SDL
        fprintf(stdout, "Initialising SDL2...\n");
        fflush(stdout);

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
            fprintf(stderr,
                    "SDL initialization failed: %s\n",
                    SDL_GetError());
            fprintf(stderr, "Exiting...");
            goto error;
        }


        for (U32 i = LOG_CHANNEL_UNKNOWN; i < LOG_CHANNEL_COUNT; ++i) {
            SDL_LogSetPriority((int)i, SDL_LOG_PRIORITY_DEBUG);
        }

        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "SDL Initialised");
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "Platform: %s", SDL_GetPlatform());
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "Logical CPU Count: %d", SDL_GetCPUCount());
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "L1 Cache Line Size: %d", SDL_GetCPUCacheLineSize());
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "System RAM: %d", SDL_GetSystemRAM());
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "RDTSC Status: %s", (SDL_HasRDTSC() ? "Present" : "Absent"));

        SDL_version sdl_version_compiled;
        SDL_VERSION(&sdl_version_compiled);
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "Compiled SDL Version: %d.%d.%d",
                   sdl_version_compiled.major,
                   sdl_version_compiled.minor,
                   sdl_version_compiled.patch);


        SDL_version sdl_version_linked;
        SDL_GetVersion(&sdl_version_linked);
        logConsole(LOG_LEVEL_INFO,
                   LOG_CHANNEL_INIT,
                   "Loaded SDL Version: %d.%d.%d",
                   sdl_version_linked.major,
                   sdl_version_linked.minor,
                   sdl_version_linked.patch);

        if (SDL_VERSIONNUM(sdl_version_compiled.major,
                           sdl_version_compiled.minor,
                           sdl_version_compiled.patch) >
            SDL_VERSIONNUM(sdl_version_linked.major,
                           sdl_version_linked.minor,
                           sdl_version_linked.patch)) {
            logConsole(LOG_LEVEL_WARN,
                       LOG_CHANNEL_INIT,
                       "Loaded SDL is older than %d.%d.%d, the game might not function properly",
                       sdl_version_compiled.major,
                       sdl_version_compiled.minor,
                       sdl_version_compiled.patch);
        }
    }

    System system = {0};

    { // Fill system state
        { // Create and configure window
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);

            system.window.window = SDL_CreateWindow("CS699",
                                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                    800, 600,
                                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
            if (system.window.window == NULL) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                         "Window not created",
                                         SDL_GetError(),
                                         NULL);
                logConsole(LOG_LEVEL_CRITICAL,
                           LOG_CHANNEL_INIT,
                           "Window could not be created: %s",
                           SDL_GetError());
                goto error;
            }
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Window Created");
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Video Driver: %s",
                       SDL_GetCurrentVideoDriver());

            SDL_SetWindowGrab(system.window.window, SDL_TRUE);
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_DisableScreenSaver();

            SDL_GetWindowSize(system.window.window,
                              &system.window.width,
                              &system.window.height);
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Resolution: %d x %d", system.window.width, system.window.height);
        }

        { // Set up OpenGL
            SDL_GL_LoadLibrary(NULL);

            system.window.gl_context = SDL_GL_CreateContext(system.window.window);
            if (system.window.gl_context == NULL) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                         "GL context not created",
                                         SDL_GetError(),
                                         system.window.window);
                logConsole(LOG_LEVEL_CRITICAL,
                           LOG_CHANNEL_INIT,
                           "GL Context could not be created: %s",
                           SDL_GetError());
                goto error;
            }
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "OpenGL context created");

            gladLoadGLLoader(SDL_GL_GetProcAddress);
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Glad loaded through SDL 2");

            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "OpenGL Version:  %s\n", glGetString(GL_VERSION));
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "GLSL Version:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

            if(GLAD_GL_ARB_debug_output) {
                glDebugMessageCallbackARB(&logGLDebugCallback, NULL);
            }
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);


            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Enabling V-Sync...");
            if (SDL_GL_SetSwapInterval(-1) == -1) { // Late Swap Tearing
                logConsole(LOG_LEVEL_WARN,
                           LOG_CHANNEL_INIT,
                           "Late Swap Tearing not enabled: %s", SDL_GetError());
                if (SDL_GL_SetSwapInterval(1) == -1) { // Normal V-Sync
                    logConsole(LOG_LEVEL_WARN,
                               LOG_CHANNEL_INIT,
                               "V-Sync not enabled: %s", SDL_GetError());
                }
            }

            glViewport(0, 0,
                       system.window.width,
                       system.window.height);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            { // Set up quad
                glGenVertexArrays(1, &system.window.quad_vao);
                glBindVertexArray(system.window.quad_vao);

                GLfloat vertices[] = {
                                      // Pos        Tex
                                      1.0f, 1.0f,   1.0f, 1.0f,
                                      -1.0f, 1.0f,  0.0f, 1.0f,
                                      -1.0f, -1.0f, 0.0f, 0.0f,

                                      1.0f, 1.0f,   1.0f, 1.0f,
                                      -1.0f, -1.0f, 0.0f, 0.0f,
                                      1.0f, -1.0f,  1.0f, 0.0f,
                };

                GLuint VBO;
                glGenBuffers(1, &VBO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            { // Setup Lua framebuffer
                glGenFramebuffers(1, &(system.window.luabuffer));
                glBindFramebuffer(GL_FRAMEBUFFER, system.window.luabuffer);

                // generate texture
                glGenTextures(1, &system.window.luabuffer_texture);
                glBindTexture(GL_TEXTURE_2D, system.window.luabuffer_texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                             system.window.width, system.window.height,
                             0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glBindTexture(GL_TEXTURE_2D, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, system.window.luabuffer_texture, 0);

                GLuint rbo;
                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                      system.window.width, system.window.height);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    logConsole(LOG_LEVEL_ERROR,
                               LOG_CHANNEL_OPENGL,
                               "Framebuffer not complete");
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            { // Setup X-Bloom framebuffer
                glGenFramebuffers(1, &(system.window.xbloombuffer));
                glBindFramebuffer(GL_FRAMEBUFFER, system.window.xbloombuffer);

                // generate texture
                glGenTextures(1, &system.window.xbloombuffer_texture);
                glBindTexture(GL_TEXTURE_2D, system.window.xbloombuffer_texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                             system.window.width, system.window.height,
                             0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glBindTexture(GL_TEXTURE_2D, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, system.window.xbloombuffer_texture, 0);

                GLuint rbo;
                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                      system.window.width, system.window.height);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    logConsole(LOG_LEVEL_ERROR,
                               LOG_CHANNEL_OPENGL,
                               "Framebuffer not complete");
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            { // Set up XBLOOM rendering
                assetLoadShader("data/shaders/xbloom.vert", "data/shaders/xbloom.frag",
                                &system.window.xbloom_shader);
            }

            { // Set up CRT rendering
                assetLoadShader("data/shaders/crt.vert", "data/shaders/crt.frag",
                                &system.window.crt_shader);
            }
        }

        { // Set up audio
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Setting up Audio...");

            SDL_AudioSpec audiospec_desired = {0};

            audiospec_desired.freq = 48000;
            audiospec_desired.format = AUDIO_S16LSB;
            audiospec_desired.channels = 2;
            audiospec_desired.samples = 1600;

            system.audio.audio_device = SDL_OpenAudioDevice(NULL,
                                                            0,
                                                            &audiospec_desired,
                                                            &(system.audio.audio_spec),
                                                            0);

            if (system.audio.audio_device == 0) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                         "Audio device not created",
                                         SDL_GetError(),
                                         system.window.window);
                logConsole(LOG_LEVEL_CRITICAL,
                           LOG_CHANNEL_INIT,
                           "Audio device not created: %s", SDL_GetError());
                goto error;
            }

            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Audio device created");
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Audio Driver: %s",
                       SDL_GetCurrentAudioDriver());

            system.audio.bytes_per_sample = (SDL_AUDIO_BITSIZE(system.audio.audio_spec.format) *
                                             (system.audio.audio_spec.channels)/8);
            system.audio.target_queue_bytes = (4 *
                                               system.audio.audio_spec.samples *
                                               system.audio.bytes_per_sample);
            system.audio.audio_buffer = calloc(1, system.audio.target_queue_bytes);


            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Audio Channels: %hhu\n",
                       system.audio.audio_spec.channels);
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Audio Sampling Frequency: %d\n",
                       system.audio.audio_spec.freq);
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Audio samples Per second: %hu\n",
                       system.audio.audio_spec.samples);
            logConsole(LOG_LEVEL_INFO,
                       LOG_CHANNEL_INIT,
                       "Audio buffer size (samples): %u\n",
                       system.audio.bytes_per_sample);

            SDL_PauseAudioDevice(system.audio.audio_device, 0);
        }
    }

    lua_State *game_code = luaL_newstate();
    luaL_openlibs(game_code);

    { // Fill game_code with all code and data
        lua_newtable(game_code); // <Table>
        lua_setglobal(game_code, "Engine"); // Engine
        lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}

        lua_getglobal(game_code, "Engine"); // Engine
        lua_newtable(game_code); // Engine <Table>
        lua_setfield(game_code, -2, "Functions"); // Engine
        lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}

        lua_newtable(game_code); // <Table>
        lua_setglobal(game_code, "Game"); // Game
        lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}

        // lua_newtable(game_code); // <Table>
        // lua_setglobal(game_code, "Mode"); // Mode
        // lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}

        // lua_newtable(game_code); // <Table>
        // lua_setglobal(game_code, "State"); // State
        // lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}

        lua_newtable(game_code); // <Table>
        lua_setglobal(game_code, "System"); // System
        lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}

        { // C Closures
            lua_getglobal(game_code, "Engine"); // Engine
            lua_newtable(game_code); // Engine <Table>
            lua_setfield(game_code, -2, "Functions"); // Engine
            lua_getfield(game_code, -1, "Functions"); // Engine Functions
            lua_replace(game_code, 1); // Functions

#define SCRIPT_FUNCTION_SYSTEM_UPVALUE(FUNC) do {               \
                char *str = #FUNC;                              \
                lua_pushlightuserdata(game_code, &system);      \
                lua_pushcclosure(game_code, FUNC, 1);           \
                lua_setfield(game_code, 1, &(str[6]));          \
            } while (0)

#define SCRIPT_FUNCTION_GAME_UPVALUE(FUNC) do {               \
                char *str = #FUNC;                              \
                lua_pushlightuserdata(game_code, game_code);    \
                lua_pushcclosure(game_code, FUNC, 1);           \
                lua_setfield(game_code, 1, &(str[6]));          \
            } while (0)

#define SCRIPT_FUNCTION_SYSTEM_GAME_UPVALUE(FUNC) do {          \
                char *str = #FUNC;                              \
                lua_pushlightuserdata(game_code, &system);      \
                lua_pushlightuserdata(game_code, game_code);    \
                lua_pushcclosure(game_code, FUNC, 2);           \
                lua_setfield(game_code, 1, &(str[6]));          \
            } while (0)

#define SCRIPT_FUNCTION_NO_UPVALUE(FUNC) do {   \
        char *str = #FUNC;                      \
        lua_pushcclosure(game_code, FUNC, 0);   \
        lua_setfield(game_code, 1, &(str[6]));  \
    } while (0)

            { // Asset systems
                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAssetLoadShader);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptAssetUnloadShader);

                SCRIPT_FUNCTION_GAME_UPVALUE(scriptAssetLoadScript);

                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAssetLoadTexture);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptAssetUnloadTexture);

                SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAssetLoadTrueTypeFont);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptAssetUnloadTrueTypeFont);

                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAssetLoadTexturedQuad);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptAssetUnloadTexturedQuad);

                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAssetLoadColoredQuad);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptAssetUnloadColoredQuad);

                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAssetLoadColoredLine);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptAssetUnloadColoredLine);
            }

            { // Audio System
                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptAudioSinusoidal);
            }

            { // File System
                // SCRIPT_FUNCTION_SYSTEM_UPVALUE(scriptFileWriteToBase);
            }

            { // Log System
                SCRIPT_FUNCTION_NO_UPVALUE(scriptLog);
            }

            { // Render System
                SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderGetTextDimensions);
                SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderText);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderTexturedQuad);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderTexturedShadedQuad);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderColoredQuad);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderColoredLine);

                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderEnableAdditiveBlend);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderEnableInterpolativeBlend);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderEnableMultiplicativeBlend);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderDisableBlend);

                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderMarkRegion);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderDrawToMarkedRegion);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptRenderRemoveMarking);
            }

            { // Event System
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptEventEnableTextInput);
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptEventDisableTextInput);
            }

            { // System System :)
                // SCRIPT_FUNCTION_NO_UPVALUE(scriptSystemQuitGame);
            }

#undef SCRIPT_FUNCTION_SYSTEM_UPVALUE
#undef SCRIPT_FUNCTION_SYSTEM_GAME_UPVALUE
#undef SCRIPT_FUNCTION_NO_UPVALUE

            lua_pop(game_code, lua_gettop(game_code)); // {EMPTY}
        }

        { // Load assets
            if (assetLoadScript(game_code, "data/assets.lua") == false) {
                goto error;
            }
        }
    }

    lua_getglobal(game_code, "Loop");
    lua_getfield(game_code, 1, "Init");
    if (lua_pcall(game_code, 0, 0, 0)) {
        logConsole(LOG_LEVEL_CRITICAL,
                   LOG_CHANNEL_LOOP,
                   "Loop.Init failed: %s",
                   lua_tostring(game_code, -1));
        goto error;
    }
    lua_pop(game_code, lua_gettop(game_code));

    { /* This will make sure that
       * 1) no new global variables can be declared,
       * 2) no global variables (except the ones that have been declared already) can be read,
       * 3) already declared globals can be redefined (for script hotloading)
       */

        lua_getglobal(game_code, "_G"); // _G
        if (lua_getmetatable(game_code, 1) == 0) { // _G <metatable>
            lua_newtable(game_code); // _G <Table>
        }

        lua_pushstring(game_code, "__newindex"); // _G <metatable> "__new_index"
        luaL_loadstring(game_code,
                        "local function stop_declarations (t, k, v)\n"  \
                        "  error('Can not declare \"' .. k .. '\" as global', 2)\n" \
                        "end\n"                                         \
                        "return stop_declarations\n");
        // _G <metatable> "__new_index" <chunk>
        lua_pcall(game_code, 0, 1, 0);
        // _G <metatable> "__new_index" <function>
        lua_settable(game_code, 2); // _G <metatable>

        lua_pushstring(game_code, "__index"); // _G <metatable> "__index"
        luaL_loadstring(game_code,
                        "local function stop_readings (t, k, v)\n"      \
                        "  error('Can not read \"' .. k .. '\" from globals', 2)\n" \
                        "end\n"                                         \
                        "return stop_readings\n");
        // _G <metatable> "__index" <chunk>
        lua_pcall(game_code, 0, 1, 0);
        // _G <metatable> "__index" <function>
        lua_settable(game_code, 2); // _G <metatable>

        lua_setmetatable(game_code, 1); // _G
        lua_pop(game_code, 1); // {EMPTY}
    }
#if 0
    // start ffmpeg telling it to expect raw rgba 720p-60hz frames
    // -i - tells it to read frames from stdin
    const char* cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1920x1080 -i - "
        "-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip output.mp4";

    // open pipe to ffmpeg's stdin in binary write mode
    FILE* ffmpeg = popen(cmd, "w");

    int* buffer = malloc(sizeof(*buffer) * (Size)system.window.width*(Size)system.window.height);
    printf("BUFFER: %p\n", (void*)buffer);
    fflush(stdout);
#endif

    SDL_StartTextInput();

    global_game_is_running = true;
    logConsole(LOG_LEVEL_INFO,
               LOG_CHANNEL_LOOP,
               "Starting Main Loop");

    system.time.last_counter = SDL_GetPerformanceCounter();
    while (global_game_is_running) {
        F64 last_frame_time = timeMicrosecondsElapsed(&(system.time.last_counter));

        SDL_PumpEvents();

        { // Event Processing
            lua_newtable(game_code); // <events>

            { // Keyboard Events
                // IMPORTANT: Do not free this pointer
                const U8 *keyboard = SDL_GetKeyboardState(NULL);

                for (U32 i = SDL_SCANCODE_UNKNOWN; i < SDL_NUM_SCANCODES; ++i) {
                    if (keyboard[i] == 1) {
                        if (system.controls.keyboard[i] == 1) {
                            eventKeyboard(game_code,
                                          SDL_GetKeyName(SDL_GetKeyFromScancode(i)),
                                          "Hold");
                        } else {
                            eventKeyboard(game_code,
                                          SDL_GetKeyName(SDL_GetKeyFromScancode(i)),
                                          "Down");
                        }
                    } else {
                        if (system.controls.keyboard[i] == 1) {
                            eventKeyboard(game_code,
                                          SDL_GetKeyName(SDL_GetKeyFromScancode(i)),
                                          "Up");
                        }
                    }
                }
                memcpy(system.controls.keyboard, keyboard, sizeof(U8) * SDL_NUM_SCANCODES);
            }

            { // Misc Events
                SDL_Event event;
                while (SDL_PollEvent(&event) != 0) {
                    if (event.type == SDL_QUIT) {
                        global_game_is_running = false;
                    } else if (event.type == SDL_KEYDOWN) {
                        SDL_Keycode sym = event.key.keysym.sym;
                        if (sym == SDLK_ESCAPE) {
                            // TODO(naman): Make this an event so that game can display menu
                            global_game_is_running = false;
                        }
                        if (SDL_IsTextInputActive()) {
                            switch (sym) {
                                case SDLK_BACKSPACE:
                                case SDLK_KP_BACKSPACE: {
                                    eventTextControl(game_code, "Backspace");
                                } break;
                                case SDLK_DELETE: {
                                    eventTextControl(game_code, "Delete");
                                } break;
                                case SDLK_RETURN:
                                case SDLK_RETURN2:
                                case SDLK_KP_ENTER: {
                                    eventTextControl(game_code, "Enter");
                                } break;
                                case SDLK_RIGHT: {
                                    eventTextControl(game_code, "Right");
                                } break;
                                case SDLK_LEFT: {
                                    eventTextControl(game_code, "Left");
                                } break;
                                case SDLK_DOWN: {
                                    eventTextControl(game_code, "Down");
                                } break;
                                case SDLK_UP: {
                                    eventTextControl(game_code, "Up");
                                } break;
                                case SDLK_TAB: {
                                    eventTextControl(game_code, "Tab");
                                } break;
                                default:
                                    break;
                            }
                        }
                    }
                    else if (event.type == SDL_TEXTINPUT) {
                        eventText(game_code, event.text.text);
                    }
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, system.window.luabuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        { // Call Loop function
            // <events>
            lua_getglobal(game_code, "Loop"); // <Events> Loop
            lua_getfield(game_code, 2, "Loop"); // <Events> Loop Loop()
            lua_pushnumber(game_code, last_frame_time); // <Events> Loop Loop() last_frame_time
            lua_pushvalue(game_code, 1); // <Events> Loop Loop() last_frame_time <Events>
            if (lua_pcall(game_code, 2, LUA_MULTRET, 0)) {
                logConsole(LOG_LEVEL_CRITICAL,
                           LOG_CHANNEL_LOOP,
                           "UpdateAndRender failed: %s",
                           lua_tostring(game_code, -1));
                goto error;
            }
            B32 result = (B32)lua_toboolean(game_code, -1);

            if (result == false) {
                global_game_is_running = false;
            }
            lua_pop(game_code, lua_gettop(game_code));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, system.window.xbloombuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(system.window.xbloom_shader);
        glBindVertexArray(system.window.quad_vao);
//        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, system.window.luabuffer_texture);
        U32 scan_pos = (U32)(((UINT64_MAX - system.time.last_counter) / 2000000) % (U32)system.window.height);
        glUniform1ui(glGetUniformLocation(system.window.xbloom_shader, "scan_pos"), scan_pos);
        glUniform2i(glGetUniformLocation(system.window.xbloom_shader, "resolution"),
                    system.window.width, system.window.height);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(system.window.crt_shader);
        glBindVertexArray(system.window.quad_vao);
        //glDisable(GL_DEPTH_TEST);
        glUniform1ui(glGetUniformLocation(system.window.crt_shader, "scan_pos"), scan_pos);
        glUniform2i(glGetUniformLocation(system.window.crt_shader, "resolution"),
                    system.window.width, system.window.height);
        glUniform1i(glGetUniformLocation(system.window.crt_shader, "screenTexture"), 0);
        glUniform1i(glGetUniformLocation(system.window.crt_shader, "blurTexture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, system.window.luabuffer_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, system.window.xbloombuffer_texture);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(0);

        SDL_GL_SwapWindow(system.window.window);

#if 0
        glReadPixels(0, 0, system.window.width, system.window.height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        fwrite(buffer, sizeof(int)*(Size)system.window.width*(Size)system.window.height, 1, ffmpeg);
#endif
    }

    return 0;

 error:
    return -1;
}
