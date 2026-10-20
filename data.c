#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "data.h"

void dbi(DB*d){  // empty db
    memset(d,0,sizeof *d);
    d->xs=d->xl=d->xu=d->xf=d->xp=d->xi=d->xd=d->xe=1;
}

St* fst(DB*d,ii id){rep(i,0,d->ns-1)if(d->s[i].id==id)return &d->s[i];return 0;}  // find station by id
Sl* fsl(DB*d,ii id){rep(i,0,d->nl-1)if(d->l[i].id==id)return &d->l[i];return 0;}  // find open time
Us* fus(DB*d,ii id){rep(i,0,d->nu-1)if(d->u[i].id==id)return &d->u[i];return 0;}  // find visitor

ii ast(DB*d,const char*c,const char*nm,const char*fc,double x,double y,int cap){    // add station
    if(d->ns>=MXS)return XFUL;
    St*s=&d->s[d->ns++];
    memset(s,0,sizeof *s);
    s->id=d->xs++;
    strncpy(s->c,c,CL-1);strncpy(s->nm,nm,NL-1);strncpy(s->fc,fc,NL-1);
    s->x=x;s->y=y;s->cap=cap;s->ac=1;  
    d->dt=1;
    return s->id;
}
ii asl(DB*d,ii sid,int day,mm a,mm b,int cap){  // add open time
    if(d->nl>=MXL)return XFUL;
    if(!fst(d,sid)||b<=a)return XARG;
    Sl*s=&d->l[d->nl++];
    s->id=d->xl++;s->sid=sid;s->d=day;s->s=a;s->e=b;s->cap=cap;
    d->dt=1;
    return s->id;
}
ii aus(DB*d,const char*nm,mm a,mm b){  // add visitor
    if(d->nu>=MXU)return XFUL;
    Us*u=&d->u[d->nu++];
    memset(u,0,sizeof *u);
    u->id=d->xu++;
    strncpy(u->nm,nm,NL-1);
    u->as=a;u->ae=b;u->sp=1.35;u->t=time(0);
    d->dt=1;
    return u->id;
}
ii afv(DB*d,ii uid,ii lid,double f){  // save favour
    Sl*s=fsl(d,lid);
    if(!s||!fus(d,uid)||f<0||f>10)return XARG;
    rep(i,0,d->nf-1)if(d->f[i].uid==uid&&d->f[i].lid==lid){d->f[i].f=f;return d->f[i].id;}
    if(d->nf>=MXF)return XFUL;
    Fv*v=&d->f[d->nf++];
    v->id=d->xf++;v->uid=uid;v->lid=lid;v->sid=s->sid;v->f=f;v->z=0;v->t=time(0);
    d->dt=1;
    return v->id;
}
int gps(DB*d,ii uid,double x,double y,double acc){    // save visitor x y
    Us*u=fus(d,uid);
    if(!u)return XNF;
    u->x=x;u->y=y;u->acc=acc;u->gps=1;  
    d->dt=1;
    return OK;
}
ii ase(DB*d,ii uid,int ft){  // start event
    if(d->ne>=MXE)return XFUL;
    Se*e=&d->e[d->ne++];
    memset(e,0,sizeof *e);
    e->id=d->xe++;e->uid=uid;e->ft=ft;e->t=time(0);
    return e->id;
}
int cse(DB*d,ii eid,int ni,int no,double bv,double ms){  // close event
    rep(i,0,d->ne-1)if(d->e[i].id==eid){
        d->e[i].ni=ni;d->e[i].no=no;d->e[i].bv=bv;d->e[i].ms=ms;
        d->dt=1;
        return OK;
    }
    return XNF;
}
ii apl(DB*d,ii eid,ii uid,int rk,const ii*ls,int n){  // save one plan
    if(d->np>=MXP||d->ni+n>MXI)return XFUL;
    Pl*p=&d->p[d->np++];
    memset(p,0,sizeof *p);
    p->id=d->xp++;p->eid=eid;p->uid=uid;p->rk=rk;p->nk=n;
    mm lo=32000,hi=-1;
    rep(j,0,n-1){
        Sl*s=fsl(d,ls[j]);
        if(!s)continue;
        double f=gtf(d,uid,ls[j]);
        Pi*t=&d->i[d->ni++];
        t->id=d->xi++;t->pid=p->id;t->lid=s->id;t->sid=s->sid;t->o=j+1;t->f=f;
        p->tv+=f;
        p->bz=(mm)(p->bz+len(s));
        if(s->s<lo)lo=s->s;
        if(s->e>hi)hi=s->e;
    }
    if(hi>lo)p->idl=(mm)(hi-lo-p->bz);
    d->dt=1;
    return p->id;
}
ii ads(DB*d,ii eid,ii uid,ii sid,double sm,double pm,int mt){  // save one distance
    if(d->nd>=MXD)return XFUL;
    Us*u=fus(d,uid);
    Ds*x=&d->d[d->nd++];
    x->id=d->xd++;x->eid=eid;x->uid=uid;x->sid=sid;
    x->dm=sm;x->pm=pm;x->mt=mt;x->rk=0;
    x->wm=w2m(pm,u?u->sp:1.35);
    d->dt=1;
    return x->id;
}

