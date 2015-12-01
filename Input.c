#include <stdio.h>
#define N 100
#define NDAT 100000 //1M -> セグメンテーション違反です (コアダンプ)

typedef struct point{
  float x;
  float y;
  float z;
}Point;

int cover_loss(Point p[],int n);
void makevrml(Point *p, int n);

int main(void){
  int i = 0,n;
  FILE *fp;
  char *fname = "001.dat";
  Point p[NDAT] = {};
  fp = fopen(fname,"r");
  while (((fscanf(fp, "%f %f %f", &p[i].x, &p[i].y, &p[i].z) != EOF) && i < NDAT)) i++;
  n = i;
  for (i=0; i<n; i++){
    printf("%f ",p[i].x);
    printf("%f ",p[i].y);
    printf("%f ",p[i].z);
    putchar('\n');
  }
  putchar('\n');
  n = cover_loss(p,n);
  for (i=0; i<n; i++){
    printf("%f ",p[i].x);
    printf("%f ",p[i].y);
    printf("%f ",p[i].z);
    putchar('\n');
  }
  makevrml(p, n);
  return 0;
}

int cover_loss(Point p[],int n){
  int i,j,c=0;
  char flag[NDAT] = {};
  for (i=0; i<n; i++){
    if (p[i].z <= -9999) flag[i] = 1;
    else c++;
  }
  j=0;
  for (i=0; i<n; i++){
    if (flag[i] == 0){
      p[j] = p[i];
      j++;
    }
  }
  return c;
}

void makevrml(Point p[], int n){
  int i;
  FILE *fp;
  char *outfile = "output.wrl";
  fp = fopen(outfile, "w");

  fprintf(fp, "#VRML V2.0 utf8\n");
  fprintf(fp, "Shape { geometry PointSet{");
  fprintf(fp, "color Color { color [");
  for(i=0; i<n; i++){
    fprintf(fp, "1.0 0.0 0.0");
    if(i != n-1)  fprintf(fp, ", ");
  }
  fprintf(fp, "]} coord Coordinate{point[");
  for(i=0; i<n; i++){
    fprintf(fp, "%.1f ",p[i].x + 10000);
    fprintf(fp, "%.1f ",p[i].y + 50000);
    fprintf(fp, "%.1f",p[i].z);
    if(i != n-1)  fprintf(fp, ", ");
  }
  fprintf(fp, "]}}}");
  fclose(fp);
}
