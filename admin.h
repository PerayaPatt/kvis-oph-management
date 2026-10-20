#ifndef ADMIN_H
#define ADMIN_H
#include <stdio.h>
#include <string.h>
#include "data.h"

#define ADF "oh.dat"  // the binary file
#define ADV 1  // file version

int bsave(DB*d){  // write stations to file
    FILE*f=fopen(ADF,"wb");
    if(!f)return XIO;
    int v=ADV,e=0;
    if(fwrite("KVOH",1,4,f)!=4)e=1;  // key word
    if(fwrite(&v,sizeof v,1,f)!=1)e=1;
    if(fwrite(&d->ns,sizeof d->ns,1,f)!=1)e=1;
    if(fwrite(&d->nl,sizeof d->nl,1,f)!=1)e=1;
    if(fwrite(&d->xs,sizeof d->xs,1,f)!=1)e=1;
    if(fwrite(&d->xl,sizeof d->xl,1,f)!=1)e=1;
    if(d->ns&&fwrite(d->s,sizeof(St),(size_t)d->ns,f)!=(size_t)d->ns)e=1;
    if(d->nl&&fwrite(d->l,sizeof(Sl),(size_t)d->nl,f)!=(size_t)d->nl)e=1;
    fclose(f);
    if(e)return XIO;
    d->dt=0;
    return OK;
}
int bload(DB*d){  // read stations from file
    FILE*f=fopen(ADF,"rb");
    if(!f)return XIO;
    char mg[4];
    int v=0,ns=0,nl=0;
    ii xs=0,xl=0;
    int e=0;
    if(fread(mg,1,4,f)!=4||memcmp(mg,"KVOH",4))e=1;  // check key word
    if(!e&&(fread(&v,sizeof v,1,f)!=1||v!=ADV))e=1;  // check version
    if(!e&&(fread(&ns,sizeof ns,1,f)!=1||ns<0||ns>MXS))e=1;
    if(!e&&(fread(&nl,sizeof nl,1,f)!=1||nl<0||nl>MXL))e=1;
    if(!e&&fread(&xs,sizeof xs,1,f)!=1)e=1;
    if(!e&&fread(&xl,sizeof xl,1,f)!=1)e=1;
    if(!e&&ns&&fread(d->s,sizeof(St),(size_t)ns,f)!=(size_t)ns)e=1;
    if(!e&&nl&&fread(d->l,sizeof(Sl),(size_t)nl,f)!=(size_t)nl)e=1;
    fclose(f);
    if(e)return XPAR;
    d->ns=ns;d->nl=nl;d->xs=xs;d->xl=xl;d->dt=0;
    return OK;
}
int ldel(DB*d,ii lid){  // delete one time
    int k=0;
    rep(i,0,d->nl-1)if(d->l[i].id!=lid)d->l[k++]=d->l[i];
    if(k==d->nl)return XNF;
    d->nl=k;d->dt=1;
    return OK;
}
int sdel(DB*d,ii sid){  // delete station and its times
    int k=0;
    rep(i,0,d->ns-1)if(d->s[i].id!=sid)d->s[k++]=d->s[i];
    if(k==d->ns)return XNF;
    d->ns=k;k=0;
    rep(i,0,d->nl-1)if(d->l[i].sid!=sid)d->l[k++]=d->l[i];
    d->nl=k;d->dt=1;
    return OK;
}
void aslot(DB*d,ii sid){  // add or remove times
    for(;;){
        St*s=fst(d,sid);
        if(!s)return;
        cls();
        ttl("OPEN TIMES");
        psl(d,sid);
        static const char*o[]={"add a time","remove a time"};
        int k=mnu("open times",o,2);
        if(!k)return;
        if(k==1){
            mm a=gt("open from"),z=gt("open until");
            int cap=gi("seats for this time",1,999);
            if(asl(d,sid,1,a,z,cap)<=0)er_("end must be after start , or table is full");
            else{ok_("time added");d->dt=1;}
        }else{
            int id=gi("remove which # ( 0 = back )",0,999999);
            if(!id)continue;
            if(ldel(d,(ii)id)!=OK)er_("no such #");
            else ok_("time removed");
        }
        wait_();
    }
}
void aadd(DB*d){  // new station
    char nm[NL],cd[CL],fc[NL];
    cls();
    ttl("NEW STATION");
    gs("room",nm,NL);
    gs("subject code",cd,CL);
    gs("subject area",fc,NL);
    double x=gd("x in metres",0,9999),y=gd("y in metres",0,9999);
    int cap=gi("seats per time",1,999);
    ii id=ast(d,cd,nm,fc,x,y,cap);
    if(id<=0){er_("station table is full");wait_();return;}
    ok_("station added , now give it the open times");
    wait_();
    aslot(d,id);
}
void aedit(DB*d){  // change one station
    cls();
    pst(d,1);
    ii sid=gsid(d,"edit which station");
    if(!sid)return;
    for(;;){
        St*s=fst(d,sid);
        if(!s)return;
        char b[300];
        int nt=0;
        rep(i,0,d->nl-1)if(d->l[i].sid==sid)nt++;
        cls();
        ttl("EDIT STATION");
        snprintf(b,sizeof b,"room        : %s\nsubject code: %s\nsubject area: %s\nx , y       : %.1f , %.1f\nseats       : %d\nstatus      : %s\nopen times  : %d",
            s->nm,s->c,s->fc,s->x,s->y,s->cap,s->ac?"open":"closed",nt);
        bx(s->nm,b);
        static const char*o[]={"room","subject code","subject area","x , y","seats","open / closed","open times"};
        int k=mnu("change what",o,7);
        if(!k)return;
        if(k==1)gs("room",s->nm,NL);
        else if(k==2)gs("subject code",s->c,CL);
        else if(k==3)gs("subject area",s->fc,NL);
        else if(k==4){s->x=gd("x in metres",0,9999);s->y=gd("y in metres",0,9999);}
        else if(k==5)s->cap=gi("seats per time",1,999);
        else if(k==6)s->ac=!s->ac;
        else aslot(d,sid);
        d->dt=1;
    }
}
void arem(DB*d){  // remove station
    cls();
    pst(d,1);
    ii sid=gsid(d,"remove which station");
    if(!sid)return;
    St*s=fst(d,sid);
    if(!s)return;
    char b[120];
    snprintf(b,sizeof b,"remove %s and all its open times",s->nm);
    if(!gy(b))return;
    if(sdel(d,sid)!=OK)er_("not found");
    else ok_("station removed");
    wait_();
}
int adm(DB*d){  // admin menu
    for(;;){
        char b[160];
        cls();
        ban();
        nl(1);
        ctr("A D M I N   T E R M I N A L");
        snprintf(b,sizeof b,"%s  %d station(s) , %d open time(s)  %s  %s",DOT,d->ns,d->nl,DOT,d->dt?"not saved yet":"saved");

        nl(1);
        in_(b);
        static const char*o[]={"list stations","add station","edit station and its open times","remove station","save to file"};
        int k=mnu("admin menu",o,5);
        if(!k){
            if(d->dt&&gy("save the changes before leaving")){  // not saved yet ?
                if(bsave(d)!=OK){er_("cannot write the file");wait_();continue;}
                ok_("saved");
            }
            return OK;
        }
        if(k==1){cls();pst(d,1);wait_();}
        else if(k==2)aadd(d);
        else if(k==3)aedit(d);
        else if(k==4)arem(d);
        else{
            if(bsave(d)!=OK)er_("cannot write the file");
            else{snprintf(b,sizeof b,"saved %d station(s) and %d open time(s) to %s",d->ns,d->nl,ADF);ok_(b);}
            wait_();
        }
    }
}
#endif
