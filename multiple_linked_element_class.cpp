#pragma once
#include<iostream>
using std::cout; using std::endl; using std::boolalpha; using std::ostream;
#include<initializer_list>
using std::initializer_list;
#include <algorithm> 
using std::max; using std::copy; using std::swap; using std::count; using std::copy_if;
#include <utility> 
using std::make_pair; using std::pair; using std::swap;
#include <stdexcept> 
using std::runtime_error; 
#include<iterator>
using std::ostream_iterator;



const static size_t element_array_size = 5;

//////////////////////////////////// Element ////////////////////////////////////
template<typename K, typename V>
struct Element{
public:
  K key_;
  V values_[element_array_size];
  size_t count_ = 0; 

  Element()=default;

  Element(K key, initializer_list<V> values){
    key_ = key;
    count_ = values.size();
    copy(values.begin(), values.end(), values_);
    }

  friend ostream& operator<<(ostream& out, const Element& ele){
    //should be of the form "key:val1,val2,val3"
    out << ele.key_ << ":";

    //copy values without a trailing comma
    copy(ele.values_, ele.values_+ ele.count_ - 1, ostream_iterator<V>(out,","));

    if (ele.count_ != 0) //if the array isn't empty, copy last ele
        out << (*(ele.values_ + ele.count_ -1));

    return out;
  }

    //def below
    bool operator==(const Element&)const;
    bool operator<(const K&) const;
 
};

////////////////////// more element class definitions //////////////////////////

template<typename K, typename V>
bool Element<K,V>::operator==(const Element& ele_to_compare)const{
    //if eles have different val size or key, they're different
    if (count_ != ele_to_compare.count_ || key_ != ele_to_compare.key_) {
        return false;
    }
    
    //check if the every val in the arrays are identical
    for(size_t i = 0; i < count_; i++)
    {
        if (*(values_+ i) != *(ele_to_compare.values_+i)) {
            return false;
        }
    }
    
    //key, size, and values are identical
    return true;
}

template<typename K, typename V>
bool Element<K,V>::operator<(const K& key) const{
    return (key_ < key);
}

///////////////////////////////////// MVM //////////////////////////
template<typename K, typename V> 
class MVM{
public:
  Element<K, V> * data_ = nullptr;
  size_t num_keys_ = 0; 
  size_t num_elements_ = 0; 
  Element<K, V> * find_key(K);
  size_t find_value(V , K*& );
  void grow(); 
    bool add(K,V);  
    bool remove_key(K);
    size_t remove_value(V, K*&);

public:
  MVM()=default;

  MVM(initializer_list<Element<K,V>> ellies){
    num_keys_ = ellies.size();
    num_elements_ = num_keys_ ; 
    data_ = new Element<K,V>[num_elements_];
    copy(ellies.begin(), ellies.end(), data_);
  }

  MVM(const MVM& other){
    num_keys_ = other.num_keys_;
    num_elements_ = other.num_elements_; 
    data_ = new Element<K,V>[num_elements_];
    copy(other.data_, other.data_ + other.num_keys_, data_);
  } 

  ~MVM(){
      //In the example code, the only thing specifically deleted is the array. How does the compiler know to delete all of the other types?
      //Or does it?

    delete[] data_;
  }

  friend ostream& operator<<(ostream& out, MVM& mv){
    //print every element using the Element class's friend function without trailing space
    copy(mv.data_, mv.data_ + mv.num_keys_ - 1, ostream_iterator<Element<K,V>>(out, " "));

    //if the array isn't empty, copy last ele
    if (mv.num_keys_ != 0) 
        out << (*(mv.data_ + mv.num_keys_ -1));

    return out;
  }

    size_t size(){return num_keys_;}
};
 
///////////////// More MVM Definitions ///////////////////////
template<typename K, typename V> 
bool MVM<K,V>::remove_key(K key){
    //key actually doesn't exist
    if((*find_key(key)).key_ != key){
        return false;
    }else{
        //make a new array and copy everything except for the key
        Element<K,V> * temp_ary;
        temp_ary  = new Element<K,V> [num_keys_ - 1]{}; 
        copy_if(data_, data_+num_keys_, temp_ary, [key](Element<K,V> ellie){return (ellie.key_ != key);});
        swap(temp_ary, data_); //then switch
        delete [] temp_ary;
        num_keys_ -= 1; //change count
        return true;
    }

}

template<typename K, typename V> 
size_t MVM<K,V>::remove_value(V value, K*(& keys_found)){
    size_t matching_keys  = find_value(value, keys_found);
    
    if (matching_keys == 0)
        return 0;

    for(size_t i = 0; i < matching_keys; i++)
    {
        Element<K,V>* key_loc = (*this).find_key(*(keys_found + i));

        V * temp_ary;
        temp_ary  = new V [(*key_loc).count_- 1]{}; 
        copy_if((*key_loc).values_, (*key_loc).values_ + (*key_loc).count_, temp_ary, [value](V comp_val){return (comp_val != value);});
        swap(temp_ary, (*key_loc).values_); //then switch
        delete [] temp_ary;
        (*key_loc).count_ -= 1; //change count
        return true;
    }
    
}

