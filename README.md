# Autofilm

An early stage multithreaded Vulkan rendering engine in C++ designed for multichannel video playback and immersive installations.
![Multichannel Rendering Demo](https://github.com/user-attachments/assets/59eedcb6-033d-4aca-a99a-e4d3323ab502)

## About
This project is a C++ Vulkan renderer that I began developing during my MSc in Creative Computing in 2024. The title Autofilm comes from the intention to expose an API for procedural film editing techniques.

The idea started from work I did for Erika Tan's artwork [_Ancestral (r)Evocations_](https://www.tate.org.uk/visit/tate-modern/display/tanks/museum-x-machine-x-me) as part of [Transforming Collections](https://www.arts.ac.uk/ual-decolonising-arts-institute/projects/transforming-collections) at the Tate Modern. The artist wanted a GIF and Video player to interact with an instrument-playing machine learning model. I created that software [using PyOpenGL](https://github.com/danhearn/ancestral-revocations-installation), but I was inspired to make a general purpose tool for multi-channel installations using a highperformance, modern API. I had wanted to try Vulkan for years, and I was working with [computational film tools](https://lukeplowden.net/projects/the-very-thing/).

## Features
- **Multichannel Output:** Capable of rendering to multiple windows simultaneously by managing multiple Vulkan swapchains.
- **Multithreaded Rendering:** The render loop parallelizes the recording of draw commands. It amounts to one thread per swapchain.

## Getting Started
### Requirements
- Currently only builds on Windows.
- Require's Vulkan, GStreamer
- The shader paths are hardcoded in at `Autofilm/src/Vulkan/VulkanAPI.cpp` and the shaders will need to be compiled by running `Autofilm/src/Shaders/shaders/compileShaders.bat`

### How to run
1. Clone this repository with its submodules: `git clone --recursive git@github.com:lukeplowden/Autofilm.git`
2.  Compile the shaders by running the provided script:
    ```sh
    ./Autofilm/src/Shaders/shaders/compileShaders.bat
    ```
3.  Shader paths are hardcoded within `Autofilm/src/Vulkan/VulkanAPI.cpp`. You may need to update these paths to match your local environment before compiling the C++ source.

## Reflections
- **Architecture**: This project was valuable for making me think more deeply about C++ & OOP design patterns. I followed The Cherno's game engine series quite closely, making sure to understand each aspect. This could have been more balanced with iterative developments, and applying abstractions later. Vulkan doesn't lend itself to abstractions easily, especially the first time round. Since this project I have developed a more personal style, and I would approach this again by stripping back code, implenting features and abstracting once ideas arise. I would probably be more Data Oriented, too. Both Vulkan and GStreamer lend themselves to this style, which I understand more now.
- **Performance**: I successfully implemented multi-threaded command encoding. This forced me to frontload Vulkan's synchronisation primitives in my learning, and I'm glad I did. I expect the parralellisation would be useful in video decoding, too. It was also a preoptimisation, however.
- **Video:** An interesting direction for this project is to implement video decoding in Vulkan itself. I did not get as far as fully integrating GStreamer, but in the case of this project, a custom solution would be a more interesting learning approach.

## Acknowledgements
I couldn't have gotten so far without these resources
- [VkGuide](https://vkguide.dev/)
- [Vulkan Tutorial](https://vulkan-tutorial.com/)
- [The Cherno's Game Engine Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
- [Sascha Willems' Multithreading example](https://github.com/SaschaWillems/Vulkan/blob/master/examples/multithreading/multithreading.cpp)