int lof(DB*d,ii sid,Sl*o,int mx){  // open times of station
    int n=0;
    rep(i,0,d->nl-1)if(d->l[i].sid==sid&&n<mx)o[n++]=d->l[i];
    return n;
}
int pou(DB*d,ii uid,Pl*o,int mx){  // plans of visitor
    int n=0;
    rep(i,0,d->np-1)if(d->p[i].uid==uid&&n<mx)o[n++]=d->p[i];
    return n;
}
int iop(DB*d,ii pid,Pi*o,int mx){  // stops of plan
    int n=0;
    rep(i,0,d->ni-1)if(d->i[i].pid==pid&&n<mx)o[n++]=d->i[i];
    return n;
}
mm len(const Sl*s){return (mm)(s->e-s->s);}  // how many minutes
double gtf(DB*d,ii uid,ii lid){  // favour of one time
    rep(i,0,d->nf-1)if(d->f[i].uid==uid&&d->f[i].lid==lid)return d->f[i].f;
    return 0;
}
double w2m(double m,double sp){return sp>0?m/sp/60:0;}  // metres to minutes
void m2s(mm t,char*b,size_t n){snprintf(b,n,"%02d:%02d",t/60,t%60);}  // 540 to 09:00

int lod(DB*d){  // built in rooms, if no file
    if(d->ns)return OK;
    static const struct{const char*r,*c,*f;double x,y;int cap;}T[]={    // room, code, area, x, y, seats
        {"103","AR","Arts",30,40,30},  
        {"115","OT","Occupations",150,40,30},  
        {"212","MA","Mathematics",120,140,30},  
        {"213","MA","Mathematics",130,140,30},  
        {"232","TH","Thai",320,140,30},  
        {"243","ST","Science & Tech",430,140,30},  
        {"312","EN","English",120,240,30},  
        {"313","EN","English",130,240,30},  
        {"322","ST","Science & Tech",220,240,30},  
        {"GYM","HP","Health & PE",480,60,60},  
        {"100 seat","SO","Social Studies",260,320,100},  
    };  
    int n=sizeof T/sizeof T[0];  
    rep(i,0,n-1){  
        ii a=ast(d,T[i].c,T[i].r,T[i].f,T[i].x,T[i].y,T[i].cap);  
        mm s0=(mm)(540+i*15%90),st=(mm)(70+i%4*10),ln=(mm)(40+i%3*10);    // made up open times
        rep(j,0,3)asl(d,a,1,(mm)(s0+j*st),(mm)(s0+j*st+ln),T[i].cap);  
    }  
    return OK;
}

