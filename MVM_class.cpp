#include "proj09_class.h"
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<initializer_list>
using std::initializer_list;
#include<iostream>
using std::ostream;
#include<algorithm>
using std::lower_bound;
#include<iterator>
using std::ostream_iterator; using std::distance;
#include<iostream>
using std::cout;

///////element class definitions and friends///////

Element::Element (string key, initializer_list<string> values): key_(key), values_(values) {}

bool Element::operator==(const Element& ele_to_compare)const{
    //eles only have keys and values, so if they're the same the eles are the same
    return (key_ == ele_to_compare.key_ && values_ == ele_to_compare.values_);
}

ostream& operator<<(ostream& out, Element& ele){
    //should be of the form "key:val1,val2,val3"
    out << ele.key_ << ":";

    //copy values without a trailing comma
    copy(ele.values_.begin(), ele.values_.end() - 1, ostream_iterator<string>(out,","));
    out << (*(ele.values_.end()-1));
    return out;
}

///////MVM class definitions and friends////////

//see if/find the key exists in the vector of elements
vector<Element>::iterator MVM::find_key(string input_key){
    return lower_bound(data_.begin(), data_.end(), input_key, 
        [](Element ele, string input_key){return ele.key_ < input_key;});
}

vector<string> MVM::find_value(string input_value){
     //for iterating through the vec of eles in the class 
    vector<Element>::const_iterator test_ele = data_.cbegin();
    vector<Element>::const_iterator end = data_.cend();
    vector<string> matching_keys {}; //will hold keys to eles containing the input_value

    //for every element, see if one of the element's values matches the input value
    while(test_ele != end){
        if ( find(((*test_ele).values_).cbegin(), ((*test_ele).values_).cend(), input_value) 
            !=  ((*test_ele).values_).cend())  {

            matching_keys.push_back((*test_ele).key_);//add key name if input_val exists there
        }
        
        test_ele++;
    }
    return matching_keys;
}

MVM::MVM(std::initializer_list<Element> data): data_(data){}

size_t MVM::size(){
    return distance(data_.begin(),data_.end());
}

bool MVM::add(string key,string value){
    vector<Element>::iterator key_loc = find_key(key);
  
    //key doesnt exist
    if (key_loc == data_.end() || (*key_loc).key_ != key ){ 
        Element new_ele (key,{value});
        data_.insert(key_loc, new_ele);
    }

    //key exists and value doesn't
    else if(find(find_value(value).begin(), find_value(value).end(), key) 
            != find_value(value).end() && 
            find((*key_loc).values_.begin(), (*key_loc).values_.end(), value) == (*key_loc).values_.end()){
        //insert value into the string vec of the associated element
        (*key_loc).values_.push_back(value);
    }

    //both key and value exist
    else {
        return false;
    }
    return true;
}

bool MVM::remove_key(string key){
    vector<Element>::iterator key_loc = find_key(key);

    //if the key doesnt exist
    if (key_loc == data_.end() || (*key_loc).key_ != key ){ 
        return false;
    }
    //key exists
    else{
        data_.erase(key_loc);
        return true;
    }
}

vector<string> MVM::remove_value(string value){
    vector<string> matching_keys {}; //holds keys containing removed values
    vector<string>::iterator value_loc{}; 

    for(Element & ellie: (*this).data_){
        //location of value to remove
        value_loc = find(ellie.values_.begin(), ellie.values_.end(), value);
        
        //if necessary, remove value and save the key name
        if (value_loc != ellie.values_.end()) {
            matching_keys.push_back(ellie.key_);
            ellie.values_.erase(value_loc);
        }
    }
    
    return matching_keys;
}

//MVM friend function
ostream& operator<<(ostream& out, MVM& mapa){
    //print every element using the Element class's friend function
    for(Element ellie: mapa.data_){
        out << ellie;

        //add a space unless its the last element
        if(!(ellie == *(mapa.data_.end()-1)))
            out << " ";
    }

    return out;
}
