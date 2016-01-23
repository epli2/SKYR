#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <limits.h>
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

#define LENGTH4BUILD 50
#define LENGTH4ROAD 30 //25か30。0で道路判定しない
#define THRESHOLD 4
#define PERCENT 0.1
using namespace std;

typedef struct Point{
  float x, y, z;
}Point;


int ndat = INT_MAX;
int maxz = INT_MIN;
float minz = INFINITY;
int maxx = INT_MIN;
int maxy = INT_MIN;
int minx = INT_MAX;
int miny = INT_MAX;
int width;
int height;

void fileInput(char* filename, vector<Point> &vertices);
vector<vector<char>> detectRoad(vector<Point> &verticesRaw);
void reduceBlack(vector<vector<char>> &resultarray, vector<vector<char>> &resultarray2);
template < class GT, class TDS > string printvertexVRML(const CGAL::Triangulation_2<GT,TDS> &T);
template < class GT, class TDS > string printfacesVRML(const CGAL::Triangulation_2<GT,TDS> &T, vector<Point> &vertices);
template < class GT, class TDS > string printcolorVRML(const CGAL::Triangulation_2<GT,TDS> &T, vector<vector<char>> &roadFrag);
void addPerimeterPoint(vector<Point> &vertices, vector<Point> &verticesRaw);

int main(int argc, char *argv[]){
  char* rawdat = NULL;
  char* deleteddat = NULL;
  if(argc > 1) rawdat = argv[1];
  if(argc > 2) deleteddat = argv[2];
  if(argc > 3) ndat = atoi(argv[3]);
  // string filename_out = "output.wrl";
  // ofstream ofs(filename_out);
  // string tmpfilename = "tmp.dat";
  // ofstream tmpfile(tmpfilename);
  vector<Point> verticesRaw;
  vector<Point> verticesDeleted;
  Terrain T;
  Point3 p;
  fileInput(rawdat, verticesRaw);
  fileInput(deleteddat, verticesDeleted);
  addPerimeterPoint(verticesDeleted, verticesRaw);

  // thread t1(fileInput, filename[0], 0);
  // thread t2(fileInput, filename[1], 1);
  // thread t3(fileInput, filename[2], 2);
  // thread t4(fileInput, filename[3], 3);
  // t1.join();
  // t2.join();
  // t3.join();
  // t4.join();

  cerr << "minx:" << minx << endl;
  cerr << "maxx:" << maxx << endl;
  cerr << "miny:" << miny << endl;
  cerr << "maxy:" << maxy << endl;
  //道判定
  vector<vector<char>> roadFrag = detectRoad(verticesRaw);

  //ドロネー三角形分割
  auto startTime = chrono::system_clock::now();
  cerr << "三角形分割実行中…" << endl;
  for(vector<Point>::iterator it = verticesDeleted.begin(); it != verticesDeleted.end(); it++){
    T.insert(Point3(it->x, it->y, it->z));
    // cerr << (it - vertices[0].begin()) << "/" << vertices[0].size() << "\r" << flush;
  }
  auto endTime = chrono::system_clock::now();
  auto timeSpan = endTime - startTime;
  cerr << "三角形分割が完了しました。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;

  //Geomview出力
  #if 0
    CGAL::Geomview_stream gv(CGAL::Bbox_3(-7600, -36050, 0, -6500, -36050, 40));
    gv.set_line_width(4);
    gv.set_bg_color(CGAL::Color(0, 200, 200));
    // cerr << "Drawing Terrain in wired mode.\n";
    // gv.set_wired(true);
    // gv << T;
    // sleep(5);
    gv.clear();
    cerr << "Drawing Terrain in non-wired mode.\n";
    gv.set_wired(false);
    gv << T;
  #endif
  // tmpfile << T;


  //VRMLファイル出力
  startTime = chrono::system_clock::now();
  cerr << "VRMLファイルを作っています…" << endl;
  cout << "#VRML V2.0 utf8" << endl;
  cout << "DirectionalLight { direction 1 0 -1 color 1 1 1 intensity 0.6 }" << endl;
  cout << "Shape{" << endl;
  cout << "appearance Appearance{ material Material{} }" << endl;
  //座標
  cout << "geometry IndexedFaceSet{coord Coordinate{point[" << endl;
  cout << printvertexVRML(T);
  cout << "]} color Color { color [.4 .4 .4, .8 .6 .4, .2 .3 .2]}" << endl;
  //つなぐ点
  cout << "coordIndex[" << endl;
  cout << printfacesVRML(T, verticesRaw);
  cout << "]" << endl;
  //色指定
  cout << "colorIndex [" << endl;
  cout << printcolorVRML(T, roadFrag);
  cout << "]" << endl;
  cout << "creaseAngle 0.0" << endl;
  cout << "convex FALSE" << endl;
  cout << "solid TRUE" << endl;
  cout << "colorPerVertex TRUE" << endl;
  cout << "ccw TRUE" << endl;
  cout << "}}" << endl;
  endTime = chrono::system_clock::now();
  timeSpan = endTime - startTime;
  cerr << "VRMLファイル出来た。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;

  cerr << "Enter a key to finish" << endl;
  char ch;
  cin >> ch;
  return 0;
}

