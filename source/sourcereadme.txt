Compiling:

    Install Clang 8.0 and run the `build.linux` script in the project's root directory.

Usage:

    Run the following command from the project's root directory:

            ./bin/linux/x64/game

    Inside the program, the right side of the screen contains the shell where you can
    enter commands. The left side of the window contains the tutorial instrcutions.
    Follow the instructions to work through the demo tutorial.

Modification:

    To make changes to the tutorial, edit the file `data/scripts/tutorial.lua`

    To add new commands, add a new source file with proper command implementation
    in `data/scripts/command` and then add an entry of the file in the Lua file
    `data/scripts/command/command.lua`.
