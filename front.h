#ifndef FRONT_H
#define FRONT_H
#include "data.h"

#define VER "1.0"  // version
#define SUB "Kamnoetvidya Science Academy"  // school name
#define W 72  // screen width

#define R  "\x1b[0m"  // back to normal
#define B  "\x1b[1m"  // bold
#define K1 "\x1b[38;5;33m"  // blue
#define K2 "\x1b[38;5;208m"  // orange
#define K3 "\x1b[38;5;245m"  // grey
#define KG "\x1b[38;5;40m"  // green
#define KR "\x1b[38;5;196m"  // red
#define KY "\x1b[38;5;220m"  // yellow

#define TL "\u250c"  // box pieces
#define TR "\u2510"
#define BL "\u2514"
#define BR "\u2518"
#define HZ "\u2500"
#define VT "\u2502"
#define LT "\u251c"
#define RT "\u2524"
#define DOT "\u2022"  // dot
#define ARW "\u203a"  // arrow
#define STR "\u2605"  // star

void cls(void);  // clear screen
void ban(void);  // big logo
void ttl(const char*);  // section title
void hrl(char);  // line across
void bx(const char*,const char*);  // box with title
void nl(int);  // empty lines
void ctr(const char*);  // centre text
void pad(const char*,int);  // pad to width

int  mnu(const char*,const char**,int);  // menu
int  gi(const char*,int,int);  // ask number
double gd(const char*,double,double);  // ask decimal
void gs(const char*,char*,int);  // ask text
mm   gt(const char*);  // ask time
int  gy(const char*);  // ask yes no
int  gsid(DB*,const char*);  // ask station
int  gcoord(const char*,double*,double*);  // ask x y

void msg(const char*,const char*);  // coloured line
void ok_(const char*);  // green
void er_(const char*);  // red
void wn_(const char*);  // yellow
void in_(const char*);  // blue
void hlp(const char*);  // grey hint
void wait_(void);  // press enter
void spn(const char*,int);  // spinner
void bar(double,int);  // bar
void tick(int,int,const char*);  // step counter

void tbh(const char**,const int*,int);  // table head
void tbr(const char**,const int*,int);  // table row
void tbf(const int*,int);  // table bottom

void pst(DB*,int);  // stations
void psl(DB*,ii);  // open times
void ppl(DB*,ii);  // the 3 plans
void ppi(DB*,ii);  // stops of a plan
void pds(DB*,ii);  // nearest list
void pss(const Ana*);  // station stats
void ptb(const Ana*);  // demand bars

int f1(DB*,ii);  // plan visit
int f2(DB*,ii);  // nearest station
int f3(DB*);  // browse
int f4(DB*);  // statistics
ii  who(DB*);  // sign in
int run(DB*);  // main loop
int boot(DB*,int,char**);  // start up
int bye(DB*);  // last screen

void thm(int);  // colour on off

#endif