void fileInput(char* filename, vector<Point> &vertices){
  int i = 0;
  int lost_n = 0;
  ifstream ifs(filename);
  string str;
  Point v;
  auto startTime = chrono::system_clock::now();
  // cerr << "データ入力中…" << endl;
  while(getline(ifs, str) && i < ndat){
    sscanf(str.data(), "%f %f %f", &v.x, &v.y, &v.z);
    if(v.z <= -9999){
      v.z = -1;
      lost_n++;
    }
    if((int)v.x > maxx) maxx = (int)v.x;
    if((int)v.x < minx) minx = (int)v.x;
    if((int)v.y > maxy) maxy = (int)v.y;
    if((int)v.y < miny) miny = (int)v.y;
    if((int)v.z > maxz) maxz = (int)v.z;
    if(v.z < minz) minz = v.z;
    vertices.push_back(v);
    i++;
  }
  width = maxx - minx + 1;
  height = maxy - miny + 1;
  // cerr << maxx << " " << minx << " " << maxy << " " << miny << " " << maxz << endl;
  auto endTime = chrono::system_clock::now();
  auto timeSpan = endTime - startTime;
  cerr << filename << "のデータを入力しました。 (" << chrono::duration_cast<chrono::milliseconds>(timeSpan).count() << "ms)" << endl;
  cerr << "データ数 = " << i << endl;
  cerr << "欠損点数 = " << lost_n << endl;
}


void addPerimeterPoint(vector<Point> &vertices, vector<Point> &verticesRaw){
  auto itrNewEnd = remove_if(vertices.begin(), vertices.end(), [](Point p)->bool{ return (int)p.x == minx || (int)p.x == maxx || (int)p.y == maxy || (int)p.y == miny; });
  vertices.erase(itrNewEnd,vertices.end());
  vector<vector<float>> pointsarray (height, vector<float>(width));
  vector<vector<float>> pointsarray2 (height, vector<float>(width));

  for(int y=height - 1; y >= 0; y--){
    for(int x=0; x < width; x++){
      pointsarray[y][x] = -9999;
    }
  }

  for(vector<Point>::iterator it = verticesRaw.begin(); it != verticesRaw.end(); it++){
    pointsarray[(int)it->y - miny][(int)it->x - minx] = it->z;
  }

  for(int y=height - 1; y >= 0; y--){
    int upper = (y+1 < height - 1) ? y+1 : height - 1;
    int lower = (y-1 > 0) ? y-1 : 0;
    for(int x=0; x < width; x++){
      int left = (x-1 > 0) ? x-1 : 0;
      int right = (x+1 < width-1) ? x+1 : width - 1;
      float neighbor[8];
      int num = 0;
      for(int yy=upper; yy >= lower; yy--){
        for(int xx=left; xx <= right; xx++){
          if(pointsarray[yy][xx] > -9999){
            neighbor[num] = pointsarray[yy][xx];
            num++;
          }
        }
      }
      pointsarray2[y][x] = (num % 2 == 0) ? (neighbor[num/2-1] + neighbor[num/2])/2 : neighbor[num/2];
    }
  }
  for(int i = 0; i < width; i++){
    Point pmin = {(float)(minx + i), (float)miny, pointsarray2[0][i]};
    Point pmax = {(float)(minx + i), (float)maxy, pointsarray2[height - 1][i]};
    vertices.push_back(pmin);
    vertices.push_back(pmax);
  }
  for(int i = 1; i < height - 1; i++){
    Point pmin = {(float)minx, (float)(miny + i), pointsarray2[i][0]};
    Point pmax = {(float)maxx, (float)(miny + i), pointsarray2[i][width - 1]};
    vertices.push_back(pmin);
    vertices.push_back(pmax);
  }
}


