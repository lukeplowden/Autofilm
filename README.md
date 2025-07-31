# Autofilm library

My attempt at a multithreaded vulkan renderer for multichannel video & immersive video installation...
I got multithreaded, multichannel triangles

<img width="1507" height="820" alt="ba80327b-aa6d-41ab-9307-1eb859121509" src="https://github.com/user-attachments/assets/59eedcb6-033d-4aca-a99a-e4d3323ab502" />

### Requirements
- Currently only builds on Windows.
- Require's Vulkan, GStreamer
- The shader paths are hardcoded in at `Autofilm/src/Vulkan/VulkanAPI.cpp` and the shaders will need to be compiled by running `Autofilm/src/Shaders/shaders/compileShaders.bat`

```
git submodule update --init --recursive
```
### Declaration
Lot's of code written based on 
- Vulkan Guide
- Vulkan Tutorial
- The Cherno Game Engine youtube series
- Sascha Willem's Vulkan Examples 
