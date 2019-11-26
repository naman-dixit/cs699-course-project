/**
 * These functions are used in readig and writing files. These are the low-level functions
 * on which other systems like asset loader rely.
 *
 * @file file.c
 * @author Team Octal
 * @brief Functions for file read/write operations
 */

/**
* @brief Reads a file at the given path
*
* This function is used for loading and reading files
* for the game which contain various pieces of data needed for the
* proper operations of the game.
*
* @param file_path Path to the file
* @param size Returns the size of read file
*
* @return Pointer to the file data
*/
internal_function
Byte* fileRead (char *file_path, Size *size)
{
    SDL_RWops *rwops = rwops = SDL_RWFromFile(file_path, "rb");
    if (rwops == NULL) {
        logConsole(LOG_LEVEL_ERROR,
                   LOG_CHANNEL_FILE,
                   "Can't find file %s",
                   file_path);
        return NULL;
    }

    Size length = (Size)SDL_RWsize(rwops);
    Byte *file_data = malloc(length + 1);
    SDL_RWread(rwops, file_data, length, 1);

    if (size != NULL) {
        *size = length;
    }
    file_data[length] = '\0';

    SDL_RWclose(rwops);

    return file_data;
}
