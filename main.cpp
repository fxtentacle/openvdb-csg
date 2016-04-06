#include <iostream>
#include <vector>
#include "./openvdb/openvdb.h"
#include "./openvdb/tools/MeshToVolume.h"
#include "./openvdb/tools/VolumeToMesh.h"
#include "./openvdb/tools/Composite.h"

using namespace std;

typedef std::vector<int> MeshDataFace;

struct MeshDataAdapter {
  std::vector<openvdb::Vec3d> vertices;
  std::vector<MeshDataFace> faces;

  inline size_t polygonCount() const { return faces.size(); }; // Total number of polygons
  inline size_t pointCount() const { return vertices.size(); }; // Total number of points
  inline size_t vertexCount(size_t n) const { return faces[n].size(); }; // Vertex count for polygon n

  // Return position pos in local grid index space for polygon n and vertex v
  inline void getIndexSpacePoint(size_t n, size_t v, openvdb::Vec3d& pos) const {
    pos = vertices[faces[n][v]];
  }
};

int m2v(int argc, char* argv[])  {
  if(argc < 2+4) {
    printf("Please specify 4 parameters: input OBJ mesh path, voxel size, band width and output volume path\n");
    return 1;
  }

  const char* inputFile = argv[2];
  double voxelSize = atof(argv[3]);
  double bandWidth = atof(argv[4]);
  const char* outputFile = argv[5];

  FILE* f = fopen(inputFile,"rt");
  if(!f) {
    printf("Cannot open \"%s\"\n", inputFile);
    return 1;
  }

  MeshDataAdapter mesh;
  char line[4096];
  while(!feof(f)) {
    fgets(line,4096,f);
    if(line[0] == 'v' && line[1] == ' ') {
      // vertex
      double x=0,y=0,z=0,w=1;
      sscanf(line, "v %lf %lf %lf %lf", &x, &y, &z, &w);
      mesh.vertices.push_back(openvdb::Vec3d(x,y,z));
    } else if(line[0] == 'f' && line[1] == ' ') {
      // face
      MeshDataFace face;
      char* curToken = strchr(line, ' ');
      while(curToken && curToken[0]) {
        char* nextToken = strchr(curToken, ' ');
        if(nextToken) {
          nextToken[0] = 0;
          nextToken++;
        }
        int v=-1,vt=-1,vn=-1;
        sscanf(curToken, "%d/%d/%d", &v, &vt, &vn);
        if(v>0) face.push_back(v-1);
        curToken = nextToken;
      }
      mesh.faces.push_back(face);
    }
  }

  fclose(f);

  openvdb::math::Transform::Ptr transform = openvdb::math::Transform::createLinearTransform(voxelSize);
  openvdb::FloatGrid::Ptr volume = openvdb::tools::meshToVolume<openvdb::FloatGrid, MeshDataAdapter>(mesh,*transform,bandWidth,bandWidth,0,NULL);

  openvdb::io::File file(outputFile);
  openvdb::GridPtrVec grids;
  grids.push_back(volume);
  file.write(grids);
  file.close();

  return 0;
}

inline openvdb::FloatGrid::Ptr fetchVolume(const char* inputFile) {
  openvdb::io::File file(inputFile);
  file.open();

  openvdb::io::File::NameIterator nameIter = file.beginName();
  if( nameIter == file.endName() ) {
    printf("Error reading \"%s\" or file is empty.\n", inputFile);
    exit(1);
  }

  return openvdb::gridPtrCast<openvdb::FloatGrid>(file.readGrid(nameIter.gridName()));
}

int v2m(int argc, char* argv[])  {
  if(argc < 2+4) {
    printf("Please specify 4 parameters: input volume path, surface isovalue, adaptivity and output OBJ mesh path\n");
    return 1;
  }

  const char* inputFile = argv[2];
  double isovalue = atof(argv[3]);
  double adaptivity = atof(argv[4]);
  const char* outputFile = argv[5];

  openvdb::FloatGrid::Ptr volume = fetchVolume(inputFile);

  std::vector<openvdb::Vec3s> points;
  std::vector<openvdb::Vec3I> triangles;
  std::vector<openvdb::Vec4I> quads;
  openvdb::tools::volumeToMesh<openvdb::FloatGrid>(*volume, points, triangles, quads, isovalue, adaptivity);



  FILE* f = fopen(outputFile,"wt");
  if(!f) {
    printf("Cannot write to \"%s\"\n", outputFile);
    return 1;
  }

  for(int i=0;i<points.size();i++) fprintf(f, "v %lf %lf %lf\n", points[i].x(), points[i].y(), points[i].z());
  for(int i=0;i<triangles.size();i++) fprintf(f, "f %d// %d// %d//\n", triangles[i][0]+1, triangles[i][1]+1, triangles[i][2]+1);
  for(int i=0;i<quads.size();i++) fprintf(f, "f %d// %d// %d// %d//\n", quads[i][0]+1, quads[i][1]+1, quads[i][2]+1, quads[i][3]+1);

  fclose(f);

  return 0;
}

int vcsg(int argc, char* argv[])  {
  if(argc < 2+4) {
    printf("Please specify 4 parameters: operation (union/intersection/difference), input volume A path, input volume B path, output volume path\n");
    return 1;
  }

  const char* operation = argv[2];
  const char* inputFileA = argv[3];
  const char* inputFileB = argv[4];
  const char* outputFile = argv[5];

  openvdb::FloatGrid::Ptr volumeA = fetchVolume(inputFileA);
  openvdb::FloatGrid::Ptr volumeB = fetchVolume(inputFileB);

  if(!strcmp(operation, "union")) openvdb::tools::csgUnion(*volumeA, *volumeB);
  else if(!strcmp(operation, "intersection")) openvdb::tools::csgUnion(*volumeA, *volumeB);
  else if(!strcmp(operation, "difference")) openvdb::tools::csgUnion(*volumeA, *volumeB);

  openvdb::io::File file(outputFile);
  openvdb::GridPtrVec grids;
  grids.push_back(volumeA);
  file.write(grids);
  file.close();

  return 0;
}

int main(int argc, char* argv[])  {
  if(argc<2) {
    printf("Please specify a command: m2v v2m vcsg\n");
    return 1;
  }

  openvdb::initialize();

  const char* command = argv[1];
  if(!strcmp(command, "m2v")) return m2v(argc, argv);
  if(!strcmp(command, "v2m")) return v2m(argc, argv);
  if(!strcmp(command, "vcsg")) return vcsg(argc, argv);

  printf("Unknown command \"%s\"\n", command);
  return 1;
}