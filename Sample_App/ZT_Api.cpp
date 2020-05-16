#include "ZT_Api.hpp"
// #include <vector>

// using namespace std;

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
    unsigned char * data_in_1;
    unsigned char * data_in_2;
    unsigned char * data_out;
    unsigned char * data_null;


    tag_in = (unsigned char*) malloc (TAG_SIZE);
    tag_out = (unsigned char*) malloc (TAG_SIZE);
    tag_null = (unsigned char*) malloc (TAG_SIZE);
    data_in_1 = (unsigned char*) malloc (DATA_SIZE);
    data_in_2 = (unsigned char*) malloc (DATA_SIZE);
    data_out = (unsigned char*) malloc (DATA_SIZE);
    data_null = (unsigned char*) malloc (DATA_SIZE);

    strcpy((char *)data_in_1, "Hello World 0001");

    strcpy((char *)data_in_2, "Hello World 0002");

    // write
    printf("Requests Write Start\n");	

    zt.myZT_Access(instance_id, 1,'w', tag_in, tag_out, data_in_1, data_out);

    zt.myZT_Access(instance_id, 2,'w', tag_in, tag_out, data_in_2, data_out);

    printf("Requests Write Fin\n");

    // read
    printf("Requests Read Start\n");

    zt.myZT_Access(instance_id, 1,'r', tag_out, tag_null, data_null, data_out);

    printf("Requests Read Fin\n");

    #ifdef RESULTS_DEBUG
          printf("datasize = %d, Fetched Data 1:", DATA_SIZE);
          for(uint32_t j=0; j < DATA_SIZE;j++){
        printf("%c", data_out[j]);
          }
          printf("\n");
    #endif

    zt.myZT_Access(instance_id, 2,'r', tag_out, tag_null, data_null, data_out);

    printf("Requests Read Fin\n");

    #ifdef RESULTS_DEBUG
          printf("datasize = %d, Fetched Data 2:", DATA_SIZE);
          for(uint32_t j=0; j < DATA_SIZE;j++){
        printf("%c", data_out[j]);
          }
          printf("\n");
    #endif

    free(tag_in);
    free(tag_out);
    free(tag_null);
    free(data_in_1);
    free(data_in_2);
    free(data_out);
    free(data_null);
}



// inode system
// class myZT_vector{
//   public:
//     int ELE_SIZE;
//     myZT zt;
//     uint32_t DATA_SIZE;
//     uint32_t BLOCK_LENGTH;
//     uint32_t instance_id;  

//     myZT_vector(int size);

//     vector<int> get(usigned char * tag_in, unsigned char * tag_out);
//     void put(unsigned char * tag_out, vector<int> docList);

  
// };

// myZT_vector::myZT_vector(int size){
//   ELE_SIZE = size;
//   DATA_SIZE = 128;
//   BLOCK_LENGTH = 1000;
//   zt = myZT(DATA_SIZE, BLOCK_LENGTH);
//   instance_id = zt.myZT_New();
// }

// vector<int> myZT_vector::get(usigned char * tag_in, unsigned char * tag_out){
//   unsigned char * data_inode = (unsigned char*) malloc (DATA_SIZE);
//   unsigned char * data_temp = (unsigned char*) malloc (DATA_SIZE);

//   unsigned char * tag_inode = (unsigned char*) malloc (TAG_SIZE);

//   vector<int> result;

//   // get inode block
//   zt.myZT_Access(instance_id, 'r', tag_in, tag_inode, data_temp, data_inode);
//   vector<unsigned char *> tag_list = transfer(data_inode);

//   for(int i = 0; i < tag_list.size(); i++){
//     unsigned char * data_block = (unsigned char*) malloc (DATA_SIZE);
//     zt.myZT_Access(instance_id, 'r', tag_list[i], tag_list[i], data_temp, data_block);
//     vector<int> doc_list = transfer(data_block);
//     result.insert(result.end(), doc_list.begin(), doc_list.end());
//   }
//   data_temp = re_transfer(taglist)

//   zt.myZT_Access(instance_id, 'w', tag_inode, tag_out, data_temp, data_temp);
//   return result;
// }

// void myZT_vector::put(unsigned char * tag_out, vector<int> docList){

// }


