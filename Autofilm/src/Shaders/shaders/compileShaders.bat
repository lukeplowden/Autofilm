REM Check if the bin directory exists, if not, create it
if not exist "bin" (
    mkdir "bin"
)

%VULKAN_SDK%/Bin/glslc.exe triangle.vert -o bin/vert.spv
%VULKAN_SDK%/Bin/glslc.exe triangle.frag -o bin/frag.spv
pause