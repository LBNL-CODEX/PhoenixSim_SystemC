

//#include "electronic_router_test.h"
#include "sstmac-port/sstmac-port-config.h"
#ifdef USING_SSTMAC
#include "sstmac-port/electronic_torus_sstmac_test.h"
#else
#include "electronic_torus_test.h"
#endif
int sc_main (int argc, char * argv[]) 
{
	//run_torus();
	//run_router();

#ifdef USING_SSTMAC
	sstmac_run_torus();
#else
	run_torus();
#endif
	return 0;
}
