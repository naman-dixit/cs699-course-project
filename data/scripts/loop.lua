package.path = "data/scripts/?.lua;" .. package.path

local command = require "command/command"
local tutorial = require "tutorial"
local lex = require "lib/lex"
local inode = require "lib/inode"
local fs = require "lib/fs"
local getopt = require "command/_getopt"
local commands = require "command/command"
require "lib/table"

Loop = {
   Init = function ()
      Game.Result = true -- The return value of Loop

      Game.Messages = {} -- System messages
      Game.Line = "" -- The line of text being typed in currently
      -- prompt can be a string or nil (in case of output strings)
      Game.Text = {{["prompt"] = "", ["text"] = ""}} -- The text that might be need to rendered this frame

      Game.Here_Doc = false -- Are we in heredoc mode?
      Game.Parse_Complete = false -- Has the entirety of command been inserted and parsed?
      Game.Parse = {{["args"] = ""}} -- Contains parse of the entire inserted command, each entrylike:
      -- {["sequence"] = true/false (&&),
      --  ["background"] = true/false (&),
      --  ["output"] = filename (>/>>),
      --  ["output_append"] = true/false (>>),
      --  ["input"] = filename (<),
      --  ["heredoc"] = true/false (<<),
      --  ["heredoc_str"] = {"","",...} (<<),
      --  ["args"] = command
      -- }

      Game.Output = nil -- Contains the output of any command

      Game.FS = fs.new(inode)
      Game.FS.pwd = Game.FS.inode
      Game.FS:insert(inode.new("vmlinux1", "f"), "/")
      Game.FS:insert(inode.new("vmlinuz2", "f"), "/")
      Game.FS:insert(inode.new("home", "d"), "/")
      Game.FS:insert(inode.new("tmp", "d"), "/")
      Game.FS:insert(inode.new("temp_file.nvidia", "f"), "/tmp/")
      Game.FS:insert(inode.new("user", "d"), "/home/")

      Game.Prompt = "user@cs699 " .. Game.FS:path(Game.FS.pwd) .. " $ "
      Game.Prompt_Show = true

      Game.Cursor_Visible = true
      Game.Cursor_Toggle_At = 500000
      Game.Cursor_Time_Left = Game.Cursor_Toggle_At

      Game.Tutorial_Text = {""} -- Tutorial text
      Game.Tutorial = coroutine.create(tutorial.tutorial)
      Game.Tutorial_In_Progress = true
      Game.Tutorial_In_Progress, Game.Tutorial_Text_Current = coroutine.resume(Game.Tutorial)
      if Game.Tutorial_Text_Current ~= nil then
         for i = 1, #Game.Tutorial_Text_Current do
            table.insert(Game.Tutorial_Text, Game.Tutorial_Text_Current[i])
         end
      end
   end,

   Loop = function (last_frame_time, events)
      Game.Result = true
      Game.Prompt = "user@cs699 " .. Game.FS:path(Game.FS.pwd) .. " $ "

      local messages = Game.Messages

      do -- Process events and convert them to messages
         for _, event in ipairs(events) do
            if event.Device == "Keyboard" then
               table.insert(messages, {Type = "Key", Key = event.Key, State = event.State})
            elseif event.Device == "Text" then
               if event.Type == "Text" then
                  table.insert(messages, {Type = "Text", Text = event.Text})
               elseif event.Type == "Control" then
                  table.insert(messages, {Type = "Text", Control = event.Control})
               end
            end
         end
      end

      local y_min, y_max = Assets.Fonts.Mono.YMin, Assets.Fonts.Mono.YMax
      local y_dim = y_max - y_min
      local newline = false

      for _, msg in ipairs(messages) do -- Process the text input and convert it into line input
         if msg.Type == "Text" then
            if msg.Text ~= nil then
               Game.Line = Game.Line .. msg.Text
            else
               if msg.Control == "Enter" then
                  newline = true
               elseif msg.Control == "Backspace" then
                  if not newline then
                     Game.Line = string.sub(Game.Line, 1, string.len(Game.Line) - 1)
                  end
               end
            end
         end
      end

      if Game.Here_Doc == true then
         Game.Prompt_Show = false
      else
         Game.Prompt_Show = true
      end

      if newline == true then
         if Game.Here_Doc == true then -- Here doc entry
            local index = 0
            for i, p in ipairs(Game.Parse) do
               if p.heredoc_todo then
                  index = i
                  break
               end
            end

            local str_index = #(Game.Parse[index].heredoc_str)
            local maybe_marker = Game.Parse[index].heredoc_str[str_index] .. Game.Line
            if maybe_marker == Game.Parse[index].heredoc_marker and newline then
               Game.Parse[index].heredoc_todo = false
               Game.Here_Doc = false -- We make it true again below if need be
            else
               Game.Parse[index].heredoc_str[str_index] =  Game.Parse[index].heredoc_str[str_index] .. Game.Line
               if newline then
                  table.insert(Game.Parse[index].heredoc_str, "")
               end
            end

            for i, p in ipairs(Game.Parse) do
               if p.heredoc_todo then
                  Game.Here_Doc = true
                  break
               end
            end
         else -- Parsing
            local parse = Game.Parse
            local pos = 1
            local token, pos = lex.NextToken(Game.Line, pos)
            while token ~= "" do
               if token == "&" then
                  parse[#parse].background = true
               elseif token == "&&" then
                  table.insert(parse, {["args"] = ""})
                  parse[#parse].sequence = true
               elseif token == ">" then
                  parse[#parse].output, pos = lex.NextToken(Game.Line, pos)
                  parse[#parse].output_append = false
               elseif token == ">>" then
                  parse[#parse].output, pos = lex.NextToken(Game.Line, pos)
                  parse[#parse].output_append = true
               elseif token == "<" then
                  parse[#parse].input, pos = lex.NextToken(Game.Line, pos)
               elseif token == "<<" then
                  parse[#parse].heredoc = true
                  parse[#parse].heredoc_todo = true
                  parse[#parse].heredoc_str = {""}
                  parse[#parse].heredoc_marker, pos = lex.NextToken(Game.Line, pos)
                  Game.Here_Doc = true
               else
                  parse[#parse].args = parse[#parse].args .. " " .. token
               end

            token, pos = lex.NextToken(Game.Line, pos)
            end
            Game.Parse = parse
            Game.Parse_Complete = true
         end
      end

      if Game.Parse_Complete == true and Game.Here_Doc == false then
         local parse, parse_error = getopt.parse(Game.Parse[1].args)
         if parse == nil then
            Game.Output = {parse_error}
         else
            -- local command = Game.Parse[1].args:gmatch("%S+")()
            local command = parse.command

            if command == nil then
               Game.Output = {"Invalid command"}
            elseif command == "" then
            elseif command == "exit" then
               Game.Result = false
            elseif command == "cd" then
               local success, err
               if #parse.args >= 1 then
                  if parse.args[1]:sub(1, 1) == "/" then
                     success, err = Game.FS:cd(parse.args[1])
                  else
                     success, err = Game.FS:cd(Game.FS:path(Game.FS.pwd) .. parse.args[1])
                  end

                  if not success then
                     Game.Output = {err}
                  end
               end
            elseif commands[command] ~= nil then
               local out, err = commands[command].call(parse)
               if out == nil then
                  Game.Output = {err}
               else
                  Game.Output = out
               end
            else
               Game.Output = {"Error: command \"" .. command .. "\" not found"}
            end
         end

         Game.Parse = {{["args"] = ""}}
         Game.Parse_Complete = false

         -- Game.Output = {}
         -- local out = table.show(Game.FS.inode)
         -- for line in out:gmatch("[^\n]+") do
         --    table.insert(Game.Output, line)
         -- end
      end

      if Game.Here_Doc == true or Game.Prompt_Show == false then
         Game.Prompt_Show = false
      else
         Game.Prompt_Show = true
      end


      do -- Game.Text
         Game.Text[#Game.Text].text = Game.Line
         local prompt = nil
         if Game.Prompt_Show then
            Game.Text[#Game.Text].prompt = Game.Prompt
            prompt = Game.Prompt
         end

         if Game.Output ~= nil then
            for _, l in ipairs(Game.Output) do
               table.insert(Game.Text, {["prompt"] = nil, ["text"] = l})
            end
            Game.Output = nil
         end

         if newline then
            table.insert(Game.Text, {["prompt"] = prompt, ["text"] = ""})
         end

         if #Game.Text > math.floor(2/y_dim) then
            local diff = #Game.Text - math.floor(2/y_dim)
            for i = 1, diff do
               table.remove(Game.Text, i)
            end
         end

         if Game.Cursor_Visible == true then
            Game.Text[#Game.Text].text = Game.Text[#Game.Text].text .. "_"
         end

         Game.Cursor_Time_Left = Game.Cursor_Time_Left - last_frame_time
         if Game.Cursor_Time_Left <= 0 then
            if Game.Cursor_Visible == true then
               Game.Cursor_Visible = false
               Game.Cursor_Time_Left = Game.Cursor_Toggle_At
            else
               Game.Cursor_Visible = true
               Game.Cursor_Time_Left = Game.Cursor_Toggle_At
            end
         end
      end

      do -- Convert Line input into renderable text and render it
         local render_text = {}

         local line_extra = 0
         for line_num, line_tab in ipairs(Game.Text) do
            local line = line_tab.text
            local prompt = line_tab.prompt
            local word = ""
            local x = 0

            local function chars(str)
               local strc = {}
               for i = 1, #str do
                  table.insert(strc, string.sub(str, i, i))
               end
               return strc
            end

            table.insert(render_text, {["prompt"] = prompt, ["text"] = "", ["x"] = 0})

            if prompt ~= nil then
               render_text[#render_text].prompt = prompt
               x = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono, prompt)
               render_text[#render_text].x = x
               prompt = ""
            end
            word = ""
            local line_char = chars(line)

            for i = 1, #line do
               local c = line_char[i]

               if c == " " then
                  if word == "" then
                     local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono,
                                                                            " ")
                     x = x + x_inc
                     if x >= 1 then
                        table.insert(render_text, {["prompt"] = prompt, ["text"] = " "})
                        x = 0
                     else
                        render_text[#render_text].text = render_text[#render_text].text .. " "
                     end
                  else
                     local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono,
                                                                            word)
                     if x + x_inc >= 1 then
                        x = 0
                        table.insert(render_text, {["prompt"] = prompt, ["text"] = word})
                     else
                        render_text[#render_text].text = render_text[#render_text].text .. word
                     end

                     x = x + x_inc

                     local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono,
                                                                            " ")
                     if x + x_inc >= 1 then
                        table.insert(render_text, {["prompt"] = prompt, ["text"] = ""})
                        x = 0
                     else
                        render_text[#render_text].text = render_text[#render_text].text .. " "
                     end
                     x = x + x_inc

                     word  = ""
                  end
               else
                  word = word .. c
               end
            end

            if word ~= "" then
               local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono, word)
               if x + x_inc >= 1 then
                  x = 0
                  table.insert(render_text, {["prompt"] = prompt, ["text"] = word})
               else
                  render_text[#render_text].text = render_text[#render_text].text .. word
               end
               x = x + x_inc
            end
         end

         local render_text_begin = 1
         local render_upper_bound = math.floor(2/y_dim) - 1
         if #render_text > render_upper_bound then
            local diff = #render_text - render_upper_bound
            render_text_begin = diff
         end

         for i = render_text_begin, #render_text do
            if render_text[i].prompt ~= nil then
               Engine.Functions.RenderText(Assets.Fonts.Mono, render_text[i].prompt,
                                           Vector(0, 1 - (i - render_text_begin + 1) * y_dim, 0),
                                           Color(0.11, 1, 0.39, 1))
            end

            local text_color
            if render_text[i].prompt ~= nil then
               text_color = Color(0.11, 1, 0.09, 1)
            else
               text_color = Color(0.41, 1, 0.09, 1)
            end
            Engine.Functions.RenderText(Assets.Fonts.Mono, render_text[i].text,
                                        Vector(render_text[i].x,
                                               1 - (i - render_text_begin + 1) * y_dim,
                                               0),
                                        text_color)
         end
      end



      do -- Process tutorial text
         if newline then
            Game.Tutorial_In_Progress, Game.Tutorial_Text_Current = coroutine.resume(Game.Tutorial, Game.Line)
         else
            Game.Tutorial_Text_Current = nil
         end

         if Game.Tutorial_In_Progress == false then
            Game.Tutorial_Text_Current = nil
         end

         if Game.Tutorial_Text_Current ~= nil then
            for i = 1, #Game.Tutorial_Text_Current do
               table.insert(Game.Tutorial_Text, Game.Tutorial_Text_Current[i])
            end
         end

         if #Game.Tutorial_Text > math.floor(2/y_dim) then
            local diff = #Game.Tutorial_Text - math.floor(2/y_dim)
            for i = 1, diff do
               table.remove(Game.Tutorial_Text, i)
            end
         end
      end

      do -- Convert tutorial text into renderable text and render it
         local render_text = {}

         local line_extra = 0
         for line_num, line in ipairs(Game.Tutorial_Text) do
            local word = ""
            local x = -1

            local function chars(str)
               local strc = {}
               for i = 1, #str do
                  table.insert(strc, string.sub(str, i, i))
               end
               return strc
            end

            local line_char = chars(line)

            table.insert(render_text, "")

            for i = 1, #line do
               local c = line_char[i]

               if c == " " then
                  if word == "" then
                     local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono,
                                                                            " ")
                     x = x + x_inc
                     if x >= 0 then
                        table.insert(render_text, " ")
                        x = -1
                     else
                        render_text[#render_text] = render_text[#render_text] .. " "
                     end
                  else
                     local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono,
                                                                            word)
                     if x + x_inc >= 0 then
                        x = -1
                        table.insert(render_text, word)
                     else
                        render_text[#render_text] = render_text[#render_text] .. word
                     end

                     x = x + x_inc

                     local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono,
                                                                            " ")
                     if x + x_inc >= 0 then
                        table.insert(render_text, "")
                        x = -1
                     else
                        render_text[#render_text] = render_text[#render_text] .. " "
                     end
                     x = x + x_inc

                     word  = ""
                  end
               else
                  word = word .. c
               end
            end

            if word ~= "" then
               local x_inc = Engine.Functions.RenderGetTextDimensions(Assets.Fonts.Mono, word)
               if x + x_inc >= 0 then
                  x = -1
                  table.insert(render_text, {["prompt"] = prompt, ["text"] = word})
               else
                  render_text[#render_text] = render_text[#render_text] .. word
               end
               x = x + x_inc
            end
         end

         local render_text_begin = 1
         local render_upper_bound = math.floor(2/y_dim) - 1
         if #render_text > render_upper_bound then
            local diff = #render_text - render_upper_bound
            render_text_begin = diff
         end

         for i = render_text_begin, #render_text do
            -- local text_color = Color(0.41, 1, 0.09, 1)
            local text_color = Color(0.09, 0.41, 1, 1)
            Engine.Functions.RenderText(Assets.Fonts.Mono, render_text[i],
                                        Vector(-1,
                                               1 - (i - render_text_begin + 1) * y_dim,
                                               0),
                                        text_color)
         end
      end

      do -- Finalize
         if newline then
            Game.Line = ""
         end

         Game.Messages = {}
      end


      return Game.Result
   end,
}
