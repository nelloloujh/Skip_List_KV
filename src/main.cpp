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
    skiplist.insert(6,6);
    skiplist.insert(7,7);
    skiplist.insert(8,8);
    std::cout<<"Test find!"<<std::endl;
    std::cout<<skiplist.find(1)->value<<std::endl;

   

    std::cout<<"Test erase!"<<std::endl;
    skiplist.erase(1);
    //std::cout<<"Test []"<<std::endl;
    //std::cout<<skiplist[1]<<std::endl;
    //std::cout<<skiplist.count(1)<<std::endl;

    std::cout<<"Test display!"<<std::endl;
    skiplist.display();

    std::cout<<"Test dump!"<<std::endl;
    skiplist.dump();

    // std::cout<<"Test load!"<<std::endl; 
    // skiplist.load();
    //skiplist.display();
}