template<typename K, typename V> 
Element<K, V> * MVM<K,V>::find_key(K input_key){
    return lower_bound(data_, data_ + num_keys_, input_key);
}

template<typename K, typename V> 
size_t MVM<K,V>::find_value(V search_value, K*& keys_found){

    //keys_found must initially be a nullptr
    if (keys_found != nullptr) 
        throw std::runtime_error("found keys array must be a nullptr");

    bool contains_val = false; //flag for searching for the input value 
    size_t ary_size  = 0;

    //for every value in every element
    for(size_t i = 0; i < num_keys_; i++){
        Element<K,V> ellie = *(data_+ i);
        K * temp_ary = nullptr;

        for(size_t i = 0; i < ellie.count_; i++){
            V ellie_value = *(ellie.values_ + i);
            //see if the value matches and adjust the flag
            //(written to avoid problems with repeat values)
            if (ellie_value ==  search_value) 
                contains_val = true;
        }

        //if at least one value matched, increase the key array size and add the key
        if(contains_val == true){
            temp_ary  = new K [((keys_found == nullptr)? 0: ary_size)+1]{}; //add one space ;

            if (keys_found == nullptr){ //if this is the first matching value
                *temp_ary = ellie.key_;} //just add the key to the location
            else{
                //otherwise, copy all except for the new one, which should be added
                copy(keys_found, keys_found + ary_size, temp_ary);
                *(temp_ary+ary_size) = ellie.key_; }

            swap(temp_ary, keys_found);
            delete [] temp_ary;

            ary_size += 1;   
        }
        contains_val = false; //reset flag     
    }

    return ((keys_found == nullptr)? 0: ary_size);
}//end find_val

template<typename K, typename V> 
bool MVM<K,V>::add(K key,V value){
    Element<K, V> * key_loc = find_key(key);

    //key doesnt exist
    if (key_loc == (data_ + num_keys_) || (*key_loc).key_ != key ){ 

        Element<K,V> new_ellie (key, {value});

        (*this).grow(); //add more space

        Element<K,V> * temp_ary;
        temp_ary  = new Element<K,V> [num_keys_ + 1]{}; //hold the new data_
        bool newest_element_copied = false;

        //for every element in the data
        for(size_t i = 0; i < (num_keys_ + 1); i++){
            //if the element comes before the new one or its at the end and the new ellie hasn't already been copied
            if(((*(data_+ i)).key_ >= new_ellie.key_ || i == num_keys_ + 1) && newest_element_copied == false){
                //then this is where the new_ellie goes
                *(i+temp_ary) = new_ellie;
                newest_element_copied = true; //change flag
            }else if(newest_element_copied == false){
                *(i+temp_ary) = *(data_+ i); //otherwise, copy old eles as usual
            }else if(newest_element_copied == true){
                *(i+temp_ary) = *(data_+ i - 1); //if the new ele is now in the temp, the index will be off by one
            }
        }
        swap(temp_ary, data_);
        num_keys_ += 1;
    }

    //key exists and value doesn't
    else{ 

        K* keys_found =  nullptr; //holds names of keys containing values

        size_t num_matching_keys = (*this).find_value(value, keys_found);

        //if no keys contain the value, add the value
        if(keys_found == nullptr){
            //if values has reached capacity
            if ((*key_loc).count_ == element_array_size)
                return false;
            
            //otherwise, add the value to the end of array and increase count
            *((*key_loc).values_ + (*key_loc).count_ ) = value; 
            (*key_loc).count_ += 1;
        }

        //if the specific key does not yet contain the value 
        if(!(count(keys_found, keys_found + num_matching_keys, key) > 0)){
            //if values has reached capacity
            if ((*key_loc).count_ == element_array_size)
                return false;
            
            //otherwise, add the value to the end of array and increase count
            *((*key_loc).values_ + (*key_loc).count_ ) = value; 
            (*key_loc).count_ += 1;
        }

        //both key and value exist
        else {
                return false;
        }
        return true;
    }
    return true;
}

template<typename K, typename V> 
void MVM<K,V>::grow(){
    //if data_ is empty
    if(num_elements_ == 0){
        //add three spaces to the end
        Element<K,V> * temp_ary;
        temp_ary  = new Element<K,V> [2]{}; 
        swap(temp_ary, data_);
        delete [] temp_ary;

        num_elements_ = 2; //note the new size
    }
    //if data_ is full
    if(num_keys_ == num_elements_){
        //add three spaces to the end
        Element<K,V> * temp_ary;
        temp_ary  = new Element<K,V> [num_elements_ * 2]{}; 
        copy(data_, data_ + num_keys_, temp_ary); 
        swap(temp_ary, data_);
        delete [] temp_ary;

        num_elements_ *= 2; //note the new size
    }
}

