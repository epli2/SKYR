#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#define NDAT 1500
#define NFILE 8
#include "./tercel/Delaunay3d.h"
using namespace std;

set<Tercel::Vector> vertices[NFILE];
set<Tercel::Triangle> triangles;

void fileInput(string filename, int filenum);

int main(int argc, char *argv[]){
  int n = 0;
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
  //fileInput(filename[0], 0);
  thread t1(fileInput, filename[0], 0);
  thread t2(fileInput, filename[1], 1);
  thread t3(fileInput, filename[2], 2);
  thread t4(fileInput, filename[3], 3);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  //ドロネー三角形分割
  auto startTime = chrono::system_clock::now();
  cout << "三角形分割実行中…" << endl;
  Tercel::Delaunay3d::getDelaunayTriangles(vertices[0], &triangles);
  auto endTime = chrono::system_clock::now();
  auto timeSpan = endTime - startTime;
  cout << "三角形分割が完了しました。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;

  //VRMLファイル出力
  startTime = chrono::system_clock::now();
  cout << "VRMLファイルを作っています…" << endl;
  ofs << "#VRML V2.0 utf8" << endl;
  ofs << "Viewpoint{position 0.0 0.0 0.0" << endl;
  ofs << "description \"Entry view\"}" << endl;
  ofs << "NavigationInfo{type [ \"EXAMINE\", \"ANY\" ] headlight TRUE}" << endl;
  ofs << "Shape{appearance Appearance{material Material{diffuseColor 0.0 1.0 0.0}}"
      << "geometry IndexedFaceSet{coord Coordinate{point["
      << endl;
  for(set<Tercel::Triangle>::iterator it = triangles.begin();
    it != triangles.end(); ++it){
    Tercel::Triangle t = *it;  // 三角形取得
    for(int i = 0; i < 3; ++i) {
      float x1 = (float)t.p[i]->x;
      float y1 = (float)t.p[i]->y;
      float z1 = (float)t.p[i]->z;

      // float x2 = (float)t.p[(i+1)%3]->x;
      // float y2 = (float)t.p[(i+1)%3]->y;
      // float z2 = (float)t.p[(i+1)%3]->z;
      ofs << x1 << " " << y1 << " " << z1 << "," << endl;
      // ofs << x2 << " " << y2 << " " << z2 << "," << endl;
    }
    n++;
  }
  ofs << "]} coordIndex[" << endl;
  for(int i = 0; i < n; i++){
    for(int j = 0; j < 3; j++){
      ofs << "0, 1, 2, -1,\n" << flush;
    }
  }
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
  return 0;
}

void fileInput(string filename, int filenum){
  int i = 0;
  int lost_n = 0;
  ifstream ifs(filename);
  string str;
  Tercel::Vector v;
  auto startTime = chrono::system_clock::now();
  // cout << "データ入力中…" << endl;
  while(getline(ifs, str) && i < NDAT){
    sscanf(str.data(), "%f %f %f", &v.x, &v.y, &v.z);
    // v.x += 10000;
    // v.y += 50000;
    if(v.z <= -9999){
      v.z = 0;
      lost_n++;
    }
    // cout << v.x << " " << v.y << " " << v.z << endl;
    vertices[filenum].insert(v);
    i++;
  }
  auto endTime = chrono::system_clock::now();
  auto timeSpan = endTime - startTime;
  cout << filename << "のデータを入力しました。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;
  cout << "データ数 = " << i << endl;
  cout << "欠損点数 = " << lost_n << endl;
}
