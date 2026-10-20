#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "front.h"

#include "path.h"  // shortest path
#include "interval.h"  // interval scheduling
#include "admin.h"  // admin + file

#define DX 200 
#define DY 0

static int TH=1;  // colour
static int AD=0;  // admin mode
static char BUF[600];  // shared text buffer
static char T0[80],T1[80],T2[80],T3[80],T4[80],T5[80],T6[80],T7[80];

static const char* c(const char*s){return TH?s:"";}  // colour or black-white
void thm(int on){TH=on;}  // colour on off
static void slp(int ms){clock_t t=clock();while((clock()-t)*1000/CLOCKS_PER_SEC<ms);}  // wait ms

static char* rl(void){  // read one line
    if(!fgets(BUF,sizeof BUF,stdin))return 0;
    size_t n=strlen(BUF);
    while(n&&(BUF[n-1]=='\n'||BUF[n-1]=='\r'||BUF[n-1]==' '||BUF[n-1]=='\t'))BUF[--n]=0; // discard space at the end
    char*p=BUF;while(*p==' '||*p=='\t')p++;  // skip front spaces
    return p;
}
static int dw(const char*s){  // count letters, skip colour
    int n=0;
    while(*s){
        if(*s=='\x1b'){while(*s&&*s!='m')s++;if(*s)s++;continue;}  // jump over colour code
        if((*s&0xC0)!=0x80)n++;  // count utf8 letters
        s++;
    }
    return n;
}

void nl(int k){rep(i,1,k)putchar('\n');}  // empty lines
void cls(void){if(TH)printf("\x1b[2J\x1b[H");else nl(2);}  // clear screen

void pad(const char*s,int n){ 
    int k=0;const char*p=s;
    while(*p&&k<n){
        if(*p=='\x1b'){while(*p&&*p!='m')p++;if(*p)p++;continue;}
        unsigned char u=*p;int b=1;
        if(u>=0xF0)b=4;else if(u>=0xE0)b=3;else if(u>=0xC0)b=2;  // how many bytes
        fwrite(p,1,b,stdout);p+=b;k++;
    }
    while(k<n){putchar(' ');k++;}  // fill the rest space
}
void ctr(const char*s){  // centre the text
    int k=dw(s),l=(W-k)/2;  // left gap
    if(l<0)l=0;
    rep(i,1,l)putchar(' ');
    printf("%s\n",s);
}
void hrl(char t){  // line across
    const char*u=t=='='?"\u2550":t=='.'?"\u2504":HZ;  // choose line styLe
    printf("%s",c(K3));
    rep(i,1,W)printf("%s",u);
    printf("%s\n",c(R));
}
static void bxt(void){printf("%s%s",c(K1),TL);rep(i,1,W-2)printf("%s",HZ);printf("%s%s\n",TR,c(R));}  // box top
static void bxs(void){printf("%s%s",c(K1),LT);rep(i,1,W-2)printf("%s",HZ);printf("%s%s\n",RT,c(R));}  // box divider
static void bxb(void){printf("%s%s",c(K1),BL);rep(i,1,W-2)printf("%s",HZ);printf("%s%s\n",BR,c(R));}  // box bottom
static void bxl(const char*s,const char*col){  // one box row
    printf("%s%s%s ",c(K1),VT,c(col));
    pad(s,W-3);
    printf("%s%s%s\n",c(K1),VT,c(R));
}
void bx(const char*t,const char*b){  // box with title
    bxt();
    if(t&&*t){bxl(t,B);bxs();}
    if(b&&*b){
        const char*p=b;char q[300];
        while(*p){
            int i=0;
            while(*p&&*p!='\n'&&i<290)q[i++]=*p++;  // one row per \n
            q[i]=0;
            if(*p=='\n')p++;
            bxl(q,K3);
        }
    }
    bxb();
}
void ttl(const char*s){  // section title
    nl(1);
    printf(" %s%s%s %s%s%s\n",c(K2),STR,c(R),c(B),s,c(R));
    hrl('-');
}
void ban(void){  // big logo
    static const char*L[]={  // kvis logo
        "██  ██  ██   ██  ██  ███████",
        "██ ██   ██   ██  ██  ██     ",
        "████    ██   ██  ██  ███████",
        "██ ██    ██ ██   ██       ██",
        "██  ██    ███    ██  ███████"};
    nl(1);hrl('=');nl(1);
    rep(i,0,4){sprintf(BUF,"%s%s%s",c(K1),L[i],c(R));ctr(BUF);}
    nl(1);
    sprintf(BUF,"%s%sO P E N   H O U S E%s",c(B),c(K2),c(R));ctr(BUF);
    sprintf(BUF,"%s%s %s v%s%s",c(K3),SUB,DOT,VER,c(R));ctr(BUF);
    nl(1);hrl('=');
}

