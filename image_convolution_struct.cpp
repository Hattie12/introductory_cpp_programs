#include "proj08-struct.h"
#include<fstream>
using std::ifstream; using std::ofstream;
#include<vector>
using std::vector;
#include<string>
using std::string;
#include<ostream>
using std::endl;
#include<stdexcept>
using std::invalid_argument; using std::out_of_range;
#include<iostream>
using std::cout; using std::endl;
#include<algorithm>
using std::transform;

vector<long> split(const string &s){
    char delim = ' ';
    vector<long> pixels{};

    size_t start_pos = 0; //the beginning of the word
    size_t next_splitter = s.find(delim, start_pos); //index one after end of word

    while(true){
        //if there is another spittler symbol in the text
        if(next_splitter != string::npos){

            //if its not just triggered by an extra space
            if(s.substr(start_pos, next_splitter - start_pos) != ""){
                //add the number before the next splitter
                pixels.push_back(stoi(s.substr(start_pos, next_splitter - start_pos)));
            }

            //the next word will start one past the splitter
            start_pos = next_splitter +1;
            //and find the next splitter that's after the start of the next word
            next_splitter = s.find(delim, start_pos);
        }
        else{ //no more splitter symbols

            //if its not just triggered by an extra space
            if(s.substr(start_pos) != ""){
                //chop off the end of the string
                pixels.push_back(stoi(s.substr(start_pos)));
            }
    
            break;
        }
    }

    return pixels;
}

//constructor
Image::Image(string f_name) {
    ifstream in_file(f_name);//the file to read

    string line{}; 

    getline(in_file,line);//move past "P2"
    getline(in_file,line); //next line might be a comment

    //keep trying the next line until it gets to the dimensions
    while(true){
        try{
                height_ = split(line)[0];
                width_ = split(line)[1];
                break;
        }
        catch(const std::invalid_argument& e) {
            getline(in_file,line);
        }
    }
    
    //read max_val
    getline(in_file,line);
    max_val_ = split(line)[0];

    //for every row of pixels
    while(getline(in_file,line)){
        //split into longs and add to the vec " "
       pixels_.push_back(split(line));
    }
}

void Image::write_image(string f_name) const{
    ofstream file;
    file.open (f_name);

    //write initial height, width, P2 and max val
    file << "P2" << endl;
    file << height_ << " " << width_ << endl;
    file << max_val_ << endl;

    //write out every pixel
    for(vector<long> column: pixels_){
        for(long pixel: column){
            file << pixel << " ";
        }
        file << endl;
    }
}

double test_index(const Image& old_img, double first, double second){
    //if the index is in the vector, return the corresponding number, otherwise return zero
    return (first >= 0 && first < old_img.width_ && second >= 0 && second < old_img.height_)?
        old_img.pixels_[first][second]:
        0;
}

Image Image::convolve(vector<vector<long>> mask, long div, long whiten) const{
    Image new_image = *this; //copying the old image will make the new one the right size

    //for every pixel in the new image
    for(int columns = 0; columns < width_; columns++){
        for(int rows = 0; rows < height_; rows++){
            //multiply and then add the mask pixels times the original pixels
            //then divide and whiten
            new_image.pixels_[columns][rows] =
                (mask[0][0] * test_index(*this,columns-1,rows - 1) +
                mask[0][1] * test_index(*this,columns,rows-1) +
                mask[0][2] * test_index(*this,columns+1,rows-1) +
                mask[1][0] * test_index(*this,columns-1,rows ) +
                mask[1][1] * test_index(*this,columns,rows ) +
                mask[1][2] * test_index(*this,columns+1,rows ) +
                mask[2][0] * test_index(*this,columns-1,rows + 1) +
                mask[2][1] * test_index(*this,columns,rows + 1) +
                mask[2][2] * test_index(*this,columns+1,rows + 1)) 
                / div
                + whiten;
            
            //correct pixels that are out-of-bounds
            if (new_image.pixels_[columns][rows] <= 0){
                new_image.pixels_[columns][rows] = 0;
            }
            else if(new_image.pixels_[columns][rows] >= max_val_){
                new_image.pixels_[columns][rows] = max_val_;
            }
        }
    }

    return new_image;
}

