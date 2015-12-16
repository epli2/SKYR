#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include "Triangulation_v_ostream_2.hpp"
#include <CGAL/Cartesian.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/IO/Geomview_stream.h>
#include <CGAL/IO/Triangulation_geomview_ostream_2.h>
typedef CGAL::Cartesian<double>  K;
typedef CGAL::Projection_traits_xy_3<K> Gt3;
typedef Gt3::Point Point3;
typedef CGAL::Delaunay_triangulation_2<Gt3> Terrain;

// #define NDAT 10
#define NFILE 8
using namespace std;

typedef struct Point{
  float x, y, z;
}Point;
vector<Point> vertices[NFILE];
vector<Point>::iterator it;
int ndat;

void fileInput(string filename, int filenum);

int main(int argc, char *argv[]){
  ndat = atoi(argv[1]);
  string filename[] = { "./dat/53394610_dsm_1m.dat",
                        "./dat/53394611_dsm_1m.dat",
                        "./dat/53394620_dsm_1m.dat",
                        "./dat/53394621_dsm_1m.dat",
                        "./dat/53394630_dsm_1m.dat",
                        "./dat/53394631_dsm_1m.dat",
                        "./dat/53394640_dsm_1m.dat",
                        "./dat/53394641_dsm_1m.dat" };
  string filename_out = "output.wrl";
  ofstream ofs(filename_out);
  string tmpfilename = "tmp.dat";
  ofstream tmpfile(tmpfilename);
  Terrain T;
  Point3 p;
  fileInput(filename[0], 0);
  // thread t1(fileInput, filename[0], 0);
  // thread t2(fileInput, filename[1], 1);
  // thread t3(fileInput, filename[2], 2);
  // thread t4(fileInput, filename[3], 3);
  // t1.join();
  // t2.join();
  // t3.join();
  // t4.join();

  //ドロネー三角形分割
  auto startTime = chrono::system_clock::now();
  cout << "三角形分割実行中…" << endl;
  // Tercel::Delaunay3d::getDelaunayTriangles(vertices[0], &triangles);
  for(it = vertices[0].begin(); it != vertices[0].end(); it++){
    T.insert(Point3(it->x, it->y, it->z));
  }
  auto endTime = chrono::system_clock::now();
  auto timeSpan = endTime - startTime;
  cout << "三角形分割が完了しました。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;

  //Geomview出力
  // CGAL::Geomview_stream gv(CGAL::Bbox_3(-7600, -36050, 0, -6500, -36050, 40));
  // gv.set_line_width(4);
  // gv.set_bg_color(CGAL::Color(0, 200, 200));
  // // cout << "Drawing Terrain in wired mode.\n";
  // // gv.set_wired(true);
  // // gv << T;
  // // sleep(5);
  // gv.clear();
  // cout << "Drawing Terrain in non-wired mode.\n";
  // gv.set_wired(false);
  // gv << T;
  tmpfile << T;

  //VRMLファイル出力
  startTime = chrono::system_clock::now();
  cout << "VRMLファイルを作っています…" << endl;
  ofs << "#VRML V2.0 utf8" << endl;
  ofs << "Viewpoint{position -7541 -36046 23.4" << endl;
  ofs << "description \"Entry view\"}" << endl;
  ofs << "NavigationInfo{type [ \"EXAMINE\", \"ANY\" ] headlight TRUE}" << endl;
  ofs << "Shape{appearance Appearance{material Material{diffuseColor 0.0 1.0 0.0}}"
      << "geometry IndexedFaceSet{coord Coordinate{point["
      << endl;
  ifstream iFile2(tmpfilename);
  iFile2 >> p;
  unsigned int cnt = 0;
  while ( iFile2 >> p && cnt < T.number_of_vertices()) {
    ofs << p << "," <<endl;
    cnt++;
  }
  ofs << "]} coordIndex[" << endl;
  ofs << my_show_triangulation_faces(T);
  ofs << "]" << endl;
  ofs << "creaseAngle 0.0" << endl;
  ofs << "convex FALSE" << endl;
  ofs << "solid TRUE" << endl;
  ofs << "colorPerVertex TRUE" << endl;
  ofs << "ccw TRUE" << endl;
  ofs << "}}" << endl;
  endTime = chrono::system_clock::now();
  timeSpan = endTime - startTime;
  cout << "VRMLファイル出来た。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;

  cout << "Enter a key to finish" << endl;
  char ch;
  cin >> ch;
  return 0;
}

void fileInput(string filename, int filenum){
  int i = 0;
  int lost_n = 0;
  ifstream ifs(filename);
  string str;
  Point v;
  auto startTime = chrono::system_clock::now();
  // cout << "データ入力中…" << endl;
  while(getline(ifs, str) && i < ndat){
    sscanf(str.data(), "%f %f %f", &v.x, &v.y, &v.z);
    if(v.z <= -9999){
      v.z = 0;
      lost_n++;
    }
    vertices[filenum].push_back(v);
    i++;
  }
  auto endTime = chrono::system_clock::now();
  auto timeSpan = endTime - startTime;
  cout << filename << "のデータを入力しました。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;
  cout << "データ数 = " << i << endl;
  cout << "欠損点数 = " << lost_n << endl;
}
