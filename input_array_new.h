#include <limits.h>
#include <float.h>
#define NDAT 1150
#define N 1100000

typedef struct _range{
    int l;    //x最小
    int t;    //y最大
    int r;    //x最大、０以上に直した値 幅
    int b;    //y最大 幅
}Range;


typedef struct point{
    float x;
    float y;
    float z;
}Point;

void getRange(Range *range, char* fname);
void cover_loss(float** points, Range range, char** flag);
//void print_points(float points[][NDAT], Range range, char flag[][NDAT]);

void array_to_Points(float** points, Range range, char** flag, Point p[], int *n);


void** malloc2d(int m, int n, int size){
    void** p = malloc(sizeof(void*) * m);
    void* q = malloc(size * m * n);

    int i;
    for (i = 0; i < m; i++)
        p[i] = q + i * n * size;

    memset(q, 0, size * m * n);

    return p;
}

int input_to_array(float** p, Range *range, char** flag, char *fname){

  int i, j;
  //char flag[NDAT + 150][NDAT];
  float tmpx, tmpy, tmpz;
  FILE *fp;
  //float points[NDAT + 150][NDAT];
  //Range range;
  //Point p[N];
  fp = fopen(fname,"r");
  for (i=0; i < NDAT; i++)
    for (j=0; j < NDAT; j++)
      flag[i][j] = 1; //全点を"使用"に初期化。
  for (i=0; i < NDAT; i++)
    for (j=0; j < NDAT; j++)
      p[i][j] = 0;
  getRange(range, fname);
  while (fscanf(fp, "%f %f %f", &tmpx, &tmpy, &tmpz) != EOF) {
    i = (int)tmpx - range->l;
    j = range->t - (int)tmpy;
    p[i][j] = tmpz;
  }
  return 0;
}

void getRange(Range *range, char* fname){
  float tmpx, tmpy, tmpz;
  FILE* fp;
  range->l = INT_MAX;
  range->t = INT_MIN;
  range->r = -1;
  range->b = -1;
  float maxx = -INFINITY, miny = INFINITY;
  fp = fopen(fname, "r");
  while (fscanf(fp, "%f %f %f", &tmpx, &tmpy, &tmpz) != EOF) {
    if(tmpx > maxx) maxx = tmpx;
    if(tmpx < range->l) range->l = (int)tmpx;
    if(tmpy < miny) miny = tmpy;
    if(tmpy > range->t) range->t = (int)tmpy;
  }
  range->r = (int)maxx - range->l;
  range->b = range->t - (int)miny;
  fclose(fp);
}

void cover_loss(float** points, Range range, char** flag)
{
    int i, j, cntlos = 0;
    float sum = 0;

    if (flag[0][0] == 1) if(points[0][0] <= -9999.00) {
        if (flag[1][0] == 1) if(points[1][0] > -9999.00) {cntlos++; sum += points[1][0];}
        if (flag[0][1] == 1) if(points[0][1] > -9999.00) {cntlos++; sum += points[0][1];}
        if (flag[1][1] == 1) if(points[1][1] > -9999.00) {cntlos++; sum += points[1][1];}
        if (cntlos == 0) flag[0][0] = 0;
        else points[0][0] = sum / cntlos;
    }

    for (i = 1; i < range.r; i++) {
        if (flag[i][0] == 1) if (points[i][0] <= -9999.00) {
            cntlos = 0; sum = 0;
            if (flag[i - 1][0] == 1) if(points[i - 1][0] > -9999.00) {cntlos++; sum += points[i - 1][0];}
            if (flag[i + 1][0] == 1) if(points[i + 1][0] > -9999.00) {cntlos++; sum += points[i + 1][0];}
            if (flag[i - 1][1] == 1) if(points[i - 1][1] > -9999.00) {cntlos++; sum += points[i - 1][1];}
            if (flag[i][1]     == 1) if(points[i][1]     > -9999.00) {cntlos++; sum += points[i][1];}
            if (flag[i + 1][1] == 1) if(points[i + 1][1] > -9999.00) {cntlos++; sum += points[i + 1][1];}
            if (cntlos == 0) flag[i][0] = 0;
            else points[i][0] = sum / cntlos;
        }
    }
    for (i = 1; i < range.b; i++) {
        if (flag[0][i] == 1) if (points[0][i] <= -9999.00) {
            cntlos = 0; sum = 0;
            if (flag[0][i - 1] == 1) if(points[0][i - 1] > -9999.00) {cntlos++; sum += points[0][i - 1];}
            if (flag[0][i + 1] == 1) if(points[0][i + 1] > -9999.00) {cntlos++; sum += points[0][i + 1];}
            if (flag[1][i - 1] == 1) if(points[1][i - 1] > -9999.00) {cntlos++; sum += points[1][i - 1];}
            if (flag[1][i]     == 1) if(points[1][i]     > -9999.00) {cntlos++; sum += points[1][i];}
            if (flag[1][i + 1] == 1) if(points[1][i + 1] > -9999.00) {cntlos++; sum += points[1][i + 1];}
            if (cntlos == 0) flag[0][i] = 0;
            else points[0][i] = sum / cntlos;
        }
    }
    for (i = 1; i < range.r; i++) {
        for (j = 1; j < range.b; j++) {
            if (flag[i][j] == 1) if (points[i][j] <= -9999.00) {
                cntlos = 0; sum = 0;
                if (flag[i - 1][j - 1] == 1) { if(points[i - 1][j - 1] > -9999.00) {cntlos++; sum += points[i - 1][j - 1];}}
                if (flag[i - 1][j]     == 1) { if(points[i - 1][j]     > -9999.00) {cntlos++; sum += points[i - 1][j];}}
                if (flag[i - 1][j + 1] == 1) { if(points[i - 1][j + 1] > -9999.00) {cntlos++; sum += points[i - 1][j + 1];}}
                if (flag[i][j - 1]     == 1) { if(points[i][j - 1]     > -9999.00) {cntlos++; sum += points[i][j - 1];}}
                //if (flag[i][1]       == 1) { if(points[i][1]         > -9999.00) {cntlos++; sum += points[i][1];}}
                if (flag[i][j - 1]     == 1) { if(points[i][j + 1]     > -9999.00) {cntlos++; sum += points[i][j + 1];}}
                if (flag[i + 1][j - 1] == 1) { if(points[i + 1][j - 1] > -9999.00) {cntlos++; sum += points[i + 1][j - 1];}}
                if (flag[i + 1][j]     == 1) { if(points[i + 1][j]     > -9999.00) {cntlos++; sum += points[i + 1][j];}}
                if (flag[i + 1][j + 1] == 1) { if(points[i + 1][j + 1] > -9999.00) {cntlos++; sum += points[i + 1][j + 1];}}

                if (cntlos == 0) flag[i][j] = 0;
                else points[i][j] = sum / cntlos;
            }
        }
    }
}

void array_to_Points(float** points, Range range, char** flag, Point p[], int *n)
{
    int i, j, cnt=0;

    for (i = 0; i <= range.r; i++) {
        for (j = 0; j <= range.b; j++) {
            if (flag[i][j] != 0 && cnt < N) {
                p[cnt].x = i + range.l;
                p[cnt].y = range.t - j;
                p[cnt].z = points[i][j];
                cnt++;
            }
        }
    }
    *n = cnt;
}
