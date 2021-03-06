#ifndef _SHAPE_H
#define _SHAPE_H 1
   int nsp, npu, npl;
   int nl, idl[NSPMAX];
   int nu, idu[NSPMAX];
   double xbl[NSPMAX], ybl[NSPMAX];
   double xbu[NSPMAX], ybu[NSPMAX];
   double thickness;

   void newShape(int, double *, int, double *, int, double *, double *,
                 int, double*, double *, double, double*, double*);

   double HicksHenneFun(int, int, double);
#endif
