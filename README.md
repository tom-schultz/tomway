# Tomway's Game of Life

Welcome to Tomway's Game of Life! This project is my personal testbed for graphics engineering and other systems programming work. It is a C++ implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) rendered with Vulkan. Overkill, you say? Probably, but the goal is to have a simple simulation where I can implement real-time rendering techniques, profile and optimize them, then compare them against previous iterations.

As of v1.0, this project is not particularly user-friendly or intuitive. It's still mainly meant to be built and run from source by me and hasn't been tested on any machine but mine. You can find releases of the project here and on my [Itch.io page](https://katnamag.itch.io/).

#### Controls

| Control  | Binding |
| ------------- | ------------- |
| Toggle mouse/move modes | F1 |
| Save | F2 |
| Load | F3 |
| Move | WASD |
| Look | Mouse movement |
| Step simulation | Space |
| Pause/unpause simulation | L |
| Reset Application | R |

## Design

Tomway's Game of Life is a highly simplified "game engine". As of v1.0 there is no scene graph, no components or game objects, and certainly no editor or tools. The "engine" handles user input to control the camera, the simulation, and the UI. There is ambient music and a few sound effects. I'm not going to detail anything but the simulation and audio here because the other systems are not the point of this project.

### Simulation
The simulation is contained in `simulation_system`. The simulation keeps two frames of state for the game - new and old. Each tick the simulation updates the new frame's state based on the data in the old one. I also allow "teleporting" between the edges of the board, meaning that a cell on the left edge of the board has neighbors to its "left" made up of the right edge of the board and vice versa.

As of v1, the simulation is single-threaded.

### Rendering
Rendering is done in `render_system`. The important part of the API is a single function - `tomway::render_system::draw_frame`. This function takes an updated transform for the camera, conditionally fetches vertices and transfers them, draws the current vertex buffer, and then draws the UI.

## Approaches

### v1
This first implementation was intentionally primitive and simple. My goal was to get something in place as a baseline for additional rendering techniques. Each time the simulation steps, new vertices are generated for every living cell and the entire vertex buffer is updated. The vertices are rendered in chunks no larger than the maximum memory allocation size for the GPU. Each chunk is a single model with no tranform - vertices are placed in world space.  Inefficent, you say? Probably! But the goal for v1 isn't to find the most efficient method of rendering, it's to provide a baseline for comparison.

The vertices are generated in `cell_geometry`. This class exists to bridge the simulation and the rendering system and keep each unaware of the other's concerns. `cell_geometry` iterates through the list of cells and generates new vertices for each living cell based on a set of hard-coded vertices. The generated vertices are grouped into the chunks mentioned above.

Fair warning - as of v1 this class is *internally* a kitchen sink. It is a conscious decision to leave it this way because I believe I will want to significantly restructure it when I start implementing additional rendering methods.

#### Techniques

| Technique  | Implementation |
| ------------- | ------------- |
| Indexed Draws | None |
| Instanced Draws | None |
| Antialiasing | None |
| Lighting | Single directional, local shading |
| Shadows | None |
| CPU culling | None |
| Level of Detail | None |

#### v1 Performance

I profiled v1 with [Tracy](https://github.com/wolfpld/tracy), [NVIDIA NSight Graphics](https://developer.nvidia.com/nsight-graphics), and [NVIDIA Nsight Systems](https://developer.nvidia.com/nsight-systems). This version performs well at rendering static geometry but doesn't handle updating geometry very well. A 600x600 grid with the standard test file (`test/600.json`) and the simulation paused renders at ~2900 FPS with an average of 0.35 milliseconds per frame. Once the simulation is unpaused the framerate drops down to ~2300 FPS and then typically climbs from there as cells die during the simulation.

#### v1 Future Improvements
##### Poor user experience while the simulation is unpaused
~2300 FPS for unpaused simulation is a great number but the issue is with frame time stability. The simulation ticks five times per second and on those frames the frame time jumps from 0.35ms to ~37ms. This huge jump causes visible jerkiness in movement due to a large delta time for the frame. My profiling shows that this is largely caused by vertex generation and vertex transfer to the graphics card. One solution would be to simulate ahead of time then generate and transfer vertex chunks one by one in the intermediate frames. This strategy might reduce framerate but should produce better user experience.

##### Implementing more graphics techniques
See the Techniques table above - Antialiasing, CPU culling, indexed draws, and LOD would probably make a difference here in performance or looks. I will consider instancing to be a different implementation.

##### main() is a mess
Main() is basically a pile of messy glue code. Improving `input_system` to support actions and action callbacks would help.

##### No explanation of controls and no "turbo" button
I want an in-game help screen/button. It's also a PITA to get close to the simulation, so some sort of turbo button that makes you fly fast would be great.

##### The audio system's internal design is bad
SoLoud doesn't implement [Rule of Five](https://en.cppreference.com/w/cpp/language/rule_of_three) for its WAV audio sources so they can't be copied around. I ended up creating my own `audio` class that wraps it and declares `friend audio_system`, as do several other handler-type classes for audio. This feels questionable and needs to be revisited.

##### The input system isn't feature complete
It only supports the specific buttons I need. As mentioned above, an action system for inputs would help me clean up code elsewhere too.

## Libraries Used

| System  | Library | License |
| ------------- | ------------- | ------------- |
| Windowing and Platform  | [SDL](https://github.com/libsdl-org/SDL)  | [Zlib](https://github.com/libsdl-org/SDL/blob/SDL2/LICENSE.txt)  |
| Rendering  | [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)  | [Multiple](https://vulkan.lunarg.com/license/)  |
| Audio  | [SoLoud](https://solhsa.com/soloud/)  | [Zlib](https://solhsa.com/soloud/legal.html)  |
| File Dialog  | [Native File Dialog](https://github.com/mlabbe/nativefiledialog)  | [Zlib](https://github.com/mlabbe/nativefiledialog/blob/master/LICENSE)  |
| JSON | [RapidJSON](https://github.com/Tencent/rapidjson)  | [Multiple](https://github.com/Tencent/rapidjson/blob/master/license.txt)  |
| Profiling | [Tracy](https://github.com/wolfpld/tracy)  | [3-Clause BSD](https://github.com/wolfpld/tracy/blob/master/LICENSE)  |

## Licensing

This project is licensed under the public domain. Please see LICENSE for more info. Vendor software licensing can be seen above. Licenses for the individual assets can be found next to the asset files and are all public domain assets.
