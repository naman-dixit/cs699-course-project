Vector = {}
Vector.__index = Vector -- Functions called on vectors will be searched for in this table

function Vector.__add(a, b) -- Addition [a + b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return Vector.New(a.X + b.X, a.Y + b.Y, a.Z + b.Z)
  elseif a_is_vector then
    error("Can not add a Vector to a " .. type(b))
  elseif b_is_vector then
    error("Can not add a " .. type(a) .. " to a Vector")
  else
    error("Vector.__add: This should not happen")
  end
end

function Vector.__sub(a, b) -- Subtraction [a - b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return Vector.New(a.X - b.X, a.Y - b.Y, a.Z - b.Z)
  elseif a_is_vector then
    error("Can not subtract a Vector from a " .. type(b))
  elseif b_is_vector then
    error("Can not subtract a " .. type(a) .. " from a Vector")
  else
    error("Vector.__sub: This should not happen")
  end
end

function Vector.__mul(a, b) -- Scaling/Component-Multiplication [a * b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector
  local a_is_number = type(a) == "number"
  local b_is_number = type(b) == "number"

  if a_is_number and b_is_vector then
    return Vector.New(b.X * a, b.Y * a, b.Z * a)
  elseif a_is_vector and b_is_number then
    return Vector.New(a.X * b, a.Y * b, a.Z * b)
  elseif a_is_vector and b_is_vector then
    return Vector.New(a.X * b.X, a.Y * b.Y, a.Z * b.Z)
  elseif a_is_vector then
    error("Can not multiply a Vector with " .. type(b))
  elseif b_is_vector then
    error("Can not multiply a " .. type(a) .. " with Vector")
  else
    error("Vector.__mul: This should not happen")
  end
end

function Vector.__div(a, b) -- Reverse Scaling [a / b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector
  local b_is_number = type(b) == "number"

  if a_is_vector and b_is_number then
    return Vector.New(a.X / b, a.Y / b, a.Z / b)
  elseif a_is_vector then
    error("Can not divide a Vector by a " .. type(b))
  elseif b_is_vector then
    error("Can not divide a " .. type(a) .. " by a Vector")
  else
    error("Vector.__div: This should not happen")
  end
end

function Vector.__mod (a, b) -- Cross product [a % b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return Vector.New(a.Y * b.Z - a.Z * b.Y,
                      a.Z - b.X - a.X * b.Z,
                      a.X * b.Y - a.Y * b.X)
  elseif a_is_vector then
    error("Can not cross multiply a Vector with " .. type(b))
  elseif b_is_vector then
    error("Can not cross multiply a " .. type(a) .. " with Vector")
  else
    error("Vector.__mod: This should not happen")
  end
end

function Vector.__pow(a, b) -- Dot product [a ^ b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return a.X * a.X + b.Y * b.Y + a.Z * b.Z
  elseif a_is_vector then
    error("Can not cross multiply a Vector with " .. type(b))
  elseif b_is_vector then
    error("Can not cross multiply a " .. type(a) .. " with Vector")
  else
    error("Vector.__mod: This should not happen")
  end
end

function Vector.__unm(a) -- Negation [-a]
  if getmetatable(a) == Vector then
    return Vector.New(-a.X, -a.Y, -a.Z)
  else
    error("Vector.__unm: This should not happen")
  end
end

function Vector.__len(a) -- Length [#a]
  if getmetatable(a) == Vector then
    if a.Y == 0 and a.Z == 0 then
      return 1
    elseif a.Z == 0 then
      return 2
    else
      return 3
    end
  else
    error("Vector.__len: This should not happen")
  end
end

function Vector.__eq(a, b) -- Equality [a == b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return a.X == b.X and a.Y == b.Y and a.Z == b.Z
  elseif a_is_vector then
    error("Can not compare a Vector with " .. type(b))
  elseif b_is_vector then
    error("Can not compare a " .. type(a) .. " with Vector")
  else
    error("Vector.__eq: This should not happen")
  end
end

function Vector.__lt(a, b) -- Less than [a < b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return a:MagSq() < b:MagSq()
  elseif a_is_vector then
    error("Can not compare a Vector with " .. type(b))
  elseif b_is_vector then
    error("Can not compare a " .. type(a) .. " with Vector")
  else
    error("Vector.__lt: This should not happen")
  end
end

function Vector.__le(a, b) -- Less than or equal to [a <= b]
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return a:MagSq() <= b:MagSq()
  elseif a_is_vector then
    error("Can not compare a Vector with " .. type(b))
  elseif b_is_vector then
    error("Can not compare a " .. type(a) .. " with Vector")
  else
    error("Vector.__le: This should not happen")
  end
end

function Vector.__tostring(a)
  if getmetatable(a) == Vector then
    return a.X .. " i + " .. a.Y .. " j + " .. a.Z .. " k"
  else
    error("Vector.__tostring: This should not happen")
  end
end


function Vector.New(x, y, z)
  local x_type = type(x)
  local y_type = type(y)
  local z_type = type(z)

  local x_is_valid = (x_type == "number") or (x_type == "nil")
  local y_is_valid = (y_type == "number") or (y_type == "nil")
  local z_is_valid = (z_type == "number") or (z_type == "nil")

  if x_is_valid and y_is_valid and z_is_valid then
    return setmetatable({X = x or 0, Y = y or 0, Z = z or 0}, Vector)
  else
    error("All arguements should be numbers or nil")
  end
end

function Vector.Distance(a, b)
  local a_is_vector = getmetatable(a) == Vector
  local b_is_vector = getmetatable(b) == Vector

  if a_is_vector and b_is_vector then
    return (b - a):Mag()
  else
    error("Vector.Distance: Arguement must be both vectors")
  end
end

function Vector:Clone()
  if getmetatable(self) == Vector then
    return Vector.New(self.X, self.Y, self.Z)
  else
    error("Argumement should be vector")
  end
end

function Vector:Unpack()
  if getmetatable(self) == Vector then
    return self.X, self.Y, self.Z
  else
    error("Argumement should be vector")
  end
end

function Vector:Mag()
  if getmetatable(self) == Vector then
    return math.sqrt(self.X * self.X + self.Y * self.Y + self.Z * self.Z)
  else
    error("Argumement should be vector")
  end
end

function Vector:MagSq()
  if getmetatable(self) == Vector then
    return self.X * self.X + self.Y * self.Y + self.Z * self.Z
  else
    error("Argumement should be vector")
  end
end

function Vector:Normalize()
  return self / self:Mag()
end

function Vector.Lerp (a, b, factor)
  return a + (factor * (b - a))
end


setmetatable(Vector, { __call = function(_, ...) return Vector.New(...) end })