void correct_pixel(const bool &is_even, vector<long>::iterator pixel, const long &max_val){
    //adjusts pixels to be even or odd without giving a value out-of-bounds

    if((is_even) && !((*pixel) % 2)){ //when pixel should be even but isnt
        *pixel += 1;
        return;
    }
    
    //when it should be odd but isn't, and its max_val
    if(!(is_even) && ((*pixel) % 2) && ((*pixel) == max_val)){
        *pixel -= 1;
        return;
    }
    if(!(is_even) && ((*pixel) % 2)){ //when its should be even but isnt
        *pixel -= 1;
        return;
    }
}

void Image::embed(const Image& secret){
    //column iterators
    vector<vector<long>>::const_iterator secret_column_itr = secret.pixels_.cbegin();
    vector<vector<long>>::iterator plain_column_itr = pixels_.begin();

    ///row iterators
    vector<long>::const_iterator secret_pixel_itr = (*secret_column_itr).cbegin();
    vector<long>::iterator plain_pixel_itr = (*plain_column_itr).begin();

    //for every pixel in secret
    while(secret_column_itr != secret.pixels_.cend()){
        while(secret_pixel_itr != (*secret_column_itr).cend()){
            //adjust corresponding plain pixel

            //secret is not zero
            if(*secret_pixel_itr){
                //plain should be odd
                correct_pixel(true, plain_pixel_itr, max_val_);
            }

            //secret is zero
            if(!(*secret_pixel_itr)){
                //plain should be even
                correct_pixel(false, plain_pixel_itr, max_val_);
            }

            //check the next pixels
            ++plain_pixel_itr;
            ++secret_pixel_itr;
        }
        //check next rows
        ++plain_column_itr;
        ++secret_column_itr;
        secret_pixel_itr = (*secret_column_itr).cbegin();
        plain_pixel_itr = (*plain_column_itr).begin();
    }
}

//various functions that send the correct mask, divisor, and whiten to consolve
Image Image::extract(long max) const{
    //initialize the new image and temp column to the right size
    Image extracted_img = *this;
    vector<long> new_col = (*this).pixels_[0];

    vector<vector<long>>::const_iterator plain_column_itr = pixels_.cbegin();
    vector<vector<long>>::iterator new_column_itr = extracted_img.pixels_.begin();

    //for every column of pixels
    while(plain_column_itr != pixels_.cend()){
        //old pixels should be equal to max_val or zero if they're even or odd
        transform((*plain_column_itr).cbegin(), (*plain_column_itr).cend(),
                                    (*new_column_itr).begin(), 
                                    [max](long pixel)->long
                                    {return (pixel%2)?max:0;});
        ++plain_column_itr;
        ++new_column_itr;
    }

    extracted_img.max_val_ = max;

    return extracted_img;

/* I couldn't get this to work. 
    long max = max_val_
        //transform each column
        transform(pixels_.cbegin(),pixels_.cend(),extracted_img.pixels_.begin(),
            //by individually transforming each row
            [max](vector<long> column)
            {
                vector<long> new_col = column;
                return transform(column.begin(), column.end(), new_col.begin(), 
                        [max, new_col](long pixel)
                        {return (pixel%2)?max:0;});
            });
*/

}

Image Image::sharpen() const{
    return this -> convolve({{0,-1,0},{-1,5,-1},{0,-1,0}});
}

Image Image::edge_detect() const{
    return this -> convolve({{0,1,0},{1,-4,1},{0,1,0}}, 1, 50);
}

Image Image::blur() const{
    return this -> convolve({{1,1,1},{1,1,1},{1,1,1}}, 9);
}

Image Image::emboss() const{
    return this -> convolve({{-2,-1,0},{-1,1,1},{0,1,2}});
}