int mnu(const char*t,const char**o,int n){  // menu show choices
    nl(1);
    printf(" %s%s%s %s%s%s\n",c(K2),ARW,c(R),c(B),t,c(R));
    hrl('.');
    rep(i,1,n)printf("   %s%s%d%s   %s\n",c(B),c(K1),i,c(R),o[i-1]);  // list the options
    printf("   %s0%s   %sback / exit%s\n",c(K3),c(R),c(K3),c(R));
    hrl('.');
    return gi("choice",0,n);  // 0 = back
}
int gi(const char*q,int lo,int hi){  // ask whole number
    for(;;){
        printf(" %s%s%s %s %s[%d-%d]%s: ",c(K2),ARW,c(R),q,c(K3),lo,hi,c(R));
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        if(!*p){er_("empty");continue;}
        char*e;long v=strtol(p,&e,10);  // text to number
        while(*e==' ')e++;
        if(*e||v<lo||v>hi){er_("out of range");continue;}  // junk or out of range
        return (int)v;
    }
}
double gd(const char*q,double lo,double hi){  // ask number, blank = low
    for(;;){
        printf(" %s%s%s %s %s[%.4g-%.4g]%s: ",c(K2),ARW,c(R),q,c(K3),lo,hi,c(R));
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        if(!*p)return lo;  // blank = lowest
        char*e;double v=strtod(p,&e);
        while(*e==' ')e++;
        if(*e||v<lo||v>hi){er_("out of range");continue;}
        return v;
    }
}
void gs(const char*q,char*o,int n){  // ask text
    for(;;){
        printf(" %s%s%s %s: ",c(K2),ARW,c(R),q);
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        if(!*p){er_("empty");continue;}
        strncpy(o,p,n-1);o[n-1]=0;  // copy, keep 0 at end
        return;
    }
}
mm gt(const char*q){  // ask time hh:mm
    for(;;){
        printf(" %s%s%s %s %s[HH:MM]%s: ",c(K2),ARW,c(R),q,c(K3),c(R));
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        int h=-1,m=0;
        if(sscanf(p,"%d:%d",&h,&m)==2||sscanf(p,"%d.%d",&h,&m)==2||sscanf(p,"%d",&h)==1)  // 9:30 or 9.30 or 9
            if(h>=0&&h<24&&m>=0&&m<60)return (mm)(h*60+m);  // convert into minutes
        er_("use HH:MM");
    }
}
int gy(const char*q){  // ask yes no
    for(;;){
        printf(" %s%s%s %s %s[y/n]%s: ",c(K2),ARW,c(R),q,c(K3),c(R));
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        if(*p=='y'||*p=='Y')return 1;  // only y or n
        if(*p=='n'||*p=='N')return 0;
        er_("y or n");
    }
}
int gsid(DB*d,const char*q){  // ask which station
    for(;;){
        printf(" %s%s%s %s %s[room or # , blank=cancel]%s: ",c(K2),ARW,c(R),q,c(K3),c(R));
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        if(!*p)return 0;
        St*s=0;
        rep(i,0,d->ns-1)if(!strcmp(d->s[i].nm,p))return d->s[i].id;  // match room name
        char*e;long v=strtol(p,&e,10);
        if(!*e&&v>0){s=fst(d,(ii)v);if(s)return s->id;}  // or match #
        er_("unknown station");
    }
}
int gcoord(const char*q,double*x,double*y){  // ask x y
    for(;;){
        printf(" %s%s%s %s %s[x,y | g=main gate | blank=cancel]%s: ",c(K2),ARW,c(R),q,c(K3),c(R));
        fflush(stdout);
        char*p=rl();
        if(!p)exit(0);
        if(!*p||*p=='x'||*p=='X')return 0;  // blank = cancel
        if(*p=='g'||*p=='G'){*x=DX;*y=DY;return 1;}
        double a,b;
        if(sscanf(p,"%lf,%lf",&a,&b)==2||sscanf(p,"%lf %lf",&a,&b)==2)  // 120,85 or 120 85
            if(a>=-1e5&&a<=1e5&&b>=-1e5&&b<=1e5){*x=a;*y=b;return 1;}
        er_("format: 120,85");
    }
}

