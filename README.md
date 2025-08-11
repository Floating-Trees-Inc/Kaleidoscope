# Kaleidoscope

Kaleidoscope is the internal game engine used to make games at Floating Trees. Built from the ground up by @AmelieHeinrich.

## Building and running

xmake is required.\
To build: `xmake`\
To run: `xmake run {target}`, target can be your app (for instance, demo_app)

## Notes

This engine is **Windows only** for now. More platforms may be added in the future however, MacOS being one of my priorities.
The mindset I have for this engine is to leave rendering and world management up to the game. Kaleidoscope is just here to provide the tools to help -- but the way you make and render your game or program is up to you.

## Projects using Kaleidoscope

- [Seraph](https://github.com/AmelieHeinrich/Seraph): Showcase renderer for my portfolio
- RF: Indie game I'm making with the engine.

## Features

- RHI with D3D12/Vulkan support. Has bindless, raytracing, mesh shaders, indirect commands
- Input system
- Asset management
- Graphics utilities (resource recycling)
- Custom ImGui renderer
- Basic access to the system sound buffer
- Test suite for various engine modules, auto test runner written in Haskell
