-- Inspired by
-- http://lua-users.org/wiki/AlternativeGetOpt

-- getopt_alt.lua

-- getopt, POSIX style command line argument parser
-- param arg contains the command line arguments in a standard table.
-- param options is a string with the letters that expect string values.
-- returns a table where associated keys are true, nil, or a string value.
-- The following example styles are supported
--   -a one  ==> opts["a"]=="one"
--   -bone   ==> opts["b"]=="one"
--   -c      ==> opts["c"]==true
--   --c=one ==> opts["c"]=="one"
--   -cdaone ==> opts["c"]==true opts["d"]==true opts["a"]=="one"
-- note POSIX demands the parser ends at the first non option
--      this behavior isn't implemented.local getopt = {}

-- getopt.parse("asd -a g -t -yui --as=5454 a.c", "a") =>
-- { ["pars"] = { ["a"] = "g",
--                ["t"] = true,
--                ["y"] = true,
--                ["u"] = true,
--                ["i"] = true,
--                ["as"] = 5454 },
--   ["args"] = {"a.c"},
--   ["command"] = "asd" }

local getopt = {}

local function isalnum (c)
    local allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    local find = string.find(allowed, c)

    if find then
       return true
    else
       return false
    end
end

function getopt.parse(arg_str, options)
   local arg = {}

   if options == nil then
      options = ""
   end

   for w in arg_str:gmatch("%S+") do
      table.insert(arg, w)
   end

   local tab = {["pars"] = {}, ["args"] ={}}
   local arg_consumed = false
   local only_args_next = false

   for i, w in ipairs(arg) do
      if arg_consumed then
         arg_consumed = false
         -- continue
      elseif i == 1 then
         tab.command = w
         -- continue
      else
         if string.sub(w, 1, 2) == "--" and not only_args_next then
            if string.len(w) == 2 then
               only_args_next = true
               -- continue
            else
               local x = string.find(w, "=", 1, true )
               if x then
                  local key = string.sub(w, 3, x-1)
                  local value = string.sub(w, x+1)
                  tab.pars[key] = value
               else
                  local key = string.sub(w, 3)
                  tab.pars[key] = true
               end
            end
         elseif string.sub(w, 1, 1) == "-" and not only_args_next then
            local y = 2
            local l = string.len(w)
            local jopt
            while (y <= l) do
               jopt = string.sub(w, y, y)
               if isalnum(jopt) == false then
                  return nil, "Wrong parameter \"" .. jopt .. "\" used"
               end
               if string.find(options, jopt, 1, true) then
                  if y < l then
                     local key = jopt
                     local value = string.sub(w, y + 1)
                     tab.pars[key] = value
                     y = l
                  else
                     local narg = arg[i + 1]
                     if string.sub(narg, 1, 2) == "--" or string.sub(narg, 1, 1) == '-' then
                        return nil, "No arguments provided for " .. jopt
                     else
                        local key = jopt
                        local value = narg
                        tab.pars[key] = value
                        arg_consumed = true
                     end
                  end
               else
                  local key = jopt
                  tab.pars[key] = true
               end
               y = y + 1
            end
         else
            tab.args[#tab.args + 1] = w
         end
      end
   end

   return tab
end

function getopt.compare(sys_table, user_table, order)
   if sys_table["command"] ~= user_table["command"] then
      return false
   end

   local sys_args = sys_table["args"]
   local user_args = user_table["args"]

   if #user_args ~= #sys_args then
      return false
   else
      if order == true then
         for i = 1, #sys_args do
            if sys_args[i] ~= user_args[i] then
               return false
            end
         end
      else
         for i = 1, #sys_args do
            local exists = false
            for j = 1, #user_args do
               if sys_args[i] == user_args[j] then
                  exists = true
               end
            end
            if exists == false then
               return false
            end
         end

         for i = 1, #user_args do
            local exists = false
            for j = 1, #sys_args do
               if user_args[i] == sys_args[j] then
                  exists = true
               end
            end
            if exists == false then
               return false
            end
         end
      end
   end

   local sys_pars = sys_table["pars"]
   local user_pars = user_table["pars"]

   do
      for k, v in pairs(sys_pars) do
         local exists = false
         for k2, v2 in pairs(user_pars) do
            if k == k2 and v == v2 then
               exists = true
            end
         end
         if exists == false then
            return false
         end
      end

      for k, v in pairs(user_pars) do
         local exists = false
         for k2, v2 in pairs(sys_pars) do
            if k == k2 and v == v2 then
               exists = true
            end
         end
         if exists == false then
            return false
         end
      end
   end

   return true
end


return getopt
