local inode = {}
inode.__index = inode

function inode.new(name, kind)
   local node = setmetatable({}, inode)
   node.name = name
   node.children = {}
   node.kind = kind
   return node
end

return inode
