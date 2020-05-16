#include "../Globals.hpp"
#include "../CONFIG.h"
#include "../CONFIG_FLAGS.h"
#include "utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstdint>
#include <random>
#include "ZT.hpp"
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/obj_mac.h>

class myZT{
    public:
        EC_KEY *ENCLAVE_PUBLIC_KEY = NULL;
        unsigned char *enclave_public_key;


        uint32_t DATA_SIZE;
        uint32_t MAX_BLOCKS;

        uint32_t STASH_SIZE;
        uint32_t OBLIVIOUS_FLAG;
        uint32_t RECURSION_DATA_SIZE;
        uint32_t ORAM_TYPE;

        unsigned char *encrypted_request,  *encrypted_response;
        uint32_t request_size, response_size;
        uint32_t bulk_batch_size;
        std::string log_file;

        clock_t generate_request_start, generate_request_stop, extract_response_start, extract_response_stop, process_request_start, process_request_stop, generate_request_time, extract_response_time,  process_request_time;
        uint8_t Z;
        FILE *iquery_file;

        myZT(uint32_t data_size, uint32_t block_size);
        uint32_t myZT_New();
        void myZT_Access(uint32_t instance_id, uint32_t block_id, char op_type, unsigned char * tag_in, unsigned char * tag_out, unsigned char * data_in, unsigned char * data_out);
        void myZT_Bulk_Access(uint32_t instance_id, uint32_t* block_list, char op_type, uint32_t batch_size, unsigned char * tag_in, unsigned char * tag_out, unsigned char * data_in, unsigned char * data_out);


};

myZT::myZT(uint32_t data_size, uint32_t block_size){
  // params
  DATA_SIZE = data_size;
  MAX_BLOCKS = block_size;
  STASH_SIZE = 150;
  OBLIVIOUS_FLAG = 1;
  RECURSION_DATA_SIZE = 64;
  //path
  ORAM_TYPE = 0;
  bulk_batch_size = 0;
  Z = 4;

  // Variables for Enclave Public Key retrieval 
  uint32_t max_buff_size = PRIME256V1_KEY_SIZE;
  unsigned char bin_x[PRIME256V1_KEY_SIZE], bin_y[PRIME256V1_KEY_SIZE], signature_r[PRIME256V1_KEY_SIZE], signature_s[PRIME256V1_KEY_SIZE];
  
  ZT_Initialize(bin_x, bin_y, signature_r, signature_s, max_buff_size);
  
  EC_GROUP *curve;
  EC_KEY *enclave_verification_key = NULL;
  ECDSA_SIG *sig_enclave = ECDSA_SIG_new();	
  BIGNUM *x, *y, *xh, *yh, *sig_r, *sig_s;
  BN_CTX *bn_ctx = BN_CTX_new();
  int ret;

  if(NULL == (curve = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1)))
	  printf("Setting EC_GROUP failed \n");

  EC_POINT *pub_point = EC_POINT_new(curve);
  //Verify the Enclave Public Key
  enclave_verification_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  xh = BN_bin2bn(hardcoded_verification_key_x, PRIME256V1_KEY_SIZE, NULL);
  yh = BN_bin2bn(hardcoded_verification_key_y, PRIME256V1_KEY_SIZE, NULL);
  EC_KEY_set_public_key_affine_coordinates(enclave_verification_key, xh, yh);
  unsigned char *serialized_public_key = (unsigned char*) malloc (PRIME256V1_KEY_SIZE*2);
  memcpy(serialized_public_key, bin_x, PRIME256V1_KEY_SIZE);
  memcpy(serialized_public_key + PRIME256V1_KEY_SIZE, bin_y, PRIME256V1_KEY_SIZE);
	  
  sig_enclave->r = BN_bin2bn(signature_r, PRIME256V1_KEY_SIZE, NULL);
  sig_enclave->s = BN_bin2bn(signature_s, PRIME256V1_KEY_SIZE, NULL);	
  
  ret = ECDSA_do_verify((const unsigned char*) serialized_public_key, PRIME256V1_KEY_SIZE*2, sig_enclave, enclave_verification_key);
  if(ret==1){
	  printf("GetEnclavePublishedKey : Verification Successful! \n");
  }
  else{
	  printf("GetEnclavePublishedKey : Verification FAILED! \n");
  }
  
  //Load the Enclave Public Key
  ENCLAVE_PUBLIC_KEY = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  
  x = BN_bin2bn(bin_x, PRIME256V1_KEY_SIZE, NULL);
  y = BN_bin2bn(bin_y, PRIME256V1_KEY_SIZE, NULL);
  if(EC_POINT_set_affine_coordinates_GFp(curve, pub_point, x, y, bn_ctx)==0)
	  printf("EC_POINT_set_affine_coordinates FAILED \n");

  if(EC_KEY_set_public_key(ENCLAVE_PUBLIC_KEY, pub_point)==0)
	  printf("EC_KEY_set_public_key FAILED \n");

  BN_CTX_free(bn_ctx);
  free(serialized_public_key);
}

