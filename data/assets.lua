Assets = {
  Scripts = {},
  Fonts = {},
  Shaders = {},
}


function Assets:Script (table)
   self.Scripts[table.ID] = {}
   Engine.Functions.AssetLoadScript(self.Scripts[table.ID], table.ScriptPath)
end

function Assets:TrueTypeFont (table)
  local font_size = 3 * table.FontScreenSize
  local scaling_factor = 1/500

  self.Fonts[table.ID] = {}
  Engine.Functions.AssetLoadTrueTypeFont(self.Fonts[table.ID], table.FontPath,
                                         font_size, scaling_factor,
                                         table.VertexPath, table.FragmentPath)

  local _, y_min, y_max = Engine.Functions.RenderGetTextDimensions(self.Fonts[table.ID], "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
  self.Fonts[table.ID].YMin = y_min
  self.Fonts[table.ID].YMax = y_max
end

Assets:TrueTypeFont{
   ID = "Mono",
   FontPath = "data/fonts/TerminusTTF-4.47.0.ttf",
   FontScreenSize = 8, -- Acceptable range ≈ [5, 35] (depends on font)
   VertexPath = "data/shaders/text.vert",
   FragmentPath = "data/shaders/text.frag",
}

Assets:Script{
   ID = "Vector",
   ScriptPath = "data/scripts/lib/vector.lua"
}

Assets:Script{
   ID = "Color",
   ScriptPath = "data/scripts/lib/color.lua"
}

Assets:Script{
   ID = "Loop",
   ScriptPath = "data/scripts/loop.lua"
}