void msg(const char*col,const char*s){printf("   %s%s%s %s%s%s\n",c(col),DOT,c(R),c(col),s,c(R));}  // dot + text
void ok_(const char*s){msg(KG,s);}  // green
void er_(const char*s){msg(KR,s);}  // red
void wn_(const char*s){msg(KY,s);}  // yellow
void in_(const char*s){msg(K1,s);}  // blue
void hlp(const char*s){printf("   %s%s%s\n",c(K3),s,c(R));}  // grey hint
void wait_(void){printf("\n   %spress ENTER%s",c(K3),c(R));fflush(stdout);rl();}  // wait for enter
void spn(const char*s,int n){  // spinner
    static const char*f[]={"⠋","⠙","⠹","⠸","⠼","⠴","⠦","⠧","⠇","⠏"};  // spinner frames
    rep(i,0,n-1){printf("\r   %s%s%s %s ",c(K2),f[i%10],c(R),s);fflush(stdout);slp(55);}
    printf("\r   %s%s%s %s \n",c(KG),DOT,c(R),s);
}
void bar(double p,int n){  // bar chart
    if(p<0)p=0;
    if(p>1)p=1;
    int k=(int)(p*n+0.5);  // how many full blocks
    printf("%s",c(p>0.9?KR:p>0.6?KY:K1));  // red = busy
    rep(i,1,k)printf("█");
    printf("%s",c(K3));
    rep(i,k+1,n)printf("░");
    printf("%s",c(R));
}
void tick(int i,int n,const char*s){printf("\n   %s[%d/%d]%s %s%s%s\n",c(K3),i,n,c(R),c(B),s,c(R));}  // step counter

static void tln(const int*w,int n,const char*l,const char*m,const char*r){  // table border
    printf("%s%s",c(K3),l);
    rep(j,0,n-1){rep(i,1,w[j]+2)printf("%s",HZ);if(j<n-1)printf("%s",m);}  // one piece per column
    printf("%s%s\n",r,c(R));
}
void tbh(const char**h,const int*w,int n){  // table head
    tln(w,n,TL,"\u252c",TR);
    printf("%s%s%s",c(K3),VT,c(R));
    rep(j,0,n-1){printf(" %s%s",c(B),c(K2));pad(h[j],w[j]);printf("%s %s%s%s",c(R),c(K3),VT,c(R));}
    putchar('\n');
    tln(w,n,LT,"\u253c",RT);
}
void tbr(const char**v,const int*w,int n){  // table row
    printf("%s%s%s",c(K3),VT,c(R));
    rep(j,0,n-1){putchar(' ');pad(v[j],w[j]);printf(" %s%s%s",c(K3),VT,c(R));}
    putchar('\n');
}
void tbf(const int*w,int n){tln(w,n,BL,"\u2534",BR);}  // table bottom

