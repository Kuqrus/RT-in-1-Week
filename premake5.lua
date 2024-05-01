-- premake5.lua
workspace "RTinOneWeekend"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RTinOneWeekend"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "RTinOneWeekend"