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

## Objectives

1. Detect collisions between the bullet and the 2D sheet, and find the impact location.

2. Construct Voronoi diagram generation function with a given number of seeds (completed).

3. Generate a list of random points on the 2D sheet, move them towards the impact point, and generate Voronoi diagram based on the points (completed).

4. Create new objects for each cell in the Voronoi diagram (completed).

5. Set depth Z in the direction of normals with given value to every seed and add the new vertice to vertice list (completed).

6. Create gap polygons between the front and back surface by setting 4 vertices and add to vertice list (completed).

7. Connect every 3 vertices to create mesh and assign the mesh to the new objects, add new objects and remove old objects from object list(completed).

8. Make new objects move following velocity and update every loop.

