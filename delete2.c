#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "input_array_new.h"

#define NDAT 1150
#define N 1100000

void delete_point(float** p,float** q,Range range,char** flag, float m);
// void array_to_Points(float** points, Range range, char** flag,
//                      Point* p, int *n);

int main(int argc, char** argv)
{
  float m = 0;
  char* fname = NULL;
  if(argc > 1) fname = argv[1];
  if(argc > 2) m = atof(argv[2]);
  int i, n;
  char ** flag = (char**)malloc2d(NDAT, NDAT, sizeof(char));
  float** points = (float**)malloc2d(NDAT, NDAT, sizeof(float));
  float** pointq = (float**)malloc2d(NDAT, NDAT, sizeof(float));
  Range range;
  Point *p = malloc(sizeof(Point) * N);


  if (input_to_array(points, &range, flag, fname) == 0) {
    cover_loss(points, range, flag);
  }
  delete_point(points,pointq,range,flag,m);
  array_to_Points(points,range,flag,p,&n);
  for (i=0; i<n; i++) printf ("%f %f %f\n", p[i].x, p[i].y, p[i].z);

  return 0;
}

void delete_point(float** p,float** q,Range range,char** flag, float m)
{
  int i,j,k,l;
  float d[3][3];
  int cnt=0,cntp=0;
  // int laplacian55[5][5] = {{-1, -3, -4, -3, -1},
  //                          {-3, 0, 6, 0, -3},
  //                          {-4, 6, 20, 6, -4},
  //                          {-3, 0, 6, 0, -3},
  //                          {-1, -3, -4, -3, -1}};

  for(i = 0; i <= range.r; i++){
    for(j = 0; j <= range.b; j++){
      if(flag[i][j] == 1) cntp++;
    }
  }

  for(i = 0; i <= range.r; i++){
    for(j = 0; j <= range.b; j++){
      int usep = 0;
      // q[i][j] = 0;
      // for(k = -2; k <= 2; k++){
      //   for(l = -2; l <= 2; l++){
      //     if(!(i+k < 0 || j+l < 0 || i+k > range.r || j+l > range.b)){
      //       q[i][j] += p[i+k][j+l] * laplacian55[k+2][l+2];
      //       usep++;
      //     }
      //   }
      // }
      // q[i][j] *= usep * 0.04;
      for(k = -1; k <= 1; k++){
        for(l = -1; l <= 1; l++){
          if((i==0 && k==-1) || (j==0 && l==-1) || (i==range.r && k==1) || (j==range.b && l==1)){
            d[k+1][l+1] = 0;
          }else{
            // d[k+1][l+1] = (p[i][j] - p[i+k][j+l])*(p[i][j] - p[i+k][j+l]);
            d[k+1][l+1] = p[i+k][j+l];
            usep++;
          }
        }
      }
      // q[i][j] = sqrt((d[0][0] +d[0][1] +d[0][2] +d[1][0] +d[1][1] +d[1][2] +d[2][0] +d[2][1] +d[2][2]) / usep);
      q[i][j] = d[0][0] + d[0][1] + d[0][2] + d[1][0] - usep * d[1][1] + d[1][2] + d[2][0] + d[2][1] + d[2][2];
      if(fabs(q[i][j]) < m) flag[i][j] = 0;
    }
  }

  for(i = 0; i <= range.r; i++){
    for(j = 0; j <= range.b; j++){
      float mindiff = INFINITY;
      float maxdiff = -INFINITY;
      for(k = -1; k <= 1; k++){
        for(l = -1; l <= 1; l++){
          if(!((k==0 && l==0) || (i==0 && k==-1) || (j==0 && l==-1) || (i==range.r && k==1) || (j==range.b && l==1))){
            float diff = p[i][j] - p[i+k][j+l];
            if(maxdiff < diff) maxdiff = diff;
            if(mindiff > diff) mindiff = diff;
          }
        }
      }
      if(maxdiff < 0) flag[i][j] = 0;
      if(mindiff > 0) flag[i][j] = 0;
    }
  }

  for(i = 0; i <= range.r; i++){
    for(j = 0; j <= range.b; j++){
      if(i % 2 == 1 || j % 2 == 1) flag[i][j] = 0; //２分の１
      // if(i % 2 == 1 || j % 2 == 1 || (i+j) % 4 == 2) flag[i][j] = 0; //４分の１
    }
  }
  //削減
  // for(i = 0; i <= range.r; i++){
  //   for(j = 0; j <= range.b; j++){
      // int usep = 0;
      // for(k=-1;k<=1;k++){
      //   for(l=-1;l<=1;l++){
      //     if(! ((i==0 && k==-1) || (j==0 && l==-1) || (i==range.r && k==1) || (j==range.b && l==1))){
      //       usep++;
      //       // if(q[i+k][j+l] < m) cnt++;
      //       if(-m < q[i+k][j+l] && q[i+k][j+l] < m) cnt++;
      //     }
      //   }
      // }
      // if(cnt > usep * 0.7) flag[i][j] = 0;
      // cnt = 0;
  //   }
  // }
  /*
  //使う点数
  for(i=0;i<NDAT;i++){
    for(j=0;j<NDAT;j++){
      if(flag[i][j]==1) cntq++;
    }
  }
  printf("cntq=%d\n",cntq);

  //削減率
  printf("削減率=%f\n",100-(double)cntq/cntp*100);
  */
}

// void array_to_Points(float** points, Range range, char** flag,
//                      Point* p, int *n)
// {
//   int i, j, cnt=0;
//
//   for (i = 0; i < range.r; i++) {
//     for (j = 0; j < range.b; j++) {
//       if (flag[i][j] == 1 && cnt < N) {
// 	p[cnt].x = i + range.l;
// 	p[cnt].y = j + range.t;
// 	p[cnt].z = points[i][j];
// 	cnt++;
//       }
//     }
//   }
//   *n = cnt;
// }
