#include <vector>

using namespace std;

const int maxColision = 3;
const float maxFillFactor = 0.8;

template<typename TK, typename TV>
struct ChainHashNode {
    TK key;
    TV value;
    size_t hashcode;
    ChainHashNode* next;

    ChainHashNode(TK key_, TV value_, size_t hashcode_){
       this->key = key_;
       this->value = value_;
       this->hashcode = hashcode_;
       this->next = nullptr;
    }
};

template<typename TK, typename TV>
class ChainHashListIterator {
private:
    ChainHashNode<TK, TV>* current;
public:
    ChainHashListIterator(ChainHashNode<TK, TV>* current_){
       this->current = current_;
    }

    ChainHashListIterator& operator++(){
       if(this->current != nullptr)
           this->current = this->current->next;
       return *this;
    }

    bool operator!=(const ChainHashListIterator<TK, TV>& other) const{
       return this->current != other.current;
    }

    ChainHashNode<TK, TV>& operator*(){
       return *(this->current);
    }
};

template<typename TK, typename TV>
class ChainHash
{
private:
    typedef ChainHashNode<TK, TV> Node;
    typedef ChainHashListIterator<TK, TV> Iterator;

    Node** array;  // array de punteros a Node
    int nsize; // total de elementos <key:value> insertados
    int capacity; // tamanio del array
    int *bucket_sizes; // guarda la cantidad de elementos en cada bucket
    int usedBuckets; // cantidad de buckets ocupados (con al menos un elemento)

public:
    ChainHash(int initialCapacity = 10){
       this->capacity = initialCapacity;
       this->array = new Node*[capacity]();
       this->bucket_sizes = new int[capacity]();
       this->nsize = 0;
       this->usedBuckets = 0;
    }

    TV get(TK key){
       size_t hashcode = getHashCode(key);
       size_t index = hashcode % capacity;

       Node* current = this->array[index];
       while(current != nullptr){
          if(current->key == key) return current->value;
          current = current->next;
       }
       throw std::out_of_range("Key no encontrado");
    }

    int size(){ return this->nsize; }

    int bucket_count(){ return this->capacity; }

    int bucket_size(int index) {
       if(index < 0 || index >= this->capacity) throw std::out_of_range("Indice de bucket invalido");
       return this->bucket_sizes[index];
    }

    void set(TK key, TV value) {
       if(fillFactor() > maxFillFactor) rehashing();

       size_t hashcode = getHashCode(key);
       int index = hashcode % this->capacity;

       Node* current = this->array[index];
       while(current != nullptr){
          if(current->key == key){
             current->value = value;
             return;
          }
          current = current->next;
       }

       Node* newNode = new Node(key, value, hashcode);
       newNode->next = this->array[index];
       this->array[index] = newNode;

       this->bucket_sizes[index]++;
       if(this->bucket_sizes[index] == 1){
          this->usedBuckets++;
       }
       this->nsize++;
    }

    bool remove(TK key){
       size_t hashcode = getHashCode(key);
       int index = hashcode % this->capacity;

       Node* current = this->array[index];
       Node* prev = nullptr;

       while(current != nullptr){
          if(current->key == key){
             if(prev == nullptr){
                this->array[index] = current->next;
             } else {
                prev->next = current->next;
             }
             delete current;
             this->bucket_sizes[index]--;
             if(this->bucket_sizes[index] == 0){
                this->usedBuckets--;
             }
             this->nsize--;
             return true;
          }
          prev = current;
          current = current->next;
       }
       return false;
    }

    bool contains(TK key){
       try {
          get(key);
          return true;
       } catch(const std::out_of_range&){
          return false;
       }
    }

    Iterator begin(int index){
       if(index < 0 || index >= this->capacity)
          throw std::out_of_range("Indice de bucket invalido");
       return Iterator(this->array[index]);
    }

    Iterator end(int index){
       return Iterator(nullptr);
    }

private:
    double fillFactor(){
       return (double)this->usedBuckets / (double)this->capacity;
    }

    size_t getHashCode(TK key){
       std::hash<TK> ptr_hash;
       return ptr_hash(key);
    }

    void rehashing(){
       Node** oldArray = array;
       int* oldBucketSizes = bucket_sizes;
       int oldCapacity = capacity;

       this->capacity = capacity * 2;
       this->array = new Node*[capacity]();
       this->bucket_sizes = new int[this->capacity]();
       this->nsize = 0;
       this->usedBuckets = 0;

       for (int i = 0; i < oldCapacity; i++){
          Node* current = oldArray[i];
          while (current != nullptr){
             Node* next = current->next;

             int new_index = current->hashcode % this->capacity;

             current->next = this->array[new_index];
             this->array[new_index] = current;

             this->bucket_sizes[new_index]++;
             if (this->bucket_sizes[new_index] == 1) {
                this->usedBuckets++;
             }
             current = next;
          }
       }

       delete[] oldArray;
       delete[] oldBucketSizes;
    }

public:
    ~ChainHash(){
       for(int i = 0; i < capacity; i++){
          Node* current = array[i];
          while(current != nullptr){
             Node* next = current->next;
             delete current;
             current = next;
          }
       }

       delete[] array;
       delete[] bucket_sizes;
    }
};

template<>
void ChainHash<string, vector<int>>::set(string key, vector<int> value) {
    if (fillFactor() > maxFillFactor) rehashing();

    size_t hashcode = getHashCode(key);
    int index = hashcode % this->capacity;

    Node* current = this->array[index];
    while (current != nullptr) {
        if (current->key == key) {
            current->value.push_back(value[0]);  
            return;
        }
        current = current->next;
    }

    Node* newNode = new Node(key, value, hashcode);
    newNode->next = this->array[index];
    this->array[index] = newNode;

    this->bucket_sizes[index]++;
    if (this->bucket_sizes[index] == 1) {
        this->usedBuckets++;
    }
    this->nsize++;
}
