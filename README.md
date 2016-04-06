# Getting Started

1. You need to have OpenVDB and boost and TBB3+ installed. 
2. Update CMakeLists.txt with your paths.
3. Compile with cmake

# Example use of the resulting binary

## Convert 3 test files to voxel grids
./ovdbcsg m2v test.obj 1 10 test-x1.vol
./ovdbcsg m2v testA.obj 1 10 test-A.vol
./ovdbcsg m2v testB.obj 1 10 test-B.vol

## Convert back to check that voxel grids have OK resolution
./ovdbcsg v2m test-x1.vol 0 0.5 test2-x1.obj
./ovdbcsg v2m test-A.vol 0 0.5 test2-A.obj
./ovdbcsg v2m test-B.vol 0 0.5 test2-B.obj

## CSG Union
./ovdbcsg vcsg union test-A.vol test-B.vol test-AB.vol
./ovdbcsg v2m test-AB.vol 0 0.5 test2-AB1.obj

## CSG Intersection
./ovdbcsg vcsg intersection test-A.vol test-x1.vol test-AB.vol
./ovdbcsg v2m test-AB.vol 0 0.0 test2-AB2.obj

## CSG Difference
./ovdbcsg vcsg difference test-B.vol test-x1.vol test-AB.vol
./ovdbcsg v2m test-AB.vol 0 0.0 test2-AB3.obj


