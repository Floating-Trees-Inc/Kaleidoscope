# Kaleidoscope

Kaleidoscope is the internal game engine used to make my own games. Built from the ground up by @AmelieHeinrich.

## Building and running

xmake is required.\
To build: `xmake`\
To run: `xmake run {target}`, target can be your app (for instance, demo_app)

## Notes

This engine is **Windows/MacOS only** for now. More platforms may be added in the future however, consoles in priority.

## Projects using Kaleidoscope

- [Seraph](https://github.com/AmelieHeinrich/Seraph): Showcase renderer for my portfolio
- RF: Indie game I'm making with the engine.

## Features

- RHI with D3D12/Vulkan/Metal support. Has bindless, raytracing, mesh shaders, indirect commands
- Unified shader system, write once run everywhere
- Input system
- Asset management
- Custom ImGui/Im3D renderer
- Access to the system sound buffer
- Test suite for various engine modules, auto test runner written in Haskell: ![](.github/test.png)

## Samples

| Samples | Screenshot |
| -------- | ------- |
| [Hello, Triangle](code/samples/triangle/) | ![](.github/s_tri.png) |
| [Hello, Raytraced Triangle](code/samples/raytracing_simple/) | ![](.github/s_ray.png) |
| [ImGui, Simple](code/samples/imgui_simple/) | ![](.github/s_gui.png) |
| [Im3D Example](code/samples/im3d_simple/) | ![](.github/s_im3d.png) |
| [Dev Console](code/samples/dev_console/) | ![](.github/s_devc.png) |
| [Simple mixer](code/samples/mixer/) | (No screenshot) |
