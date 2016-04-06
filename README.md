# Getting Started

1. You need to have OpenVDB and boost and TBB3+ installed. 
2. Update CMakeLists.txt with your paths.
3. Compile with cmake

# What can the tool do?

```
./ovdbcsg m2v input.obj voxel-size band-width output.vol
```
Will convert an OBJ mesh to a voxel volume with the given voxel grid size. Band width is the number of neighboring voxels that get filled with a signed distance to the surface. It's basically how strongly the surface can be modified before the resulting loss of precision becomes noticeable. I usually just use 10.

```
./ovdbcsg v2m input.vol isovalue adaptivity output.obj
```
Will convert a voxel volume to a OBJ mesh. Normally, you set isovalue to 0 but you can set it to a positive or negative float value, and then the surface will be moved inwards (negative) or outwards (positive) by the distance you specify. Adaptivity is how much the mesh topology will be optimized for your object. Using a value of 0, your object will be covered with a grid of polygons that is the same size as the grid of the input voxel volume data. Using a value of 0.5, flat areas in the OBJ model will be optimized to use fewer faces.

```
./ovdbcsg vcsg union inputA.vol inputB.vol output.vol
./ovdbcsg vcsg intersection inputA.vol inputB.vol output.vol
./ovdbcsg vcsg difference inputA.vol inputB.vol output.vol
```
Will calculate the CSG union / intersection / difference of two voxel volume grids.


# Example use of the resulting binary

## Convert 3 test files to voxel grids
```
./ovdbcsg m2v test.obj 1 10 test-x1.vol
./ovdbcsg m2v testA.obj 1 10 test-A.vol
./ovdbcsg m2v testB.obj 1 10 test-B.vol
```

## Convert back to check that voxel grids have OK resolution
```
./ovdbcsg v2m test-x1.vol 0 0.5 test2-x1.obj
./ovdbcsg v2m test-A.vol 0 0.5 test2-A.obj
./ovdbcsg v2m test-B.vol 0 0.5 test2-B.obj
```

## CSG Union
```
./ovdbcsg vcsg union test-A.vol test-B.vol test-AB.vol
./ovdbcsg v2m test-AB.vol 0 0.5 test2-AB1.obj
```

## CSG Intersection
```
./ovdbcsg vcsg intersection test-A.vol test-x1.vol test-AB.vol
./ovdbcsg v2m test-AB.vol 0 0.0 test2-AB2.obj
```

## CSG Difference
```
./ovdbcsg vcsg difference test-B.vol test-x1.vol test-AB.vol
./ovdbcsg v2m test-AB.vol 0 0.0 test2-AB3.obj
```

## Move surface 0.1mm inwards
This is useful for example when you want to 3d print a socket and plug combination and you forgot to leave a small air gap in between.
```
./ovdbcsg m2v test.obj 1 10 test-x1.vol
./ovdbcsg v2m test-x1.vol -0.1 0.0 test-inwards-01mm.obj
```

## Move surface 0.1mm outwards
```
./ovdbcsg m2v test.obj 1 10 test-x1.vol
./ovdbcsg v2m test-x1.vol 0.1 0.0 test-outwards-01mm.obj
```