uint32_t myZT::myZT_New(){
    return ZT_New(MAX_BLOCKS, DATA_SIZE, STASH_SIZE, OBLIVIOUS_FLAG, RECURSION_DATA_SIZE, ORAM_TYPE, Z);
}
 

void myZT::myZT_Access(uint32_t instance_id, uint32_t block_id, char op_type, unsigned char * tag_in, unsigned char * tag_out, unsigned char * data_in, unsigned char * data_out){
    printf("Call Access\n");
    //prepare encrypted request
    uint32_t encrypted_request_size;
    encrypted_request_size = computeCiphertextSize(DATA_SIZE);
    response_size = DATA_SIZE;
    encrypted_request = (unsigned char *) malloc (encrypted_request_size);				
    encrypted_response = (unsigned char *) malloc (response_size);	

    //Prepare Request:
    //request = rs[i]
    printf("Prepare Request\n");
    //generate_request_start = clock();
    encryptRequest(block_id, op_type, data_in, DATA_SIZE, encrypted_request, tag_in, encrypted_request_size);
    //generate_request_stop = clock();		

    //Process Request:
    printf("Process Request\n");
    //process_request_start = clock();		
    ZT_Access(instance_id, ORAM_TYPE, encrypted_request, encrypted_response, tag_in, tag_out, encrypted_request_size, response_size, TAG_SIZE);
    //process_request_stop = clock();				

    //Extract Response:
    printf("Extract Request\n");
    //extract_response_start = clock();
    extractResponse(encrypted_response, tag_out, response_size, data_out);
    //extract_response_stop = clock();

    free(encrypted_request);
    free(encrypted_response);
}

// 
void myZT::myZT_Bulk_Access(uint32_t instance_id, uint32_t* block_list ,char op_type, uint32_t batch_size ,unsigned char * tag_in, unsigned char * tag_out, unsigned char * data_in, unsigned char * data_out){
    printf("Call Access\n");
    //prepare encrypted request
    uint32_t req_counter = 0;	
    uint32_t encrypted_request_size;
    encrypted_request_size = computeBulkRequestsCiphertextSize(batch_size);
    response_size = DATA_SIZE * batch_size;
    encrypted_request = (unsigned char *) malloc (encrypted_request_size);				
    encrypted_response = (unsigned char *) malloc (response_size);	

    //Prepare Request:
    //request = rs[i]
    printf("Prepare Request\n");
	encryptBulkReadRequest(block_list, req_counter, batch_size, encrypted_request, tag_in, encrypted_request_size);

    //Process Request:
    printf("Process Request\n");		
    ZT_Access(instance_id, ORAM_TYPE, encrypted_request, encrypted_response, tag_in, tag_out, encrypted_request_size, response_size, TAG_SIZE);
		

    //Extract Response:
    printf("Extract Request\n");
    extractResponse(encrypted_response, tag_out, response_size, data_out);


    free(encrypted_request);
    free(encrypted_response);
}
