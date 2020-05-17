#include "ZT_Api.hpp"
// #include <vector>

// using namespace std;

// void DumpToZT(int wordID, const vector<int> &v);
// vector<int> LoadFromZT(int wordID);

#define RESULTS_DEBUG

/*
* BLOCK_SIZE 1024
* STRUCTURE inode info in meta_instance
* BLOCK content store in data instance
* meta structure: 
* inode: length block_id | 4 | 4 | ... 
* 
*/

class Controller{
  public:

    uint32_t data_instance;
    uint32_t meta_instance;
    uint32_t BLOCK_SIZE;
    uint32_t BLOCK_LENGTH;

    uint32_t data_counter;
    uint32_t meta_counter;

    unsigned char * tag_in, *tag_out, *data_in, *data_out;
    myZT zt;

    Controller();
    ~Controller();
    // return block_id
    uint32_t DumpToZT(unsigned char * addr, uint32_t data_length);
    // return length
    unsigned char * LoadFromZT(uint32_t block_id, uint32_t & data_length);

};

Controller::Controller(){

    data_counter = 0;

    meta_counter = 0;
    
    BLOCK_LENGTH = 1000;

    BLOCK_SIZE = 1024;

    zt = myZT(BLOCK_SIZE, BLOCK_LENGTH);

    meta_instance = zt.myZT_New();

    data_instance = zt.myZT_New();

    tag_in = (unsigned char*) malloc (TAG_SIZE);
    tag_out = (unsigned char*) malloc (TAG_SIZE);
    data_in = (unsigned char*) malloc (BLOCK_SIZE);
    data_out = (unsigned char*) malloc (BLOCK_SIZE);

}

Controller::~Controller(){
  free(tag_in);
  free(tag_out);
  free(data_in);
  free(data_out);
}

uint32_t Controller::DumpToZT(unsigned char * addr, uint32_t data_length){
  int block_count = (data_length + BLOCK_SIZE - 1)/ BLOCK_SIZE;
  int meta[block_count + 2];
  meta[0] = block_count;
  meta[1] = data_length;
  // write data block
  for(int i = 0; i < block_count; i++){
    memcpy(data_in, addr + i*BLOCK_SIZE, BLOCK_SIZE);
    zt.myZT_Access(data_instance, data_counter + i, 'w', tag_in, tag_out, data_in, data_out);
    meta[i + 2] = data_counter + i;
  }

  //write meta block
  memcpy(data_in, meta, sizeof(meta));
  zt.myZT_Access(meta_instance, meta_counter, 'w', tag_in, tag_out, data_in, data_out);

  // update Controller info
  meta_counter += 1;
  data_counter += block_count;

  return meta_counter - 1;
}

unsigned char * Controller::LoadFromZT(uint32_t block_id, uint32_t & data_length){
  // read from meta
  zt.myZT_Access(meta_instance, block_id, 'r', tag_in, tag_out, data_in , data_out);
  uint32_t* meta = (uint32_t *)data_out;
  int block_length = meta[0];
  data_length = meta[1];

  // space allocation
  unsigned char * data = (unsigned char *)malloc(data_length);
  unsigned char * bulk_data_out = (unsigned char *)malloc(block_length * BLOCK_SIZE);

  // bulk read block
  zt.myZT_Bulk_Access(data_instance, meta + 2, block_length, tag_in, tag_out, data_in, bulk_data_out);
  memcpy(data, bulk_data_out, data_length);
  free(bulk_data_out);
  return data;
}

int main(int argc, char *argv[]){

  uint32_t BLOCK_SIZE = 1024;

  uint32_t BLOCK_LEN = 10;

  // controller testing
  Controller ct = Controller();


  unsigned char * chunk = (unsigned char *)malloc(BLOCK_LEN * BLOCK_SIZE);

  for(int i = 0; i < BLOCK_LEN; i++){
    strcpy((char *)chunk + i * BLOCK_SIZE, "Hello World233233233!");
  }

  uint32_t id = ct.DumpToZT(chunk, BLOCK_LEN * BLOCK_SIZE);

  uint32_t length_out = 0;

  unsigned char * chunk_out = ct.LoadFromZT(id, length_out);

  #ifdef RESULTS_DEBUG
    printf("datasize = %d, Data out:", length_out);
    for(uint32_t j=0; j < BLOCK_LEN*BLOCK_SIZE;j++){
      printf("%c", chunk_out[j]);
    }
    printf("\n");
  #endif
  
  free(chunk);
  free(chunk_out);
  return 0;
}
 

