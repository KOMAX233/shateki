# Shateki: Japanese gun shooting game with Voronoi diagram and mesh generation in fracture simulation

## Description
The program shateki simulates flat sheet fracture with Voronoi diagram and mesh generation.

It starts with a thin square sheet in the center of the screen, a dot reticle for approximate aiming, and you can left click mouse to shoot bullets. You can use WASD keys to move around and shoot at the sheet. When the bullet collides with the sheet, it determines the impact point of the collision. 

After finding the impact point, it generates a given number of random points within a 0.2 * 0.2 box as the sites for voronoi diagram generation, and the number is set to 50 in the code as default. It generates a voronoi diagram given the points, bounding box(square sheet), and we get information about sites, vertices and edges list for each voronoi regions. Pressing 2 key and shoot the same time to see voronoi not centered in the bounding box. Pressing F key shows the voronoi points, edges. 

After the voronoi diagram is generated, it generates front and back polygon, and gap vertically between the front and back polygon, so each voronoi diagram gets depth and turns from 2D to 3D. Pressing F key also shows the mesh generated for the 2D voronoid diagram. Each voronoi region is made to a Destructible Object and mesh is generated for each of them. Each Destructible Object is assigned a random color at construction, so that it will be easier to visualize the fracture. After the new objects are created and meshes are assigned to them, the new objects fall and move based on bullet mass, velocity, and the new object's mass. 

There is a 20 * 20 bounding box around the scene. If the bullet or fragments get outside the bounding box, they will disappear and be cleaned. pressing R key can reset the square back complete in the center of the space and reset the camera position and direction back to the initial value.

## Compilation and running
### Build
1. Open Cmake GUI and select the project-dir as the source code, and project-dir/build as the build directory.
2. Configure and generate.
3. Build files will be generated in project-dir/build.
### Run
1. To run the program, open the solution build/shateki.sln in Visual Studio.
2. Right click on shateki in solution explorer, click Set as Startup Project.
3. Click Local Windows Debugger to start the program. 
4. You can also use the src/shateki.exe to run the program in src/ directory.

## Implementation
### Algorithms, data structures, and complexities
#### Voronoi Diagram [1]
- Voronoi diagram is defined for split a 2D plane into regions. 
#### Fortune's Sweepline algorithm [1]
- Fortune's Sweepline algorithm is used for its efficiency. It has a time complexity of O(nlog(n)), which is faster than flip edge algorithm using delaunay triangle.
- Every time a sweepline passes points, arcs are created such that all points should be closest to the center point, site. An arc leaves the line when a circle event occurs, and the arc reaches the farthest part of the circle.
#### Destructible Object
- Destructible Object is defined for destructible 3D objects. It follows physics rules, so it has mass, porition, velocity, and whether affected by gravity. Bullet is set to have no gravity for simplicity. It has update() get called every frame, so position and velocity can be updated here.
- It also has a random color at construction for seeing different objects more clearly.

#### Object destruction algorithm [2]
```Algorithm 1 Object Destruction
1: procedure **Fracture**(Destructible obj, Point hitposition, List objectlist)
   // obj is the object to destroy, hitposition is the impact location on the
   // object, objectlist is where new objects will be added
2:    p ← Randomized point list
3:    Move p towards hitposition
4:
5:
6:    ClipPolygon ← obj.mesh.polygon
      // Get the polygon from the mesh
7:    Diagram ← GenerateDiagram(p, ClipPolygon)
      // Generate clipped Voronoi diagram
8:
9:    for each site ∈ Diagram do
10:       newObj as new Destructible object
11:       newObj.mesh ← GenerateMesh(site.vertices, obj.scale)
12:       Send newObj.mesh to GPU
13:       newObj.mesh.polygon ← site.vertices
14:       objectlist ← newObj
15:
16:    Mark obj for removal
```
- The pseudocode is used in main() in shateki.cpp to generate voronoi diagram pattern, make each site a new Destructible object, and generate mesh for them.

#### Mesh
- Mesh is defined to represent 3D objects. We can draw the 2D plane, and generate mesh for it by giving a list of Vertex and indices. Mesh has a list of Vertices and Indices to draw vertices based on indices. We want to draw point, edge, triangle in 3D, we use the drawAsPoints flag or the number of indices to decide. We use Mesh to bind buffer data.
- Vertex structure stores the position, color. 
#### Mesh Creation algorithm [2]
```Algorithm 2 Mesh Creation
1: procedure **GenerateMesh**(Site Vertices PointList, Scale scale)
   // The received PointList is in a x, y format, it is the job of this
   // function to convert this into three dimensions with z being based
   // on the received scale.
2:
3:    Initialize new VertexList
4:
5:    // This is the front polygon
6:    for each p ∈ PointList do
7:        Initialize v ← (p, scale) as new vertex
8:        v.normal ← ZPositive
9:        Add v to VertexList
10:
11:   // This is the back polygon
12:   for each p ∈ PointList do
13:       Initialize v ← (p, -scale) as new vertex
14:       v.normal ← ZNegative
15:       Add v to VertexList
16:
17:   // These are the gap polygons
18:   for i < PointList.size do
19:       // Using j as a step ahead in point list
20:       j = i + 1
21:       if i = PointList.size - 1 then
22:           j = 0
23:
24:       Initialize v1, v2, v3, v4 as new vertices
25:       v1 ← (PointList[i], scale)
26:       v2 ← (PointList[j], scale)
27:       v3 ← (PointList[j], -scale)
28:       v4 ← (PointList[i], -scale)
29:       v1, v2, v3, v4.normal ← CrossProduct(v1 - v4, ZPositive)
30:       Add v1, v2, v3, v4 to VertexList
31:
32:   // Though any triangulation method can be used to create a face list for
33:   // these vertices, the simple method of connecting every 3 vertices of
34:   // each polygon is used since they are all convex.
35:
36:   mesh ← VertexList Initialize a new mesh
37:   return mesh
```
- The pseudocode is used to define Mesh GenerateMesh(const std::vector<glm::vec2>& pointList, float scale) by adding z value for each vertex, and connect every four vertices to form a face.

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
[1] Voronoi Diagram and Delaunay Triangulation in O(n log n) with Fortune's Algorithm. https://codeforces.com/blog/entry/85638. 

[2] Jerry Ronnegren. 2020. Real Time Mesh Fracturing Using 2D Voronoi Diagrams. https://urn.kb.se/resolve?urn=urn:nbn:se:bth-20161.
