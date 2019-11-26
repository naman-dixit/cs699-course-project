local echo = {}

function echo.call(tbl)
   local args = tbl["args"]
   local text = {}

   local ret_text = table.concat(args, " ")
   return {ret_text}
end

return echo
