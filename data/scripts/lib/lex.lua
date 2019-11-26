-- Inspired from
-- https://github.com/Nezteb/lexer-parser-in-lua/blob/master/lexit.lua

local lex = {}

local function chars(str)
   local strc = {}
   for i = 1, #str do
      table.insert(strc, string.sub(str, i, i))
   end
   return strc
end

function lex.isalpha (c)
   if c:len() ~= 1 then
      return false
   elseif c >= "A" and c <= "Z" then
      return true
   elseif c >= "a" and c <= "z" then
      return true
   else
      return false
   end
end

function lex.isdigit(c)
   if c:len() ~= 1 then
      return false
   elseif c >= "0" and c <= "9" then
      return true
   else
      return false
   end
end

function lex.isspace(c)
   if c:len() ~= 1 then
      return false
   elseif c == " " or c == "\t" or c == "\n" or c == "\r" or c == "\f" then
      return true
   else
      return false
   end
end


function lex.NextToken (input, pos)
   local token = ""
   local index = pos

   local function charCurrent ()
      return string.sub(input, index, index)
   end

   local function charNext (str, pos)
      return string.sub(input, index + 1, index + 1)
   end

   local function charNextNext (str, pos)
      return string.sub(input, index + 2, index + 2)
   end

   local function charDrop()
      index = index + 1
   end

   local function charAdd()
      token = token .. charCurrent()
      charDrop()
   end

   while lex.isspace(charCurrent()) == true do
      charDrop()
   end
   if input:len() > 0 then
      if charCurrent() == "\"" then
         charDrop()
         while index <= input:len() and charCurrent() ~= "\"" do
            if charCurrent() == "\\" then
               charDrop()
            end
            charAdd()
         end
      elseif charCurrent() == "&" then
         if charNext() ~= "&" then
            charAdd()
         else
            charAdd()
            charAdd()
         end
      elseif charCurrent() == ">" then
         if charNext() ~= ">" then
            charAdd()
         else
            charAdd()
            charAdd()
         end
      elseif charCurrent() == "<" then
         if charNext() ~= "<" then
            charAdd()
         else
            charAdd()
            charAdd()
         end
      else
         while
            index <= input:len() and
            lex.isspace(charCurrent()) == false and
            charCurrent() ~= "\"" and
            charCurrent() ~= "&" and
            charCurrent() ~= ">" and
            charCurrent() ~= "<"
         do
            if charCurrent() == "\\" then
               charDrop()
            end
            charAdd()
         end
      end
   end

   return token, index
end

return lex
