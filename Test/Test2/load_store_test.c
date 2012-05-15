/**
   \file
   \author lcs08
   \brief Test funzioni lista eventi
   
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include "CUnit/TestDB.h"
#include "CUnit/Basic.h"
#include "load_store_suite.h"




int main(int argc, char* argv[])
{
    CU_BasicRunMode mode = CU_BRM_VERBOSE;
    unsigned int ret;

    if (CU_initialize_registry()) 
	printf("\nInitialization of Test Registry failed.");
    
    add_suite_loadstoreagenda();
    
    CU_basic_set_mode(mode);
    printf("\nTests completed with return value %d.\n", CU_basic_run_tests());
    printf("\nTests failed %d.\n", ret = CU_get_number_of_tests_failed() );
    CU_cleanup_registry();

    if ( ret == 0 ) 
	return EXIT_SUCCESS;
    else
	return EXIT_FAILURE;

}