void reduceBlack(vector<vector<char>> &resultarray, vector<vector<char>> &resultarray2){
  for(int y=height - 1; y >= 0; y--){
    int upper = (y+LENGTH4ROAD < height - 1) ? y+LENGTH4ROAD : height - 1;
    int lower = (y-LENGTH4ROAD > 0) ? y-LENGTH4ROAD : 0;
    for(int x=0; x < width; x++){
      int left = (x-LENGTH4ROAD > 0) ? x-LENGTH4ROAD : 0;
      int right = (x+LENGTH4ROAD < width - 1) ? x+LENGTH4ROAD : width - 1;
      int num = 0;
      int total = 0;
      resultarray2[y][x] = resultarray[y][x];
      if (resultarray[y][x] == 0) {
        for(int yy=upper; yy >= lower; yy--){
          for(int xx=left; xx <= right; xx++){
            if((x-xx)*(x-xx)+(y-yy)*(y-yy) <= LENGTH4ROAD*LENGTH4ROAD){
              total++;
              if(resultarray[yy][xx] == 0) num++;
            }
          }
        }
        if(num < total * 0.4) resultarray2[y][x] = 2;
      }
    }
  }
}

vector<vector<char>> detectRoad(vector<Point> &verticesRaw){
  vector<vector<float>> pointsarray (height, vector<float>(width));
  vector<vector<float>> pointsarray2 (height, vector<float>(width));
  vector<vector<char>> resultarray (height, vector<char>(width));
  vector<vector<char>> resultarray2 (height, vector<char>(width));
  for(int y=height - 1; y >= 0; y--){
    for(int x=0; x < width; x++){
      pointsarray[y][x] = -9999;
    }
  }

  for(vector<Point>::iterator it = verticesRaw.begin(); it != verticesRaw.end(); it++){
    pointsarray[(int)it->y - miny][(int)it->x - minx] = it->z;
  }

  for(int y=height - 1; y >= 0; y--){
    int upper = (y+1 < height - 1) ? y+1 : height - 1;
    int lower = (y-1 > 0) ? y-1 : 0;
    for(int x=0; x < width; x++){
      int left = (x-1 > 0) ? x-1 : 0;
      int right = (x+1 < width-1) ? x+1 : width - 1;
      float neighbor[8];
      int num = 0;
      for(int yy=upper; yy >= lower; yy--){
        for(int xx=left; xx <= right; xx++){
          if(pointsarray[yy][xx] > -9999){
            neighbor[num] = pointsarray[yy][xx];
            num++;
          }
        }
      }
      pointsarray2[y][x] = (num % 2 == 0) ? (neighbor[num/2-1] + neighbor[num/2])/2 : neighbor[num/2];
    }
  }

  for(int y=height - 1; y >= 0; y--){
    int upper = (y+LENGTH4BUILD < height - 1) ? y+LENGTH4BUILD : height - 1;
    int lower = (y-LENGTH4BUILD > 0) ? y-LENGTH4BUILD : 0;
    for(int x=0; x < width; x++){
      int left = (x-LENGTH4BUILD > 0) ? x-LENGTH4BUILD : 0;
      int right = (x+LENGTH4BUILD < width - 1) ? x+LENGTH4BUILD : width - 1;
      resultarray[y][x] = 0;
      int num = 0;
      int total = 0;
      for(int yy=upper; yy >= lower; yy--){
        for(int xx=left; xx <= right; xx++){
          if((x-xx)*(x-xx)+(y-yy)*(y-yy) <= LENGTH4BUILD*LENGTH4BUILD){
            total++;
            if(pointsarray2[y][x] > pointsarray2[yy][xx] + THRESHOLD) num++;
          }
        }
      }
      if(num > total*PERCENT) resultarray[y][x] = 1;
    }
  }

  for(int i=0; i < 20; i++){
    reduceBlack(resultarray, resultarray2);
    reduceBlack(resultarray2, resultarray);
    // ofstream pgm("out.pgm");
    // pgm << "P2" << endl;
    // pgm << "#hello" << endl;
    // pgm << width << " " << height << endl;
    // pgm << "2" << endl;
    // for(int y=height - 1; y >= 0; y--){
    //   for(int x=0; x < width; x++){
    //     pgm << (int)resultarray[y][x];
    //     if(x != width-1) pgm << " ";
    //     else pgm << endl;
    //   }
    // }
    cerr << i << " " << flush;
  }
  cerr << endl;
  return resultarray;
}


