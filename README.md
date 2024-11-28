# Autofilm library

My attempt at a multithreaded vulkan renderer for multichannel video & immersive video installation...
I got multithreaded, multichannel triangles

![unnamed](https://git.arts.ac.uk/23036879/VulkanVideoEditor/assets/851/ba80327b-aa6d-41ab-9307-1eb859121509)

[This repository is related](https://git.arts.ac.uk/23036879/Film-labeller)

### Requirements
- Currently only builds on Windows.
- Require's Vulkan, GStreamer
- The shader paths are hardcoded in at `Autofilm/src/Vulkan/VulkanAPI.cpp` and the shaders will need to be compiled by running `Autofilm/src/Shaders/shaders/compileShaders.bat`

```
git submodule update --init --recursive
```