void pst(DB*d,int all){  // table of stations
    const char*h[]={"#","code","station","faculty","cap"};  // column names
    int w[]={3,6,26,16,4};  // column widths
    ttl("STATIONS");
    if(!d->ns){wn_("no station data");return;}
    tbh(h,w,5);
    int k=0;
    rep(i,0,d->ns-1){
        St*s=&d->s[i];
        if(!all&&!s->ac)continue;  // skip closed rooms
        k++;
        sprintf(T0,"%d",(int)s->id);
        sprintf(T4,"%d",s->cap);
        const char*v[]={T0,s->c,s->nm,s->fc,T4};
        tbr(v,w,5);
    }
    tbf(w,5);
    sprintf(BUF,"%d station(s)",k);
    in_(BUF);
}
void psl(DB*d,ii sid){  // table of open times
    St*s=fst(d,sid);
    if(!s){er_("no such station");return;}
    static Sl b[MXL];
    int n=lof(d,sid,b,MXL);  // times of this room
    if(!n){wn_("no time slot");return;}
    const char*h[]={"#","day","time","seats"};
    int w[]={4,3,13,5};
    tbh(h,w,4);
    rep(i,0,n-1){
        char x[12],y[12];
        m2s(b[i].s,x,12);m2s(b[i].e,y,12);  // 540 to 09:00
        sprintf(T0,"%d",(int)b[i].id);
        sprintf(T1,"%d",b[i].d);
        sprintf(T2,"%s - %s",x,y);
        sprintf(T3,"%d",b[i].cap);
        const char*v[]={T0,T1,T2,T3};
        tbr(v,w,4);
    }
    tbf(w,4);
}
void ppi(DB*d,ii pid){  // stops of one plan
    static Pi it[MXK];
    int n=iop(d,pid,it,MXK);  // stops of this plan
    if(!n)return;
    const char*h[]={"no","time","station","favour"};
    int w[]={2,13,26,6};
    tbh(h,w,4);
    rep(i,0,n-1){
        Sl*s=fsl(d,it[i].lid);
        St*t=fst(d,it[i].sid);
        char x[12],y[12];
        if(s){m2s(s->s,x,12);m2s(s->e,y,12);}else{strcpy(x,"?");strcpy(y,"?");}
        sprintf(T0,"%d",it[i].o);
        sprintf(T1,"%s - %s",x,y);
        sprintf(T2,"%.1f",it[i].f);
        const char*v[]={T0,T1,t?t->nm:"?",T2};
        tbr(v,w,4);
    }
    tbf(w,4);
}
void ppl(DB*d,ii uid){  // the 3 plans
    static Pl p[MXP];
    int n=pou(d,uid,p,MXP);
    if(!n){wn_("no plan yet");return;}
    ii e=0;
    rep(i,0,n-1)if(p[i].eid>e)e=p[i].eid;  // newest run only
    ttl("YOUR 3 BEST PLANS");
    rep(i,0,n-1){
        if(p[i].eid!=e)continue;
        char st[16];st[0]=0;
        rep(j,1,4-p[i].rk)strcat(st,STR);  // stars by rank
        char x[12],y[12];
        m2s(p[i].bz,x,12);m2s(p[i].idl,y,12);
        sprintf(BUF,"%s%s#%d%s  %s%s%s   favour %s%.1f%s   %d stop(s)   busy %s   idle %s",
            c(B),c(K2),p[i].rk,c(R),c(KY),st,c(R),c(B),p[i].tv,c(R),p[i].nk,x,y);
        nl(1);
        printf("   %s\n",BUF);
        ppi(d,p[i].id);
    }
}
void pds(DB*d,ii uid){  // nearest table
    ii e=0;
    rep(i,0,d->nd-1)if(d->d[i].uid==uid&&d->d[i].eid>e)e=d->d[i].eid;  // newest search only
    if(!e){wn_("no distance data");return;}
    ttl("NEAREST STATIONS");
    const char*h[]={"rk","code","station","metres","walk"};
    int w[]={3,6,26,7,7};
    tbh(h,w,5);
    int k=0;
    rep(r,1,d->ns){  // print in rank order
        rep(i,0,d->nd-1){
            Ds*x=&d->d[i];
            if(x->eid!=e||x->rk!=r)continue;
            St*s=fst(d,x->sid);
            sprintf(T0,"%d",x->rk);
            sprintf(T3,"%.0f",x->pm);
            sprintf(T4,"%.1f min",x->wm);
            const char*v[]={T0,s?s->c:"?",s?s->nm:"?",T3,T4};
            tbr(v,w,5);
            k++;
        }
    }
    tbf(w,5);
    sprintf(BUF,"%d station(s) ranked",k);
    in_(BUF);
}
void pss(const Ana*a){  // stats per station
    ttl("STATION STATISTICS");
    if(!a->ns){wn_("no data");return;}
    const char*h[]={"#","station","n","mean","sd","med","95% ci","sel%","!"};
    int w[]={3,22,3,5,5,5,15,5,1};
    tbh(h,w,9);
    rep(i,0,a->ns-1){
        const Sst*s=&a->s[i];
        sprintf(T0,"%d",(int)s->sid);
        sprintf(T1,"%ld",s->f.n);
        sprintf(T2,"%.2f",s->f.mu);
        sprintf(T3,"%.2f",s->f.sd);
        sprintf(T4,"%.2f",s->f.md);
        sprintf(T5,"%.2f - %.2f",s->f.lo,s->f.hi);
        sprintf(T6,"%.0f",s->sr*100);
        sprintf(T7,"%s",s->ol?"*":" "); 
        const char*v[]={T0,s->nm,T1,T2,T3,T4,T5,T6,T7};
        tbr(v,w,9);
    }
    tbf(w,9);
    hlp("mean = raw favour  |  sel% = share of raters that got it scheduled  |  * = outlier");
}
void ptb(const Ana*a){  // demand bars
    ttl("DEMAND BY TIME");
    if(!a->nb){wn_("no slot data");return;}
    rep(i,0,a->nb-1){
        const Tbs*b=&a->b[i];
        char x[12],y[12];
        m2s(b->bs,x,12);m2s(b->be,y,12);
        printf("   %s%s-%s%s ",c(K3),x,y,c(R));
        bar(b->lr,26);
        printf("  %s%3.0f%%%s  n=%ld  fav=%.1f\n",c(b->lr>0.9?KR:K3),b->lr*100,c(R),b->nc,b->mf);
    }
    hlp("bar = seats taken / seats offered, red = queue expected");
}

