#include "front.h"

static DB d;

int main(int ac,char**av){  // start, run, end
    if(boot(&d,ac,av))return 0;  // --help stops here
    run(&d);
    bye(&d);
    return 0;
}
