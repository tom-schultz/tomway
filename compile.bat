set ShadersIn=%1\shaders
set ShadersOut=%2\shaders

if not exist %ShadersOut% mkdir %ShadersOut%

%VULKAN_SDK%\Bin\glslc.exe shaders\shader.vert -o %ShadersOut%\vert.spv
%VULKAN_SDK%\Bin\glslc.exe shaders\shader.frag -o %ShadersOut%\frag.spv