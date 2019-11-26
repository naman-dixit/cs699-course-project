package.path = "data/scripts/lib/?.lua;" .. package.path

local inode = require "inode"

local mkdir = {}

function mkdir.call (tbl)
   local src = {}

   for i = 1, #tbl.args do
      if tbl.args[1]:sub(1, 1) == "/" then
         table.insert(src, {["path"] = tbl.args[i]})
      else
         table.insert(src, {["path"] = Game.FS:path(Game.FS.pwd) .. tbl.args[i]})
      end
   end

   for i = 1, #src do
      local parts = Game.FS.segments(src[i].path)
      local name = parts[#parts]
      local success, err = Game.FS:insert(inode.new(name, "d"), "/")
      if not success then
         return {err}
      end
   end
end

return mkdir
