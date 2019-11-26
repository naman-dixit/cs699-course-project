local ls = {}

function ls.call (tbl)
   local src = {}
   for i = 1, #tbl.args do
      if tbl.args[1]:sub(1, 1) == "/" then
         table.insert(src, {["path"] = tbl.args[i]})
      else
         table.insert(src, {["path"] = Game.FS:path(Game.FS.pwd) .. tbl.args[i]})
      end
   end

   if #tbl.args == 0 then
      table.insert(src, {["path"] = Game.FS:path(Game.FS.pwd)})
   end

   for j = 1, #src do
      local parts, err = Game.FS.segments(src[j].path)
      local root = Game.FS.inode
      local parent = root
      local grandparent = nil
      local grandparent_child_num

      if parts ~= nil then
         for _, p in ipairs(parts) do
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
            elseif child.kind ~= "d" and j < #src then
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

   local accum = 0
   local accum_max=  8
   local out = {}
   local space_optional = ""

   for i = 1, #src do
      table.insert(out, "")

      if i > 1 then
         table.insert(out, "")
      end

      if src[i].inode.kind == "d" then
         if #src > 1 and src[i].inode.kind == "d" then
            out[#out] = out[#out] .. src[i].inode.name .. ":"
            accum = 0
            table.insert(out, "")
         end

         for j = 1, #src[i].inode.children do
            if accum ~= 0 then
               space_optional = " "
            else
               space_optional = ""
            end
            out[#out] = out[#out] .. space_optional .. src[i].inode.children[j].name
            accum = accum + 1
            if accum >= accum_max then
               accum = 0
               table.insert(out, "")
            end
         end
      else
         if accum ~= 0 then
            space_optional = " "
         else
            space_optional = ""
         end
         out[#out] = out[#out] .. space_optional .. src[i].inode.name
         accum = accum + 1
         if accum >= accum_max then
            accum = 0
            table.insert(out, "")
         end
      end

      accum = 0
   end

   if out[#out] == "" then
      table.remove(out, #out)
   end

   return out
end

return ls
