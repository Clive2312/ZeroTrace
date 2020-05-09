# ZeroTrace

#### 使用方法

API Sample: 

```shell
./Sample_App/ZT_Api.cpp
```

API:

```c++
// 简化版的 Zerotrace 类
// ORAM 配置信息hard code在代码中，如需更改可到ZT_Api.hpp
class myZT;

// data_size: oram中每个data block的大小
// block size: oram中block 总数
myZT::myZT(uint32_t data_size, uint32_t block_size);

// 新建一个oram tree，返回对应该oram tree 的id
uint32_t myZT_New();

// 访问数据
// instance_id: oram实例
// op_type: 'r' 读；'w' 写
// tag_in: 需要访问的data block的tag
// tag_out: 访问完毕后该data_block所对应的新的tag
// data_in: 写入的数据，若为读操作，data_in可以是任意值
// data_out: 读取得到的数据

void myZT_Access(uint32_t instance_id,
                 char op_type, 
                 unsigned char * tag_in, unsigned char * tag_out, 
                 unsigned char * data_in, unsigned char * data_out);
```



*Sajin Sasy, Sergey Gorbunov, and Christopher Fletcher. "ZeroTrace: Oblivious memory primitives from Intel SGX." Symposium on Network and Distributed System Security (NDSS). 2018.*


