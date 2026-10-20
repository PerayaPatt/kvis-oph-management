#ifndef DB_H
#define DB_H
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define MXS 64  // max stations
#define MXL 512  // max open times
#define MXU 1000  // max visitors
#define MXF 20000  // max favours
#define MXP 3000
#define MXI 12000
#define MXD 20000
#define MXE 4000
#define TOP 3  // how many plans
#define MXK 16  // stops per plan
#define NL 64  // name length
#define CL 16
#define BW 30  // half hour block
#define D0 480  // day start 08:00
#define D1 1200  // day end 20:00
#define MXB ((D1-D0)/BW)
#define INF 1e18
#define rep(i,a,b) for(int i=(a);i<=(b);i++)  // short for loop

typedef int32_t ii;  // id number
typedef int16_t mm;  // minutes from midnight

enum{OK=0,XFUL=-1,XNF=-2,XARG=-3,XIO=-4,XPAR=-5};  // error codes
enum{FSCH=1,FRUT=2};  // what the visitor did
enum{MEUC=1,MGRA=2};  // how distance measured

typedef struct{ii id;char c[CL],nm[NL],fc[NL];double x,y;int cap,ac;}St;  // station = one room
typedef struct{ii id,sid;int d;mm s,e;int cap;}Sl;  // one open time
typedef struct{ii id;char nm[NL];double x,y,acc,sp;int gps;mm as,ae;time_t t;}Us;  // visitor
typedef struct{ii id,uid,lid,sid;double f,z;time_t t;}Fv;  // favour 0-10
typedef struct{ii id,uid,eid;int rk;double tv;int nk;mm bz,idl;double wm;}Pl;  // one plan
typedef struct{ii id,pid,lid,sid;int o;double f;}Pi;  // one stop in plan
typedef struct{ii id,uid,sid,eid;double dm,pm,wm;int rk,mt;}Ds;  // distance to station
typedef struct{ii id,uid;int ft;time_t t;double ms;int ni,no;double bv;}Se;  // visit log

typedef struct{long n;double mu,vr,sd,se,mn,q1,md,q3,mx,iqr,cv,lo,hi;}Stt;  // mean sd median etc
typedef struct{ii sid;char nm[NL];Stt f;long nr,nc;double sr,pz;int ol;}Sst;  // stats of one station
typedef struct{int d;mm bs,be;long nq,nc;double mf;int seat;double lr;}Tbs;  // stats per half hour
typedef struct{Stt gf;Sst s[MXS];int ns;Tbs b[MXB*4];int nb;time_t t;}Ana;  // all stats 

typedef struct{
    St s[MXS];int ns;  // stations
    Sl l[MXL];int nl;  // open times
    Us u[MXU];int nu;
    Fv f[MXF];int nf;
    Pl p[MXP];int np;
    Pi i[MXI];int ni;
    Ds d[MXD];int nd;
    Se e[MXE];int ne;
    ii xs,xl,xu,xf,xp,xi,xd,xe;  // next free id
    int dt;
}DB;

void dbi(DB*);  // set up, clear

ii ast(DB*,const char*,const char*,const char*,double,double,int);  // add things
ii asl(DB*,ii,int,mm,mm,int);
ii aus(DB*,const char*,mm,mm);
ii afv(DB*,ii,ii,double);
int gps(DB*,ii,double,double,double);

ii ase(DB*,ii,int);
int cse(DB*,ii,int,int,double,double);
ii apl(DB*,ii,ii,int,const ii*,int);
ii ads(DB*,ii,ii,ii,double,double,int);

St* fst(DB*,ii);  // find things
Sl* fsl(DB*,ii);
Us* fus(DB*,ii);

int lof(DB*,ii,Sl*,int);  // lists
int pou(DB*,ii,Pl*,int);
int iop(DB*,ii,Pi*,int);

mm len(const Sl*);
double gtf(DB*,ii,ii);
double w2m(double,double);
void m2s(mm,char*,size_t);

int lod(DB*);

void stc(const double*,long,Stt*);  // stats

int nrm(DB*);
int anz(DB*,Ana*);

#endif
