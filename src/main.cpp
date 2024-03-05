#include <ctime>
#include <string>

#include "skiplist.h"

int main(){
    SkipList<int,int>skiplist;
    skiplist.insert(1,1);
    skiplist.insert(2,2);
    skiplist.insert(3,3);
    skiplist.insert(4,4);
    skiplist.insert(5,5);
    skiplist.insert(1,6);
    std::cout<<"Test Debug!"<<std::endl;
    std::cout<<skiplist.find(1)->value<<std::endl;
    //skiplist.display();
}