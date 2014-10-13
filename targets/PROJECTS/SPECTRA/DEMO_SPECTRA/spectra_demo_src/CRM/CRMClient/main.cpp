#include "CRMClient.hpp"

int main(int argc,char *argv[])
{
    char * policiesurl = (char*) malloc( 100 * sizeof(char) ); 
    strcpy(policiesurl,"http://1.gae-spectra.appspot.com/policies");
    
//     strcpy(policiesurl,"http://localhost:8888/policies");
    char * measurementsurl = (char*) malloc( 100* sizeof(char) ); 
    strcpy(measurementsurl,"http://1.gae-spectra.appspot.com/measurements");
//     strcpy(measurementsurl,"http://localhost:8888/measurements");
    char * decisionsurl = (char*) malloc( 100 * sizeof(char)); 
    strcpy(decisionsurl,"http://1.gae-spectra.appspot.com/decisions");
    int c;
    
    CRMClient client (policiesurl);
    client.RequestJSONValueAsync().wait();


    return 0;
}