// int main(int argc, char *argv[]) {

//     uint32_t DATA_SIZE = 128;
//     // ORAM BLOCK LEGTH
//     // ORAM SIZE = DATA_SIZE * BLOCK_LENGTH
//     uint32_t BLOCK_LENGTH = 1000;

//     myZT zt = myZT(DATA_SIZE, BLOCK_LENGTH);

//     // initialize an oram instance
//     uint32_t instance_id = zt.myZT_New();

//     //prepare a sample request
//     unsigned char * tag_in, *tag_out, *tag_null;
//     unsigned char * data_in_1;
//     unsigned char * data_in_2;
//     unsigned char * data_out;
//     unsigned char * data_null;
    



//     tag_null = (unsigned char*) malloc (TAG_SIZE);
//     data_in_1 = (unsigned char*) malloc (DATA_SIZE);
//     data_in_2 = (unsigned char*) malloc (DATA_SIZE);
//     data_out = (unsigned char*) malloc (DATA_SIZE);
//     data_null = (unsigned char*) malloc (DATA_SIZE);
    

//     strcpy((char *)data_in_1, "Hello World 0001");

//     strcpy((char *)data_in_2, "Hello World 0002");

//     // write
//     printf("Requests Write Start\n");	

//     zt.myZT_Access(instance_id, 1,'w', tag_in, tag_out, data_in_1, data_out);

//     zt.myZT_Access(instance_id, 2,'w', tag_in, tag_out, data_in_2, data_out);

//     printf("Requests Write Fin\n");

//     // read
//     printf("Requests Read Start\n");

//     zt.myZT_Access(instance_id, 1,'r', tag_out, tag_null, data_null, data_out);

//     printf("Requests Read Fin\n");

//     #ifdef RESULTS_DEBUG
//           printf("datasize = %d, Fetched Data 1:", DATA_SIZE);
//           for(uint32_t j=0; j < DATA_SIZE;j++){
//         printf("%c", data_out[j]);
//           }
//           printf("\n");
//     #endif

//     zt.myZT_Access(instance_id, 2,'r', tag_out, tag_null, data_null, data_out);

//     printf("Requests Read Fin\n");

//     #ifdef RESULTS_DEBUG
//           printf("datasize = %d, Fetched Data 2:", DATA_SIZE);
//           for(uint32_t j=0; j < DATA_SIZE;j++){
//         printf("%c", data_out[j]);
//           }
//           printf("\n");
//     #endif

//     uint32_t batch_size = 2;
//     unsigned char * data_bulk;
//     data_bulk = (unsigned char*) malloc (batch_size * DATA_SIZE);
//     uint32_t* req_list = (uint32_t *) malloc( batch_size * sizeof(uint32_t) );

//     req_list[0] = 1;
//     req_list[1] = 2;

//     printf("Requests Bulk Read Start\n");

//     zt.myZT_Bulk_Access(instance_id, req_list, batch_size, tag_out, tag_null, data_bulk, data_bulk);

//     printf("Requests Bulk Read Fin\n");

//     #ifdef RESULTS_DEBUG
//           printf("datasize = %d, Fetched Data 2:", batch_size * DATA_SIZE);
//           for(uint32_t j=0; j < batch_size * DATA_SIZE;j++){
//         printf("%c", data_bulk[j]);
//           }
//           printf("\n");
//     #endif

//     free(tag_in);
//     free(tag_out);
//     free(tag_null);
//     free(data_in_1);
//     free(data_in_2);
//     free(data_out);
//     free(data_bulk);
//     free(data_null);
//     free(req_list);
// }



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