ii who(DB*d){  // sign in by name
    char nm[NL];
    ttl("SIGN IN");
    gs("your name",nm,NL);
    rep(i,0,d->nu-1)  // name used before ?
        if(!strcmp(d->u[i].nm,nm)){
            sprintf(BUF,"welcome back %s",nm);
            ok_(BUF);
            return d->u[i].id;
        }
    ii id=aus(d,nm,D0,D1);  // new visitor
    if(id<=0){er_("visitor table full");return 0;}
    sprintf(BUF,"registered as visitor #%d",(int)id);
    ok_(BUF);
    return id;
}
int f1(DB*d,ii uid){  // menu 1, plan visit
    cls();
    ttl("PLAN MY VISIT");
    Us*u=fus(d,uid);
    if(!u)return XNF;
    for(;;){
        mm a=gt("free from"),z=gt("free until");  // ask free time
        if(z>a){u->as=a;u->ae=z;ok_("saved");break;}
        wn_("end must be after start");
    }
    static period q[MXT];
    int cnt=0,seen=0,nq=0;
    rep(i,0,d->ns-1){
        St*s=&d->s[i];
        if(!s->ac)continue;
        static Sl b[MXL];
        int n=lof(d,s->id,b,MXL);
        if(!n)continue;
        seen++;
        sprintf(BUF,"%s  %s  %s%s",s->c,s->nm,c(K3),s->fc);
        tick(seen,d->ns,BUF);
        psl(d,s->id);
        double f=gd("favour of this station",0,10);  // favour per station
        if(f<=0)continue;
        cnt++;
        rep(j,0,n-1){
            if(nq>=MXT)break;
            afv(d,uid,b[j].id,f);
            q[nq].id=b[j].id;q[nq].first=b[j].s;q[nq].second=b[j].e;q[nq].val=f;nq++;  // keep times for the dp
        }
    }
    if(!cnt){wn_("nothing rated, nothing to schedule");wait_();return OK;}
    ii e=ase(d,uid,FSCH);  // one run = one event
    spn("running interval scheduling",14);
    clock_t t=clock();
    static int arr[MXT];
    ii ls[MXK],bl[TOP];
    int k=0,nb=0;
    rep(r,1,TOP){  // 3 plans
        int m=0;
        rep(j,0,nq-1){
            int x=0;
            rep(z,0,nb-1)if(q[j].id==bl[z])x=1;  // skip banned time
            if(!x)pd[m++]=q[j];
        }
        int g=func(pd,m,u->as,u->ae,arr);  // interval scheduling
        if(g>MXK)g=MXK;
        if(!g)break;
        rep(j,0,g-1)ls[j]=arr[j];
        apl(d,e,uid,r,ls,g);  // keep this plan
        k++;
        bl[nb++]=ls[0];
    }
    double ms=(double)(clock()-t)*1000/CLOCKS_PER_SEC;
    double bv=0;
    rep(i,0,d->np-1)if(d->p[i].eid==e&&d->p[i].tv>bv)bv=d->p[i].tv;  // best favour of the 3
    cse(d,e,cnt,k,bv,ms);
    if(k<=0){er_("no feasible plan");wait_();return OK;}
    cls();
    ppl(d,uid);
    sprintf(BUF,"%d station(s) rated %s %d plan(s) , best favour %.1f , %.1f ms",cnt,ARW,k,bv,ms);
    nl(1);
    in_(BUF);
    wait_();
    return OK;
}
int f2(DB*d,ii uid){  // menu 2, nearest station
    cls();
    ttl("FIND NEAREST STATION");
    static const char*o[]={"from a station number","from my position (x,y)"};
    int w=mnu("start from",o,2);  // 1.1 or 1.2
    if(!w)return OK;
    double x=0,y=0;
    int id=0;
    if(w==1){
        pst(d,0);
        ii sid=gsid(d,"you are at station");
        if(!sid){wn_("cancelled");wait_();return OK;}
        rep(i,1,d->ns)if(d->s[i-1].id==sid)id=i;  // station id to index
    }else{
        hlp("read your position from the campus map, x and y in metres");
        if(!gcoord("your position",&x,&y)){wn_("cancelled");wait_();return OK;}
        gps(d,uid,x,y,0);
        sprintf(BUF,"position %.1f , %.1f",x,y);
        ok_(BUF);
    }
    ii e=ase(d,uid,FRUT);
    spn("measuring every station",12);
    clock_t t=clock();
    static double xto[N+1];  // distance to each room
    static int rk[N+1];  // room order, nearest first
    int g=w==1?distance2(id,xto,rk):distance1(x,y,xto,rk);  // ranking from path.h
    int k=0;
    rep(i,1,g){  // walk the ranking
        St*s=&d->s[rk[i]-1];  // index back to station
        if(!s->ac)continue;
        double m=find1(rk[i],xto);  // metres
        if(ads(d,e,uid,s->id,m,m,w==1?MGRA:MEUC)>0)d->d[d->nd-1].rk=++k;  // save with rank
    }
    double ms=(double)(clock()-t)*1000/CLOCKS_PER_SEC;
    double bv=1e18;
    rep(i,0,d->nd-1)if(d->d[i].eid==e&&d->d[i].pm<bv)bv=d->d[i].pm;  // nearest one
    cse(d,e,1,k,bv,ms);
    if(k<=0){er_("no station to rank");wait_();return OK;}
    cls();
    pds(d,uid);
    if(w==1)sprintf(BUF,"from room %s  %s",d->s[id-1].nm,d->s[id-1].c);
    else sprintf(BUF,"from position %.1f , %.1f",x,y);
    in_(BUF);
    sprintf(BUF,"nearest is %.0f m away , computed in %.1f ms",bv,ms);
    in_(BUF);
    wait_();
    return OK;
}
int f3(DB*d){  // menu 3, browse
    for(;;){
        cls();
        pst(d,0);
        ii s=gsid(d,"open which station");
        if(!s)return OK;
        cls();
        St*t=fst(d,s);
        sprintf(BUF,"%s  %s",t->c,t->nm);
        ttl(BUF);
        sprintf(BUF,"faculty  : %s\ncapacity : %d per slot\nx , y    : %.1f , %.1f\nstatus   : %s",  // the room card
            t->fc,t->cap,t->x,t->y,t->ac?"open":"closed");
        bx(t->nm,BUF);
        psl(d,s);
        wait_();
    }
}
int f4(DB*d){  // menu 4, statistic
    static Ana a;
    cls();
    if(d->nf<3){wn_("not enough ratings collected yet");wait_();return OK;}  // need 3 rating
    spn("normalising rater bias",10);
    nrm(d);
    spn("crunching numbers",12);
    if(anz(d,&a)!=OK){er_("analysis failed");wait_();return OK;}
    cls();
    sprintf(BUF,"%d rating(s) from %d visitor(s) over %d station(s)",d->nf,d->nu,d->ns);
    ttl("STATISTICS");
    in_(BUF);
    pss(&a);
    ptb(&a);
    wait_();
    return OK;
}
int run(DB*d){  // main menu loop
    ii uid=0;
    if(AD)return adm(d);  // admin mode
    static const char*o[]={
        "plan my visit",
        "find nearest station",
        "browse stations",
        "statistics",
        "sign in / switch visitor"};
    for(;;){
        cls();
        ban();
        if(uid){
            Us*u=fus(d,uid);
            sprintf(BUF,"%s%s %s %s#%d%s",c(K3),u?u->nm:"?",DOT,c(K2),(int)uid,c(R));
        }else sprintf(BUF,"%snot signed in%s",c(K3),c(R));
        nl(1);
        ctr(BUF);
        int k=mnu("main menu",o,5);
        if(!k){if(gy("leave the program"))return OK;continue;}  // 0 = quit
        if(k==5){uid=who(d);wait_();continue;}  // sign in
        if(k==3){f3(d);continue;}
        if(k==4){f4(d);continue;}
        if(!uid){  // name needed first
            wn_("sign in first");
            uid=who(d);
            if(!uid){wait_();continue;}
        }
        if(k==1)f1(d,uid);
        else f2(d,uid);
    }
}
int boot(DB*d,int ac,char**av){  // start up
    rep(i,1,ac-1){
        if(!strcmp(av[i],"--plain"))thm(0);  // no colours
        else if(!strcmp(av[i],"--admin"))AD=1;  // admin terminal
        else if(!strcmp(av[i],"--help")){
            printf("usage: %s [--admin] [--plain]\n",av[0]);
            return 1;
        }
    }
    dbi(d);  // clear memory
    cls();
    ban();
    nl(1);
    if(bload(d)==OK){  // read oh.dat
        sprintf(BUF,"loaded %s , %d station(s) , %d open time(s)",ADF,d->ns,d->nl);
        ok_(BUF);
    }else{
        lod(d);  // no file, use built in
        sprintf(BUF,"no %s yet , using the built in rooms",ADF);
        wn_(BUF);
    }
    rep(i,1,d->ns){pos[i].x=d->s[i-1].x;pos[i].y=d->s[i-1].y;}  // copy x y for path.h
    build(d->ns);  // make distance table
    slp(500);  // let the user read
    return 0;
}
int bye(DB*d){  // last screen
    (void)d;
    nl(1);
    hrl('=');  // thanks screen
    sprintf(BUF,"%s%sthank you for visiting KVIS%s",c(B),c(K2),c(R));
    ctr(BUF);
    hrl('=');
    nl(1);
    return OK;
}
