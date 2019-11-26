local t = require "command/_tutorial"

local tutorial = {}

function tutorial.tutorial ()
   t.step("cd home", {
         "Directories can be navigated by using \"cd\" command.",
         "    Please change to home directory by entering:",
         "",
         "            cd home"
   })

   t.step("cd ..", {"",
         "Now, move back one directory up by entering:",
         "",
         "            cd .."
   })

   t.step("ls", {"",
      "To list the content of a directory, we use \"ls\" command.",
      "Without any arguments, it list contents of current directory.",
      "Enter the following command to continue:",
      "",
      "            ls"
})

t.step("ls tmp", {"",
      "Giving the name of a directory lists its contents, like below:",
      "",
      "            ls tmp"
})

t.step("mkdir dir", {"",
      "\"mkdir\" is used to create new directories.",
      "For example, enter the following command:",
      "",
      "            mkdir dir"
})

t.step("touch file", {"",
      "\"touch\" is used to create new empty files.",
      "To create a new file in the current directory, enter:",
      "",
      "            touch file"
})

t.step("mv file dir", {"",
      "\"mv\" is used to move files and directories.",
      "Let's move the newly created file into the newly created directory:",
      "",
      "            mv file dir"
})

t.step("ls dir", {"",
      "To verify that the move worked, let's list the contents of the directory",
      "",
      "            ls dir"
})

   t.finish({"", "Tutorial complete!", "Well Done!"})
end

return tutorial
