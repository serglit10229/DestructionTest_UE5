# DestructionTest_UE5

An experiment and a small competition between me and my friend. The goal was to create a voxel-based destruction system that would run as fast as possible. The demo level creates a multi-story building, made out of instanced chunks(33,000+). If the building gets shot, all chunks within a certain radius from the impact location will disappear, creating a hole. An island-searching algorithm will then determine if any part of the building is no longer connected to the root components and need to be destroyed too. This is needed to avoid having large parts of a building just hovering in the air. The system is fully scalable and can support multiple buildings on the same level with up to 2 million chunks in each. The buildings can also be made out of irregular chunks like glass, have different materials, and have a completely abstract shape. In 90% of all possible cases, the entire algorithm runs under 5ms, even with 30 million chunks in the scene, in debug mode with editor overhead. I'm using this system in my other personal project of realistic car physics simulation.

### Single
![Single](Demo(Single).gif)

### Floors
![Floors](Demo(Floors).gif)

### Total Destruction
![Total](Demo(Total).gif)
