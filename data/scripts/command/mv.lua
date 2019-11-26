package.path = "data/scripts/lib/?.lua;" .. package.path

local inode = require "inode"

local mv = {}

function mv.call (tbl)
   local src = {}
   for i = 1, #tbl.args - 1 do
      if tbl.args[1]:sub(1, 1) == "/" then
         table.insert(src, {["path"] = tbl.args[i]})
      else
         table.insert(src, {["path"] = Game.FS:path(Game.FS.pwd) .. tbl.args[i]})
      end
   end

   for j = 1, #src do
      local parts, err = Game.FS.segments(src[j].path)
      local root = Game.FS.inode
      local parent = root
      local grandparent = nil
      local grandparent_child_num

      if parts ~= nil then
         for k, p in ipairs(parts) do
            local child = nil
            local i = 1
            while child == nil and i <= #parent.children do
               if parent.children[i].name == p then
                  child = parent.children[i]
                  grandparent_child_num = i
               end
               i = i + 1
            end

            if child == nil then
               return nil, "Path \"" .. src[j].path .. "\" doesn't exist"
            elseif child.kind ~= "d" and k < #parts then
               return nil, "\"" .. src[j].path .. "\" is an invalid path"
            else
               grandparent = parent
               parent = child
            end
         end
      else
         return nil, err
      end

      src[j].inode = parent
      src[j].parent = grandparent
      src[j].parent_child_num = grandparent_child_num
   end

   for i = 1, #src do
      if src[i].inode.kind == "d" then
         return nil, "\"" .. src[i].path .. "\" is a directory"
      end
   end

   local dest = {}
   if tbl.args[1]:sub(1, 1) == "/" then
      dest.path = tbl.args[#tbl.args]
   else
      dest.path = Game.FS:path(Game.FS.pwd) .. tbl.args[#tbl.args]
   end


   do
      local parts, err = Game.FS.segments(dest.path)
      local root = Game.FS.inode
      local parent = root
      local grandparent = nil
      local grandparent_child_num
      local new_file_name = nil

      if parts ~= nil then
         for k, p in ipairs(parts) do
            local child = nil
            local i = 1
            while child == nil and i <= #parent.children do
               if parent.children[i].name == p then
                  child = parent.children[i]
                  grandparent_child_num = i
               end
               i = i + 1
            end

            if child == nil and k == #parts then
               child = inode.new("", "f")
               child.parent = parent
               table.insert(parent.children, child)
               grandparent_child_num = #parent.children
               new_file_name = p
               grandparent = parent
               parent = child
            elseif child.kind ~= "d" and k < #parts then
               return nil, "\"" .. dest.path .. "\" is an invalid path"
            else
               grandparent = parent
               parent = child
            end
         end
      else
         return nil, err
      end

      dest.inode = parent
      dest.parent = grandparent
      dest.parent_child_num = grandparent_child_num
      if new_file_name ~= nil then
         dest.name = new_file_name
      end
   end

   if dest.inode.kind == "f" and #src > 1 then
      return nil, "\"" .. dest.path .. "\" is not a directory"
   end

   if dest.inode.kind == "f" then
      print("HEY")
      dest.parent.children[dest.parent_child_num] = src[1].inode
      if dest.name ~= nil then
         dest.parent.children[dest.parent_child_num].name = dest.name
         print(dest.name)
      end
      src[1].inode.parent = dest.parent
      table.remove(src[1].parent.children, src[1].parent_child_num)
   end

   if dest.inode.kind == "d" then
      for i = 1, #src do
         table.insert(dest.inode.children, src[i].inode)
         src[i].inode.parent = dest.inode
         table.remove(src[i].parent.children, src[i].parent_child_num)
      end
   end
end

return mv
