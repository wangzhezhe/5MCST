
#include <iostream>
#include <vector>
#include <unistd.h>
#include <memory>

struct DataArray{
    int data [10];
    DataArray(){
        std::cout<<"construct dataArray"<<std::endl;
        for (int i=0;i<10;i++){
            data[i]=i;
        }
    }
    ~DataArray(){std::cout<<"destroy dataArray"<<std::endl;}
};

struct foo
{
  std::string name;
  std::shared_ptr<int> data; 
  ~foo(){std::cout<<"destroy foo"<<std::endl;}
};

DataArray test()
{
    DataArray d = DataArray();
    foo f =foo();
    f.data.reset(d.data);
    return d;   
}


int main()
{
    DataArray da = test();

    for(int i=0;i<10;i++){
        std::cout<<"value " << da.data[i]<<std::endl;
    }
    std::cout << "main finish" << std::endl;

    return 0;
}