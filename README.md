# Shateki: Japanese gun shooting game with Voronoi diagram and mesh generation in fracture simulation

## Description
The program shateki simulates flat sheet fracture with Voronoi diagram and mesh generation.

It starts with a thin square sheet in the center of the screen, a dot reticle for approximate aiming, and you can left click mouse to shoot bullets. You can use WASD keys to move around and shoot at the sheet. When the bullet collides with the sheet, it determines the impact point of the collision. 

After finding the impact point, it generates a given number of random points within a 0.2 * 0.2 box as the sites for voronoi diagram generation, and the number is set to 50 in the code as default. It generates a voronoi diagram given the points, bounding box(square sheet), and we get information about sites, vertices and edges list for each voronoi regions. Pressing 2 key and shoot the same time to see voronoi not centered in the bounding box. Pressing F key shows the voronoi points, edges. 

After the voronoi diagram is generated, it generates front and back polygon, and gap vertically between the front and back polygon, so each voronoi diagram gets depth and turns from 2D to 3D. Pressing F key also shows the mesh generated for the 2D voronoid diagram. Each voronoi region is made to a Destructible Object and mesh is generated for each of them. Each Destructible Object is assigned a random color at construction, so that it will be easier to visualize the fracture. After the new objects are created and meshes are assigned to them, the new objects fall and move based on bullet mass, velocity, and the new object's mass. 

There is a 20 * 20 bounding box around the scene. If the bullet or fragments get outside the bounding box, they will disappear and be cleaned. pressing R key can reset the square back complete in the center of the space and reset the camera position and direction back to the initial value.

## Compilation and running
1. To run the program, open the solution build/shateki.sln in Visual Studio.
2. Right click on shateki in solution explorer, click Set as Startup Project.
3. Click Local Windows Debugger to start the program. 
4. You can also use the src/shateki.exe to run the program in src/ directory.

## Implementation
### Algorithms, data structures, and complexities
- Voronoi Diagram
...............
- Fortune's Sweepline algorithm
..............., nlog(n), faster than flip edge algorithm using delaunay triangle
- Destructible Object
...............
- Mesh
...............
- Object destruction algorithm
...............
- Mesh Creation algorithm
...............

### Platform and system dependence, external libraries
- The program is written and tested on Microsoft Windows 11 Home, Version 10.0.22631 Build 22631, with NVIDIA GeForce RTX 4060 Laptop GPU, in Microsoft Visual Studio Community 2022 (64-bit) Version 17.9.6. 
- External libraries that are used are glew-1.13.0, glfw-3.1.2, glm-0.9.7.1, rpavlik-cmake-modules-fe2273.

### Input/output, control
- Debug outputs are all commented out for performance.
- R key for resetting to complete square sheet, initial camera position and horizontal and vertical angles. If new objects are already created, they will be removed.
- W/S key for moving forth and back, so you get closer or farther to the sheet.
- A/D key for moving left and right.
- 2 key while shooting the sheet to get voronoi diagram from random points not bounded by 0.2 * 0.2 box.
- F key for showing voronoi sites, edges, and square with depth.
- Left click mouse for shooting bullet. Multiple bullets can be fired when holding left click.

### Data and code sources, the re-use and adaptation of existing code, any acknowledgment of external sources.
The controls.hpp/cpp, shaders.hpp/cpp in common/ are made by opengl-tutorial.org.

### Caveats, assumptions
- Assume the velocity of bullet is 10.0 magnitude to camera direction.
- Assume conservation of momentum is used to determine the velocity of fragments after collision.
- Fragments velocity is scaled down by 0.1 to avoid too fast speed because bullet velocity is fast and fragments are light.
- A new bullet is able to be fired every time left mouse is clicked. 
- A 20 * 20 bounding box is used to remove old bullets and square fragments. If they exceed the box, remove it from the bullet or destructible objects vector.
- Assume bullet is not affected by gravity, and a flag is added for destructible object to decide if it has gravity.
- Assume Destructible objects that have graivty only move to the direction of gravity after it's destructed and voronoi diagram is generated.
- Assume bullet won't break, and won't collide with or break each other. New fragment objects won't collide and break each other.
- Assume bullet initial position is cameraposition moving forward 1.0.
- Assume an Destructible object can only be fractured once, so shooting on fragments won't break them.
- Assume random color are assigned at Destructible object construction to see the fragments more clearly.
- Assume the aiming dot size is 0.01 * 0.01.
- Before bullet and collision are implemented, impact point is assumed to be (0, 0), the center of the 2D plane.
- Voronoi diagram is implemented with Fortune's Sweepline algorithm.
- Assume camera's initial position is (0, 0, 3), horizontal angle is 3.14, and verticle angle is 0.0.
- Assume the number of random points to generate and used as voronoi diagram's sites is 50.
- Assume random points are generated with x and y coordinates in the range of [-0.1, 0.1] to the impact point.
- Assume voronoi point size is 3.0.
<!-- - jc voronoi for now -->
- Assume the z value of square sheet is in the range of [-0.1, 0.1].
- Assume gravity is 0.1 to negative y.
- Assume fps is 120.
- Assume the mass of each objects created from voronoi diagram is the area of region because they are all flat sheet with same depth.
- Shooting on very side of the object sometimes won't break it and have to move a little to the center.
## Objectives

