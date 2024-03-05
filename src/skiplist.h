#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>

const std::string STORE_FILE = "store/dumpFile";

std::mutex mtx;

// template <typename T>
// inline bool const& Less(T const& a, T const &b){
//     return a<b;
// }

template <typename T>
struct Less
{
    bool operator()(const T &a, const T &b) const
    {
        return a < b;
    }
};

template <typename K, typename V, typename Comp = Less<K>>
class SkipList
{
private:
    struct Node
    {
        const K key;
        V value;
        Node **next;
        int level;
        Node() : key(0), value(0), level(0), next(nullptr) {}
        Node(const K &k, const V &v, const int level, Node *nxt = nullptr) : key(k), value(v), level(level)
        {
            this->next = new Node *[level + 1];
            for (int i = 0; i <= level; i++)
            {
                this->next[i] = nxt;
            }
        }
        ~Node()
        {
            delete[] next;
        }
    };
    void init();
    Node *find(const K &key, Node **update);
    static constexpr int max_level = 32;
    int skip_list_level;
    int element_count;
    Node *head, *tail;
    Comp comp;
    std::ifstream file_reader;
    std::ofstream file_writer;

public:
    SkipList();
    SkipList(Comp);
    ~SkipList();
    int get_random_level();
    void insert(const K &key, const V &value);
    bool erase(const K &key);
    int count(const K &key);
    void clear();
    int size();
    void display();
    void dump();
    void load();

    struct Iter
    {
        Node *p;
        Iter() : p(nullptr) {}
        Iter(Node *rhs) : p(rhs) {}
        // 避免值的改变
        Node &operator*() const { return *p; }
        Node *operator->() const { return (p); }
        bool operator==(const Iter &rhs) const { return p == rhs.p; }
        bool operator!=(const Iter &rhs) const { return p != rhs.p; }
        void operator++(int) { p = p->next[0]; }
    };
    Iter begin();
    Iter end();
    Iter find(const K &key);
    V &operator[](const K &key);
};

// 初始化skip_list
template <typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::init()
{
    skip_list_level = 0;
    element_count = 0;
    head->next = new Node *[max_level + 1];
    for (int i = 0; i <= max_level; i++)
    {
        head->next[i] = tail;
    }
}
// 定义构造函数
template <typename K, typename V, typename Comp>
SkipList<K, V, Comp>::SkipList() : head(new Node()), tail(new Node()), comp{Comp()}
{
    init();
}
template <typename K, typename V, typename Comp>
SkipList<K, V, Comp>::SkipList(Comp less_extern) : head(new Node()), tail(new Node()), comp{less_extern}
{
    init();
}
// 定义析构函数
template <typename K, typename V, typename Comp>
SkipList<K, V, Comp>::~SkipList()
{
    // clear();
    if (file_reader.is_open())
    {
        file_reader.close();
    }
    if (file_writer.is_open())
    {
        file_writer.close();
    }
    delete head;
    delete tail;
}
// 获取上浮随机值
template <typename K, typename V, typename Comp>
int SkipList<K, V, Comp>::get_random_level()
{
    int level_random;
    for (level_random = 0;; level_random++)
    {
        if (rand() & 1)
        {
            break;
        }
    }
    return level_random < max_level ? level_random : max_level;
}

// find
template <typename K, typename V, typename Comp>
typename SkipList<K, V, Comp>::Node *SkipList<K, V, Comp>::find(const K &key, Node **update)
{
    Node *p = head;
    for (int cur_level = max_level; cur_level >= 0; cur_level--)
    {
        while (p->next[cur_level] != tail && comp(p->next[cur_level]->key, key))
        {
            p = p->next[cur_level];
        }
        if (update != nullptr)
            update[cur_level] = p;
    }
    p = p->next[0];
    return p;
}
// erase
template <typename K, typename V, typename Comp>
bool SkipList<K, V, Comp>::erase(const K &key)
{
    Node *update[max_level + 1];
    Node *p = find(key, update);
    if (p->key != key)
    {
        return false;
    }
    for (int lv = 0; lv <= skip_list_level; lv++)
    {
        if (update[lv]->next[lv] != p)
            break;
        update[lv]->next[lv] = p->next[lv];
    }
    // 清顶
    while (skip_list_level > 0 && head->next[skip_list_level] == tail)
    {
        skip_list_level--;
    }
    delete p;
}
// count
template <typename K, typename V, typename Comp>
int SkipList<K, V, Comp>::count(const K &key)
{
    Node *p = find(key, nullptr);
    if (p->key != key)
    {
        return 0;
    }
    else
        return 1;
}

