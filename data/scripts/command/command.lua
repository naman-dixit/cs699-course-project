package.path = "data/scripts/command/?.lua;" .. package.path

local command = {}

command["mv"] = require "mv"
command["echo"] = require "echo"
command["ls"] = require "ls"
command["mkdir"] = require "mkdir"
command["touch"] = require "touch"

return command