1. Detect collisions between the bullet and the 2D sheet, and find the impact location (completed).

2. Construct Voronoi diagram generation function with a given number of seeds (completed).

3. Generate a list of random points on the 2D sheet, move them towards the impact point, and generate Voronoi diagram based on the points (completed).

4. Create new objects for each cell in the Voronoi diagram (completed).

5. Set depth Z in the direction of normals with given value to every seed and add the new vertice to vertice list (completed).

6. Create gap polygons between the front and back surface by setting 4 vertices and add to vertice list (completed).

7. Connect every 3 vertices to create mesh and assign the mesh to the new objects, add new objects and remove old objects from object list(completed).

8. Make new objects move following velocity and update every loop (completed).

## References





<!-- 
## Specification



The Voronoi diagram generation in the provided code is based on Fortune's algorithm, which is an efficient, sweep-line algorithm for generating Voronoi diagrams. Fortune's algorithm has a time complexity of \(O(n \log n)\), making it suitable for handling a large number of points. Below, I'll explain the key concepts and steps involved in Fortune's algorithm as implemented in the provided code.

### Key Concepts

1. **Voronoi Diagram**: A partitioning of a plane into regions based on distance to a specific set of points. Each region corresponds to one point and consists of all locations closer to that point than to any other.

2. **Sweep Line**: An imaginary line that sweeps across the plane from top to bottom. As the line moves, it processes events (either site events or circle events).

3. **Beach Line**: A complex structure maintained by the algorithm that represents the boundary of the regions being formed by the sweep line. It consists of parabolic arcs, where each arc corresponds to a site that has been processed but whose Voronoi region has not been fully completed.

4. **Priority Queue**: Used to manage the events. Site events are associated with the sites to be processed, and circle events are associated with the points where three parabolic arcs meet.

### Steps of the Algorithm

1. **Initialization**: 
   - Sites (points) are sorted and stored in a priority queue.
   - The beach line is initialized with two dummy nodes representing the start and end.

2. **Processing Events**:
   - The main loop processes events from the priority queue until it is empty.
   - **Site Event**: Occurs when the sweep line encounters a new site (point).
     - A new arc is added to the beach line.
     - New edges are created between the new site and neighboring arcs.
     - Potential circle events are calculated and added to the priority queue.
   - **Circle Event**: Occurs when three arcs meet, indicating a vertex of the Voronoi diagram.
     - The middle arc is removed from the beach line.
     - The circle event creates a new vertex for the Voronoi diagram and finalizes edges.
     - Potential new circle events are calculated and added to the priority queue.

3. **Finalization**: 
   - After all events are processed, the remaining arcs in the beach line are used to finalize the remaining edges of the Voronoi diagram.
   - Special handling is done to ensure all edges are within the bounding box.

4. **Clipping and Filling Gaps**:
   - The edges of the Voronoi diagram are clipped to the bounding box.
   - Any gaps at the borders are filled to ensure the diagram is complete and correctly bounded.

### Core Functions

- **`jcv_site_event`**: Handles the addition of a new site event.
- **`jcv_circle_event`**: Handles the removal of an arc and creation of a circle event.
- **`jcv_halfedge_new`**: Manages the creation and linking of new half-edges in the beach line.
- **`jcv_pq_*` Functions**: Manage the priority queue operations.
- **`jcv_edge_clipline`**: Clips the edges to the bounding box.
- **`jcv_finishline`**: Finalizes edges by intersecting them with the bounding box.

### Summary

Fortune's algorithm efficiently constructs a Voronoi diagram by sweeping a line across the plane and maintaining the beach line and a priority queue of events. The provided code implements this algorithm with additional functionality for clipping and managing memory, ensuring the generated diagram is correctly bounded and efficiently handled. -->
