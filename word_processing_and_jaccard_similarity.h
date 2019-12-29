#pragma once
#include<string>
using std::string;
#include<vector>
using std::vector;
#include<iostream>
using std::ostream;
#include<map>
using std::map;

using map_type = map<string, long >;

ostream& print_vector (ostream &out, const vector<string> &v);
string clean_word(const string&);
vector<string> split(const string &s, char delim =' ');
bool process_words(map_type&, string);
ostream& print_map(ostream& out, const map_type& m);
double jaccard_similarity(const map_type &, const map_type &);
double calc_norm_factor(const map_type &);
double cosine_similarity_tf(const map_type &, const map_type &);
