#include <iostream>
#include <vector>
#include "./openvdb/openvdb.h"

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
  if(argc < 5) {
    printf("Please specify 3 parameters: input OBJ mesh path, band width and output volume path\n");
    return 1;
  }

  const char* inputFile = argv[2];
  double surfaceOffset = atof(argv[3]);
  const char* outputFile = argv[4];

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
      sscanf(line, "v %f %f %f %f", &x, &y, &z, &w);
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

  openvdb::GridType::Ptr meshToVolume	(	const MeshDataAdapter & 	mesh,
                                 const math::Transform & 	transform,
                                 float 	exteriorBandWidth = 3.0f,
                                 float 	interiorBandWidth = 3.0f,
                                 int 	flags = 0,
                                 typename GridType::template ValueConverter< Int32 >::Type * 	polygonIndexGrid = NULL
  )

  return 0;
}

int v2m(int argc, char* argv[])  {
  return 0;
}

int vmod(int argc, char* argv[])  {
  return 0;
}

int main(int argc, char* argv[])  {
  if(argc<2) {
    printf("Please specify a command: m2v v2m vmod\n");
    return 1;
  }
  const char* command = argv[1];
  if(!strcmp(command, "m2v")) return m2v(argc, argv);
  if(!strcmp(command, "v2m")) return v2m(argc, argv);
  if(!strcmp(command, "vmod")) return vmod(argc, argv);

  printf("Unknown command \"%s\"\n", command);
  return 1;
}