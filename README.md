# Shateki: Japanese gun shooting game with Voronoi diagram and mesh generation in fracture simulation

# Specification
- opengl is used
- voronoi diagram using sweep line is used
- object2 assume impact point is center of the front surface of the sheet
- seeing 36 vertices because each face is split into triangle which is 3 vertices, 6 faces, 12 triangles, 36 vertices.
- from camera direction, z is positive, front is z=0, back is z=-0.2
(-1,1) (1,1)
(-1,-1) (1,-1)

assume camera initial position 0,0,3, horizontal angle 3.14f, verticle 0.0f

task3
- assume number of sites to randomly generate is 50
- move random points in the -0.1, 0.1 range to the impact point in x and y
- assume point size is 3.0
task 5
- assume the depth of square is 0.1(given value)
task 6
- add boundry edges gap polygon and vertices if x or y is equal to size of square 0.5
task 4
- after getting the intersections, check other vertices on the same edge, if it has other vertices on the same edge that's not corner 0.5/-0.5, connect to the one next to them, if they don't have neighbor on the same edge connect them to the corner, if theres only one vertex intersected on a square edge, genreate one to the left and one to the right. 

create a new edge list
add the new edges to the edge list with the edges from the output of compute voronoi edges

create a new point list, add the vertices of all edges

now all edges should be able to used for construct a region, start from one vertex, check the edges to get the next point connect to it, stop when find edge that connect back to the first point, now the edges and vertices that can a circle is used as one destructible object.

repeat until sites number of new objects have been found, then pass the objects to generate mesh

voronoi: vd dt duality
fortune's algorithm vs delaunay traingle (maybe? v->f, e->e, f->v)
Bowyer-Watson algorithm to generate dt
https://www.mtmckenna.com/posts/2023/06/16/voronoi-diagrams
https://cartography-playground.gitlab.io/playgrounds/triangulation-delaunay-voronoi-diagram/

jc voronoi for now


task 1:
When a sheet is hit by a bullet, the pieces of the sheet will typically have velocities influenced by the impact of the bullet. The velocity of each piece can be computed based on several factors:

Initial Velocity of the Bullet: The initial velocity of the bullet imparts kinetic energy to the sheet, which is then distributed among the fragments.
Point of Impact: The location where the bullet hits the sheet will affect how the energy is distributed. Pieces closer to the impact point will generally have higher velocities.
Direction of Impact: The direction from which the bullet strikes the sheet will influence the initial velocities of the fragments.
Mass and Size of Fragments: Smaller and lighter fragments will generally have higher velocities compared to larger and heavier fragments.

task 8
assume gravity is 0.1
increase fps, decrease delta time to slow the movement
assume 120 fps

task 8
When a bullet impacts a sheet, the velocity of each fragment is influenced by:

The initial kinetic energy of the bullet. how fast
The proximity of the fragment to the impact point. how close
The direction and angle of the bullet's impact. angle
The mass and size of the fragments.

conservation of momentum 
motion of system of particles

? assign mass to each des object: assume mass is area of region because flat sheet

task 1
able to shoot a new bullet everytime left mouse is clicked? and add a bounding box to remove the old bullet and square fragment , if they exceed the box, set it to be the box to be size 10

assume aiming dot size 0.01f

assume bullet is not affected by gravity, add flag to check an object has gravity

assume bullet won't break

assume bullet initial position is cameraposition.z + 1.0f so won't see big bullet in the front

detect collision between bullet and other destructible objects from square , and change impact point to the point they touch each other

why after collision detected, it's keep printing collision detected and creating new voronoi diagram? it should only create once bullet touches square, and make them fall?
why the new objects are not falling with gravity? can you fix them?


also remove this, otherwise it cant hit and gernate new objects
                    // Remove the bullet after collision
                    //bullet.velocity = glm::vec3(0.0f);
                    //bullet.affectedByGravity = false;
and can you update r key press to reset the square and change camera back to initial position and angle?

Ensure collision detection only occurs once by adding a flag to the bullet indicating it has already collided.
Apply gravity to new fragments correctly.
Reset camera position and angle on 'R' key press.

assume a square is only hit once

assume sqaure can only be hit from the front surface

not move with momentum, move in the direction from bullet position to each sites position, plus gravity

assign random color at object construction to see the fragments more clearly

## Objectives

1. Detect collisions between the bullet and the 2D sheet, and find the impact location (completed).

2. Construct Voronoi diagram generation function with a given number of seeds (completed).

3. Generate a list of random points on the 2D sheet, move them towards the impact point, and generate Voronoi diagram based on the points (completed).

4. Create new objects for each cell in the Voronoi diagram (completed).

5. Set depth Z in the direction of normals with given value to every seed and add the new vertice to vertice list (completed).

6. Create gap polygons between the front and back surface by setting 4 vertices and add to vertice list (completed).

7. Connect every 3 vertices to create mesh and assign the mesh to the new objects, add new objects and remove old objects from object list(completed).

8. Make new objects move following velocity and update every loop (completed).



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

Fortune's algorithm efficiently constructs a Voronoi diagram by sweeping a line across the plane and maintaining the beach line and a priority queue of events. The provided code implements this algorithm with additional functionality for clipping and managing memory, ensuring the generated diagram is correctly bounded and efficiently handled.
















1. A brief description about your program. What does it do?
2. The usual README content telling us how to run your program.
3. An extra implementation section that describes some software considerations, where appropriate, about

- Algorithms, data structures, and complexities,
- Platform and system dependence or independence, global constants and configurability,
- Input/output syntax, pre- and post-processing,
- Data and code sources, the re-use and adaptation of existing code, any acknowledgment of external sources.
- Caveats, bugs, cautions, assumptions.
4. objectives.
## Objectives

1. Detect collisions between the bullet and the 2D sheet, and find the impact location (completed).

2. Construct Voronoi diagram generation function with a given number of seeds (completed).

3. Generate a list of random points on the 2D sheet, move them towards the impact point, and generate Voronoi diagram based on the points (completed).

4. Create new objects for each cell in the Voronoi diagram (completed).

5. Set depth Z in the direction of normals with given value to every seed and add the new vertice to vertice list (completed).

6. Create gap polygons between the front and back surface by setting 4 vertices and add to vertice list (completed).

7. Connect every 3 vertices to create mesh and assign the mesh to the new objects, add new objects and remove old objects from object list(completed).

8. Make new objects move following velocity and update every loop (completed).