# Shateki: Japanese gun shooting game with Voronoi diagram and mesh generation in fracture simulation

## Objectives

1. Detect collisions between the bullet and the 2D sheet, and find the impact location.

2. Construct Voronoi diagram generation function with a given number of seeds.

3. Generate a list of random points on the 2D sheet, move them towards the impact point, and generate Voronoi diagram based on the points.

4. Create new objects for each cell in the Voronoi diagram.

5. Set depth Z in the direction of normals of with given value to every seed and add the new vertice to vertice list.

6. Create gap polygons between the front and back surface by setting 4 vertices and add to vertice list.

7. Connect every 3 vertices to create mesh and assign the mesh to the new objects, add new objects and remove old objects from object list.

8. Make new objects move following velocity and update every loop.