// del
template <typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::clear()
{
    Node *p = head->next[0];
    while (p != tail)
    {
        Node *tmp = p;
        p = p->next[0];
        delete tmp;
    }
    init();
}
template <typename K, typename V, typename Comp>
int SkipList<K, V, Comp>::size()
{
    return element_count;
}
template <typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::display()
{
    for (int lv = skip_list_level; lv >= 0; lv--)
    {
        Node *p = head->next[lv];
        std::cout << "Level " << lv << ":";
        while (p != tail)
        {
            std::cout << p->key << " ";
            p = p->next[lv];
        }
        std::cout << "\n";
    }
}

// 落盘
#include <fstream>
#include <iostream>
#include <sys/stat.h>

// Function to check if a file exists
bool file_exists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Function to create a file
void create_file(const std::string& filename) {
    std::ofstream file(filename);
    file.close();
}

template <typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::dump()
{
    if (!file_exists(STORE_FILE))
    {
        std::cout << "文件不存在，创建文件" << std::endl;
        if(file_exists(STORE_FILE)){
            std::cout << "文件创建成功" << std::endl;
        }else{
            std::cout << "文件创建失败" << std::endl;
        }
    }
    std::ofstream file_writer(STORE_FILE, std::ofstream::out | std::ofstream::trunc);

    for (typename SkipList<K, V, Comp>::Iter node = begin(); node != end(); node++)
    {
        file_writer << node->key << " " << node->value << std::endl;
        std::cout << node->key << " " << node->value << std::endl;
    }
    file_writer.flush();
    file_writer.close();
    return;
}

// 读盘
template <typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::load()
{
    file_reader.open(STORE_FILE, std::ifstream::in);
    if (!file_reader.is_open())
    {
        std::cout << "打开失败\n";
        return;
    }
    K key;
    V value;
    while (file_reader >> key >> value)
    {
        std::cout << key << " " << value << std::endl;
        insert(key, value);
    }
    file_reader.close();
    return;
}
template <typename K, typename V, typename Comp>
void SkipList<K, V, Comp>::insert(const K &key, const V &value)
{
    Node *update[max_level + 1];
    Node *p = find(key, update);
    if (p->key == key)
    {
        p->value = value;
        return;
    }
    // 没找到，说明要插入全新节点
    int new_level = get_random_level();

    // 上升点高于目前最高层
    if (new_level > skip_list_level)
    {
        new_level = ++skip_list_level;
        update[new_level] = head;
    }
    Node *newnode = new Node(key, value, new_level);
    for (int i = new_level; i >= 0; i--)
    {
        p = update[i];
        newnode->next[i] = p->next[i];
        p->next[i] = newnode;
    }
    element_count++;
}

// Iterator 相关操作
template <typename K, typename V, typename Comp>
typename SkipList<K, V, Comp>::Iter SkipList<K, V, Comp>::find(const K &key)
{

    Node *p = find(key, nullptr);

    if (p == tail)
    {
        return tail;
    }
    if (p->key == key)
    {
        return Iter(p);
    }
    else
    {
        return tail;
    }
}
template <typename K, typename V, typename Comp>
typename SkipList<K, V, Comp>::Iter SkipList<K, V, Comp>::begin()
{
    return Iter(head->next[0]);
}

template <typename K, typename V, typename Comp>
typename SkipList<K, V, Comp>::Iter SkipList<K, V, Comp>::end()
{
    return Iter(tail);
}

template <typename K, typename V, typename Comp>
V &SkipList<K, V, Comp>::operator[](const K &key)
{
    Node *p = find(key, nullptr);
    if (p == tail || p->key != key)
    {
        std::cout << "not found:";
        // p = new Node(key,V());
        insert(key, V());
        p = find(key, nullptr);
    }

    return p->value;
}