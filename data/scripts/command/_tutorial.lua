local getopt = require "command/_getopt"

local t = {}

function t.step (command, instructions)
   local input = coroutine.yield(instructions)

   while getopt.compare(getopt.parse(command), getopt.parse(input)) == false do
      input = coroutine.yield()
   end
end

function t.finish (instructions)
   coroutine.yield(instructions)
end

return t