template < class GT, class TDS >
string printvertexVRML(const CGAL::Triangulation_2<GT,TDS> &T){
  ostringstream os;
  for( typename CGAL::Triangulation_2<GT, TDS>::Vertex_iterator
    vit = T.vertices_begin(); vit != T.vertices_end(); ++vit) {
      Point3 p = vit->point();
      os << p.x() << ", " << p.y() << ", " << p.z() << endl;
  }
  //3Dプリンタ用
  // for(int i = 0; i < width; i++){
  //   os << minx + i << ", " << maxy << ", " << minz - 10 << endl;
  //   os << minx + i << ", " << miny << ", " << minz - 10 << endl;
  // }
  // for(int i = 1; i < height - 1; i++){
  //   os << maxx << ", " << miny + i << ", " << minz - 10 << endl;
  //   os << minx << ", " << miny + i << ", " << minz - 10 << endl;
  // }
  os << minx << ", " << maxy << ", " << minz - 10 << endl;
  os << minx << ", " << miny << ", " << minz - 10 << endl;
  os << maxx << ", " << maxy << ", " << minz - 10 << endl;
  os << maxx << ", " << miny << ", " << minz - 10 << endl;
  return os.str();
}

template < class GT, class TDS >
string printfacesVRML(const CGAL::Triangulation_2<GT,TDS> &T, vector<Point> &vertices){
  ostringstream os;

  // Finite vertices coordinates.
  map<typename CGAL::Triangulation_2<GT, TDS>::Vertex_handle, int> V;
  vector<vector<int>> pointsarray (height, vector<int>(width));
  int inum = 0;
  for( typename CGAL::Triangulation_2<GT, TDS>::Vertex_iterator
    vit = T.vertices_begin(); vit != T.vertices_end(); ++vit) {
      Point3 p = vit->point();
      V[vit] = inum;
      pointsarray[(int)p.y() - miny][(int)p.x() - minx] = inum;
      inum++;
  }


  // Finite faces indices.
  for( typename CGAL::Triangulation_2<GT, TDS>::Face_iterator
    fit = T.faces_begin(); fit != T.faces_end(); ++fit) {
      for (int i=0; i<3; i++)
        os << V[fit->vertex(i)] << "," << flush;
      os << "-1,\n" << flush;
  }

  //x=minxの側面
  os << inum << "," << inum + 1 << ",";
  for(int y = 0; y < height; y++){
    os << pointsarray[y][0] << ",";
  }
  os << "-1,\n" << flush;
  //x=maxxの側面
  os << inum + 3 << "," << inum + 2 << ",";
  for(int y = height -1; y >= 0; y--){
    os << pointsarray[y][maxx - minx] << ",";
  }
  os << "-1,\n" << flush;
  //y=minyの側面
  os << inum + 1 << "," << inum + 3 << ",";
  for(int x = width - 1; x >= 0; x--){
    os << pointsarray[0][x] << ",";
  }
  os << "-1,\n" << flush;
  //y=maxyの側面
  os << inum + 2 << "," << inum << ",";
  for(int x = 0; x < width; x++){
    os << pointsarray[maxy - miny][x] << ",";
  }
  os << "-1,\n" << flush;
  //底
  os << inum + 2 << "," << inum + 3 << "," << inum + 1 << "," << inum << "," << "-1,\n" << flush;
  return os.str();
}

template < class GT, class TDS >
string printcolorVRML(const CGAL::Triangulation_2<GT,TDS> &T, vector<vector<char>> &roadFrag){
  ostringstream os;
  char nowfrag = 0;
  for( typename CGAL::Triangulation_2<GT, TDS>::Face_iterator
    fit = T.faces_begin(); fit != T.faces_end(); ++fit){
    for (int i=0; i<3; i++){
      Point3 p = fit->vertex(i)->point();
      nowfrag = roadFrag[(int)p.y() - miny][(int)p.x() - minx];
      os << (int)nowfrag << "," << flush;
    }
    os << "0," << flush;
  }
  //x=minxの側面
  os << 1 << "," << 1 << ",";
  for(int y = 0; y < height; y++){
    os << (int)roadFrag[y][0] << ",";
  }
  os << "0," << flush;
  //x=maxxの側面
  os << 1 << "," << 1 << ",";
  for(int y = height -1; y >= 0; y--){
    os << (int)roadFrag[y][maxx - minx] << ",";
  }
  os << "0," << flush;
  //y=minyの側面
  os << 1 << "," << 1 << ",";
  for(int x = width - 1; x >= 0; x--){
    os << (int)roadFrag[0][x] << ",";
  }
  os << "0," << flush;
  //y=maxyの側面
  os << 1 << "," << 1 << ",";
  for(int x = 0; x < width; x++){
    os << (int)roadFrag[maxy - miny][x] << ",";
  }
  os << "0," << flush;
  //底
  os << 1 << "," << 1 << "," << 1 << "," << 1 << "," << "0," << flush;
  return os.str();
}
