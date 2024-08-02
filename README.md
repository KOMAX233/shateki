# Shateki: Japanese gun shooting game with Voronoi diagram and mesh generation in fracture simulation

# Specification
- opengl is used
- voronoi diagram using sweep line is used
- object2 assume impact point is center of the front surface of the sheet
- seeing 36 vertices because each face is split into triangle which is 3 vertices, 6 faces, 12 triangles, 36 vertices.
- from camera direction, z is positive, front is z=0, back is z=-0.2
(-1,1) (1,1)
(-1,-1) (1,-1)
task3
- assume number of sites to randomly generate is 5
- move random points in the -0.1, 0.1 range to the impact point in x and y
- assume point size is 3.0
task 5
- assume the depth of square is 0.1(given value)
- 

## Objectives

1. Detect collisions between the bullet and the 2D sheet, and find the impact location.

2. Construct Voronoi diagram generation function with a given number of seeds (completed).

3. Generate a list of random points on the 2D sheet, move them towards the impact point, and generate Voronoi diagram based on the points (completed).

4. Create new objects for each cell in the Voronoi diagram (completed).

5. Set depth Z in the direction of normals with given value to every seed and add the new vertice to vertice list.

6. Create gap polygons between the front and back surface by setting 4 vertices and add to vertice list.

7. Connect every 3 vertices to create mesh and assign the mesh to the new objects, add new objects and remove old objects from object list.

8. Make new objects move following velocity and update every loop.

