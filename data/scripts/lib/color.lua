Color = {}
Color.__index = Color -- Functions called on vectors will be searched for in this table

setmetatable(Color, { __call = function(_, ...) return Color.NewFromNorm(...) end })

function Color.ArgNotNumber(arg_str, arg, low, high)
  if type(arg) ~= "number" then
    return arg_str .. " has to be a number"
  else
    if not (arg <= high and arg >= low) then
      return arg_str .. " has to be between " .. low .. " and " .. high
    end
  end

  return nil
end

function Color.ArgNotNumberOrNil(arg_str, arg, low, high)
  if type(arg) ~= "number" and type(arg) ~= "nil" then
    return arg_str .. " has to be a number or nil"
  else
    if type(arg) == "number" then
      if not (arg <= high and arg >= low) then
        return arg_str .. " has to be between " .. low .. " and " .. high
      end
    end
  end

  return nil
end


function Color.NewFromNorm(r, g, b, a)
  local r_t = Color.ArgNotNumber("R", r, 0, 1)
  if r_t then error(r_t) end

  local g_t = Color.ArgNotNumber("G", g, 0, 1)
  if g_t then error(g_t) end

  local b_t = Color.ArgNotNumber("B", b, 0, 1)
  if b_t then error(b_t) end

  local a_t = Color.ArgNotNumberOrNil("A", a, 0, 1)
  if a_t then error(a_t) end

  return setmetatable({R = r, G = g, B = b, A = a or 1}, Color)
end

function Color.NewFromHex(r, g, b, a)
  local r_t = Color.ArgNotNumber("R", r, 0, 255)
  if r_t then error(r_t) end

  local g_t = Color.ArgNotNumber("G", g, 0, 255)
  if g_t then error(g_t) end

  local b_t = Color.ArgNotNumber("B", b, 0, 255)
  if b_t then error(b_t) end

  local a_t = Color.ArgNotNumberOrNil("A", a, 0, 255)
  if a_t then error(a_t) end

  return setmetatable({R = r / 255, G = g / 255, B = b / 255, A = a / 255 or 1}, Color)
end
