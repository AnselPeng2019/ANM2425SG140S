#ifndef _GETKBY_H_
#define _GETKBY_H_

int Getk(int x1, int x2, int y1, int y2, int amp);
int Getb(int x, int y, int k, int amp);
int Gety(int x, int k, int b, int amp);



void Get_kf_bf(int *parax, int *paray, int *kf, int *bf, int dim);
int Gety_final(int pwr, int *parax, int *kf, int *bf, int dim);

#endif // _GETKBY_H_
