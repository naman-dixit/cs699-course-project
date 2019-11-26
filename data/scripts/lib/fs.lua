local filesystem = {}
filesystem.__index = filesystem

function filesystem.new (inode_lib)
    local fs = setmetatable({}, filesystem)
    fs.inode = inode_lib.new("", "d")

    return fs
end

function filesystem.segments (path)
   if path == "/" then return {} end
   local parts = {}
   for part in path:gmatch("[^/]+") do
      table.insert(parts, part)
   end
   local i = 1
   while i <= #parts do
      if parts[i] == "." then
         table.remove(parts, i)
      elseif parts[i] == ".." then
         table.remove(parts, i)
         i = i - 1
         if i < 1 then
            return nil, "Invalid path \"" .. path .. "\""
         end
         table.remove(parts, i)
      else
         i = i + 1
      end
   end

   return parts
end

function filesystem:path (inode)
   local path = ""
   local cur = inode

   repeat
      path = cur.name .. "/" ..  path
      cur = cur.parent
   until cur == nil

   return path
end

function filesystem:insert (inode, parent_path)
   local parts, err = self.segments(parent_path)
   local root = self.inode
   local parent = root

   if parts ~= nil then
      for _, p in ipairs(parts) do
         local child = nil
         local i = 1
         while child == nil and i <= #parent.children do
            if parent.children[i].name == p then
               child = parent.children[i]
            end
            i = i + 1
         end

         if child == nil then
            return false, "Parent \"" .. parent_path .. "\" doesn't exist"
         elseif child.kind ~= "d" then
            return false, "\"" .. parent_path .. "\" is not a directory"
         else
            parent = child
         end
      end
   else
      return false, err
   end

   inode.parent = parent
   table.insert(parent.children, inode)

   return true
end

function filesystem:cd (path)
   local parts, err = self.segments(path)
   local root = self.inode
   local parent = root

   if parts ~= nil then
      for _, p in ipairs(parts) do
         local child = nil
         local i = 1
         while child == nil and i <= #parent.children do
            if parent.children[i].name == p then
               child = parent.children[i]
            end
            i = i + 1
         end

         if child == nil then
            return false, "Directory \"" .. p .. "\" doesn't exist"
         elseif child.kind ~= "d" then
            return false, "\"" .. child.name .. "\" is not a directory"
         else
            parent = child
         end
      end
   else
      return false, err
   end

   Game.FS.pwd = parent

   return true
end

return filesystem
