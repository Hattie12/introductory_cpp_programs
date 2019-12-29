#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<map>
#include<iterator>
#include<algorithm>
#include <fstream>
#include<cmath>
using std::ifstream; using std::ostream_iterator; using std::transform;using std::string;
using std::set_intersection; using std::map; 
using map_type = map<string, long>; using std::ostream; using std::cout; using std::endl;
using std::vector; using std::sqrt;

//print the input vector to the ostream
ostream& print_vector (ostream & out, const vector<string> & v){

    //send everything to the iterator separated by a comma
    copy(v.begin(),v.end()-1,ostream_iterator<string>(out,","));

    //for the last element, no comma and then a new line
    out << *(v.end()-1);
    return out;
}

string clean_word(const string& s){
    //make a final string of the correct size to hold the modified one
    std::string temp_str("");

    //remove weird characters
    copy_if(s.begin(), s.end(), back_inserter(temp_str), [](char ch){return isalpha(ch);});

    std::string result{}; //for the results

    //make all characters lowercase
    transform(temp_str.begin(),
        temp_str.end(),
        back_inserter(result),
        [](char ch){
            return (tolower(ch));
        });

    return result;
}

vector<string> split(const string &s, char delim){
    vector<string> words{};//vector for separated words
    size_t start_pos = 0; //the beginning of the word
    size_t next_splitter = s.find(delim, start_pos); //index one after end of word

    //loop until break
    while(true){
        //if there is another spittler symbol in the text
        if(next_splitter != string::npos){
            //add the word before the next splitter
            words.push_back(s.substr(start_pos, next_splitter - start_pos));
            //the next word will start one past the splitter
            start_pos = next_splitter +1;
            //and find the next splitter that's after the start of the next word
            next_splitter = s.find(delim, start_pos);
        }
        else{
            //otherwise, if there are not more splitter symbols
            //chop off the end of the string
            words.push_back(s.substr(start_pos));
            //and exit the infinite loop
            break;
        }
    }

    return words;
}

bool process_words(map_type& word_mapa, string file_name){
    ifstream in_file(file_name);//the file to read

    //if the file doesn't exist, return false
    if(!in_file.is_open())
        return false;

    string line{}; 
    vector<string> words{};

    //for every line in the file
    while(getline(in_file,line)){
        //split the line with the fault " "
       words = split(line, ' ');

        //clean every word, and count it in the map
        for (auto word : words){
            word_mapa[clean_word(word)] += 1;
        }
    }
    
    return true;
}

ostream& print_map(ostream& out, const map_type& m){
    //for every pair
    for(std::pair<std::string,long> word_freq : m){
        //print the first and second word separated by a colon
        out << word_freq.first << ":" << word_freq.second;

        //it the pair isn't the last one, but a comma behind it
        if(word_freq.first != (*(--m.end())).first)
            out << ",";
    }

    return out;
}

//returns jaccard with maps of processed words
//USE only the keys
double jaccard_similarity(const map_type & word_mapa1, const map_type & word_mapa2){ 

    /* This didn't work and I wasn't sure why?
    map_type intersection{};
    map_type::const_iterator start1 = word_mapa1.begin(); 
    map_type::const_iterator start2 = word_mapa2.begin(); 
    map_type::const_iterator end1 = word_mapa1.end(); 
    map_type::const_iterator end2 = word_mapa2.end(); 
    map_type::const_iterator new_mapa_start = intersection.begin();
    set_intersection(start1, end1, start2, end2, new_mapa_start);   
    */
   
   double intersection{0}; //holds the intersection value

    //see how many words appear in both maps (the intersection)
    //for every pair
   for(std::pair<string, int> word_count : word_mapa1){
       //count it if the word appears in both
       if (word_mapa2.find(word_count.first) != word_mapa2.end()){
           intersection++;
       }
   }

    //return the jaccard similarity using the formula
    return intersection / (word_mapa1.size() + word_mapa2.size() 
            - intersection);
}

double calc_norm_factor(const map_type &m){
    double squared_values{}; //holds value under the sqrt sign

    //for every word count, square and add it
    for(std::pair<string, long> word_count : m){
        squared_values += word_count.second * word_count.second;
    }

    //take the square root of all of the squared counts
    return std::sqrt(squared_values);
}

double cosine_similarity_tf(const map_type & word_mapa1, const map_type & word_mapa2){
    double nonnormalized_dot_product{}; //will hold the sum of all of the word counts of 
                                        //map1 times all the word counts of map2

    //for every pair in the first word map
    for(std::pair<string, long> word_count : word_mapa1){
            //if the word also exists in the second map
            if (word_mapa2.find(word_count.first) != word_mapa2.end()){
                //multiply both their counts and add it
                nonnormalized_dot_product += word_count.second * 
                                            (*word_mapa2.find(word_count.first)).second;
            }
        }

    //divide the dot_product by the norm factors for both maps and return
    return nonnormalized_dot_product / (calc_norm_factor(word_mapa1) * calc_norm_factor(word_mapa2));
}
