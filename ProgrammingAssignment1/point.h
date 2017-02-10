#ifndef _POINT_H_
#define _POINT_H_
/*
 * Simplistic definition of point and operations on
 * a point for 2D double-precision space.
 */

typedef struct PointStruct {
  double x;
  double y;
} Point;

void point_translate(Point *p, double x, double y);
double point_distance(const Point *p1, const Point *p2);

static inline double point_getX(const Point *p){
  return p->x;
}
static inline double point_getY(const Point *p){
  return p->y;
}
static inline Point *point_set(Point *p, double x, double y){
  p->x = x; 
  p->y = y; 
  return p;
}


#endif
