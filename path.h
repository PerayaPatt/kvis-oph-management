#ifndef SP_H
#define SP_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#ifndef min
#define min(a,b) ((a<b) ? (a):(b))
#endif

#define N 100
#ifndef INF
#define INF 1e18
#endif
typedef struct { // store coordinate position
    double x,y;
} POS;

int n;
double *dist;
POS pos[N];

int idx(int i, int j){ // return index in 1D array when use 2D data
    return n*(i-1)+j;
}

void Floyd(int n, double *dist){ // Shortest path
    for(int k=1; k<=n; k++){
        for(int i=1; i<=n; i++){
            for(int j=1; j<=n; j++){
                if(dist[idx(i,k)]>=INF || dist[idx(k,j)]>=INF) continue;
                dist[idx(i,j)]=min(dist[idx(i,j)],dist[idx(i,k)]+dist[idx(k,j)]);
                dist[idx(j,i)]=dist[idx(i,j)];
            }
        }
    }
}

double find1(int i, double *xto){ // distance from the user to station i
    return xto[i];
}

double find2(int u, int v, double *dist){ // distance between station u and station v
    return dist[idx(u,v)];
}

int sortrk(int n, double *xto, int *rk, int id){ // sort stations in rk by the distances in xto
    int m=0;
    for(int i=1; i<=n; i++){ // assign station ID to rk
        if(i!=id) rk[++m]=i;
    }
    for(int i=1; i<=m; i++){ // sorting
        for(int j=i+1; j<=m; j++){
            if(xto[rk[j]]<xto[rk[i]]){
                int t=rk[i]; rk[i]=rk[j]; rk[j]=t;
            }
        }
    }
    return m;
}

int calc1(int n, double x0, double y0, POS pos[], double *xto, int *rk){ // sort stations from user's coordinate
    for(int i=1; i<=n; i++){
        double x=pos[i].x-x0;
        double y=pos[i].y-y0;
        xto[i]=sqrt(x*x+y*y);
    }
    return sortrk(n,xto,rk,0);
}

int calc2(int id, double *dist, int n, double *xto, int *rk){ // sort stations from user's station ID
    for(int i=1; i<=n; i++){
        xto[i]=find2(id,i,dist);
    }
    return sortrk(n,xto,rk,id);
}

void build(int m){ // precompute the distances between each station stored in "dist"
    if(m>=N) m=N-1;
    n=m;
    free(dist);
    dist=malloc((n*n+1)*sizeof(double));
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            double x=pos[i].x-pos[j].x;
            double y=pos[i].y-pos[j].y;
            dist[idx(i,j)]=sqrt(x*x+y*y);
        }
    }
    Floyd(n,dist);
}

int distance1(double x, double y, double *xto, int *rk){ // call "calc1" function when user enter the coordinate
    return calc1(n,x,y,pos,xto,rk);
}

int distance2(int id, double *xto, int *rk){ // call "calc2" function when user enter station ID
    return calc2(id,dist,n,xto,rk);
}
#endif
