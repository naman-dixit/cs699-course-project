local t = require "command/_tutorial"

local tutorial = {}

function tutorial.tutorial ()
   t.step("cd home", {
         "Directories can be navigated by using \"cd\" command.",
         "    Please change to home directory by entering:",
         "",
         "            cd home"
   })

   t.step("cd ..", {
         "Now, move back one directory up by entering:",
         "            cd .."
   })

   t.finish({"Well Done!"})
end

return tutorial