int nrm(DB*d){  // z score per visitor
    rep(i,0,d->nu-1){
        double s=0,q=0;
        long n=0;
        rep(j,0,d->nf-1)if(d->f[j].uid==d->u[i].id){s+=d->f[j].f;n++;}
        if(n<2)continue;
        double mu=s/n;
        rep(j,0,d->nf-1)if(d->f[j].uid==d->u[i].id)q+=(d->f[j].f-mu)*(d->f[j].f-mu);
        double sd=sqrt(q/(n-1));
        rep(j,0,d->nf-1)if(d->f[j].uid==d->u[i].id)d->f[j].z=sd>1e-9?(d->f[j].f-mu)/sd:0;
    }
    return OK;
}
static int cmpd(const void*a,const void*b){  // sort helper
    double x=*(const double*)a,y=*(const double*)b;
    return x<y?-1:x>y;
}
void stc(const double*x,long n,Stt*o){  // mean sd median
    memset(o,0,sizeof *o);
    if(n<1)return;
    static double b[MXF];
    rep(i,0,(int)n-1)b[i]=x[i];
    qsort(b,n,sizeof(double),cmpd);
    o->n=n;o->mn=b[0];o->mx=b[n-1];
    double s=0;
    rep(i,0,(int)n-1)s+=b[i];
    o->mu=s/n;
    double q=0;
    rep(i,0,(int)n-1)q+=(b[i]-o->mu)*(b[i]-o->mu);
    o->vr=n>1?q/(n-1):0;
    o->sd=sqrt(o->vr);
    o->se=n>0?o->sd/sqrt((double)n):0;
    o->md=n%2?b[n/2]:(b[n/2-1]+b[n/2])/2;
    o->q1=b[n/4];o->q3=b[(3*n)/4];o->iqr=o->q3-o->q1;
    o->cv=o->mu>1e-9?o->sd/o->mu:0;
    o->lo=o->mu-1.96*o->se;o->hi=o->mu+1.96*o->se;
    return;
}
int anz(DB*d,Ana*a){  // all the statistics
    memset(a,0,sizeof *a);
    static double v[MXF];
    long n=0;
    rep(i,0,d->nf-1)v[n++]=d->f[i].f;
    stc(v,n,&a->gf);
    a->ns=0;
    rep(i,0,d->ns-1){
        Sst*s=&a->s[a->ns];
        long m=0;
        rep(j,0,d->nf-1)if(d->f[j].sid==d->s[i].id)v[m++]=d->f[j].f;
        if(!m)continue;
        s->sid=d->s[i].id;
        strncpy(s->nm,d->s[i].nm,NL-1);
        stc(v,m,&s->f);
        s->nr=m;
        rep(j,0,d->ni-1)if(d->i[j].sid==d->s[i].id)s->nc++;
        s->sr=m?(double)s->nc/m:0;
        s->pz=a->gf.sd>1e-9?(s->f.mu-a->gf.mu)/a->gf.sd:0;
        s->ol=fabs(s->pz)>2.5;
        a->ns++;
    }
    a->nb=0;
    for(mm t=D0;t<D1;t=(mm)(t+BW)){
        Tbs*b=&a->b[a->nb];
        b->d=1;b->bs=t;b->be=(mm)(t+BW);
        double s=0;
        rep(i,0,d->nl-1){
            if(d->l[i].e<=t||d->l[i].s>=t+BW)continue;
            b->seat+=d->l[i].cap;
            rep(j,0,d->nf-1)if(d->f[j].lid==d->l[i].id){b->nq++;s+=d->f[j].f;}
            rep(j,0,d->ni-1)if(d->i[j].lid==d->l[i].id)b->nc++;
        }
        b->mf=b->nq?s/b->nq:0;
        b->lr=b->seat?(double)b->nc/b->seat:0;
        a->nb++;
    }
    a->t=time(0);
    return OK;
}
