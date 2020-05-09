#include "ZT_Api.hpp"

#define RESULTS_DEBUG

int main(int argc, char *argv[]) {

    uint32_t DATA_SIZE = 128;
    // ORAM BLOCK LEGTH
    // ORAM SIZE = DATA_SIZE * BLOCK_LENGTH
    uint32_t BLOCK_LENGTH = 1000;

    myZT zt = myZT(DATA_SIZE, BLOCK_LENGTH);

    // initialize an oram instance
    uint32_t instance_id = zt.myZT_New();

    //prepare a sample request
    unsigned char * tag_in, *tag_out, *tag_null;
    unsigned char * data_in;
    unsigned char * data_out;
    unsigned char * data_null;


    tag_in = (unsigned char*) malloc (TAG_SIZE);
    tag_out = (unsigned char*) malloc (TAG_SIZE);
    tag_null = (unsigned char*) malloc (TAG_SIZE);
    data_in = (unsigned char*) malloc (DATA_SIZE);
    data_out = (unsigned char*) malloc (DATA_SIZE);
    data_null = (unsigned char*) malloc (DATA_SIZE);

    strcpy((char *)data_in, "Hello World");

    // write
    printf("Requests Write Start\n");	

    zt.myZT_Access(instance_id, 'w', tag_in, tag_out, data_in, data_out);

    printf("Requests Write Fin\n");

    // read
    printf("Requests Read Start\n");

    zt.myZT_Access(instance_id, 'r', tag_out, tag_null, data_null, data_out);

    printf("Requests Read Fin\n");

    #ifdef RESULTS_DEBUG
          printf("datasize = %d, Fetched Data :", DATA_SIZE);
          for(uint32_t j=0; j < DATA_SIZE;j++){
        printf("%c", data_out[j]);
          }
          printf("\n");
    #endif

    free(tag_in);
    free(tag_out);
    free(tag_null);
    free(data_in);
    free(data_out);
    free(data_null);
}


