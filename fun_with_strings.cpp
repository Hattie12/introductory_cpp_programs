#include <string>
using std::string;
#include<cmath>
using std::pow;
#include<iostream>
using std::cout; using std::endl;


//return lower case version of string 
    std::string lower_case(const std::string& message){ 

    //the lowercase version
    std::string new_message = "";
    
    
   
    //for each letter
    
    for(auto ch: message)
    
        {
        
        //if its a capital letter, make it lowercase
        
        if (ch>64 && ch< 91) {
            
            //and add it to the end of the new string
            
            new_message.push_back(ch+32);
            
        }
        
        else{
            
            //if its already lowercase, or another character, add it to the string
            
            new_message.push_back(ch);
            
    }
    
    }
    
    
    
    
    return new_message;

}










//returns binary version of a character

string to_binary(char ch){
    
    std::string char_code = ""; //the return string
    
    
    
    
    //set ch equal to the character of the letter index
    
    ch = ch - 97;
    
    
    
    
    //if ch is a lowercase letter
    
    if (ch< 97 && ch >0) {
        
        
        
        
        //convert it to binary
        
        for(int i = 4; i>=0; i--){
        
            //if the base 10 can be divided by 2 to the corresponding power
            
            if ( ch/(pow(2,i)) >= 1) {
            
                //add the number and subtract the value
                
                char_code.push_back('1');
                
                ch = ch - pow(2,i);
                
            }
            
            else
            
            {
            
                //there is no value for that value of 2^i
                
                char_code.push_back('0');
                
            }
        
        }
        
        
        
        
        //if its not a number, return empty string
        
        return char_code;
    
    }
    
    return "";

}










char from_binary(const std::string& bit_str){

    //if the string is not length 5
    
    if (bit_str.length() != 5) {
    
        return 0;
    
    }
    
    
    
    
    //make sure the string has acceptable characters 
    
    std::string acceptable_characters = "10";
    
    //look at every character in the number
    
    for(char number: bit_str){
        
        //check that the character is allowed
        
        if(acceptable_characters.find(number) != std::string::npos){ 
        
        }
        
        else
        
        {
            
            //if its not, return null
            
            return 0;
        
        }
    
    }
    
    
    
    
    std::string temp_str = bit_str;//a temp variable for casting
    
    
    
    
    //if it does, cast the binary string to a character
    
    //this is fine because the string values are known
    
    int char_numb = (std::stoi(temp_str, nullptr,2)+97);
    
    //if the character is a lowercase letter, return the char
    
    if ((char_numb <= 122 && char_numb >= 97)) {
    
        return static_cast<char>(char_numb);
    
    }
    
    
    return 0;

}










//make sure the message has enough characters for encoding

bool check_message(const std::string& plaintext, const std::string& secret_message){

    int count = 0;//the message length
    
    
    
    
    //for each character
    
    for(auto ch: plaintext){
    
        //count it if its a capital or lowercase letter
        
        if((ch< 123 && ch >96) || (ch>64 && ch< 91)){
        
            count += 1;
        
        }
    
    }
    
    
    
    
    //if the plaintext is greater than the secret times five, its good
    
    return (5*secret_message.length() <= count);

}










string encode(const string& plaintext, const string& secret_message){


    
    
    //make sure the lengths work
    
    if (!check_message(plaintext, secret_message)) {
        
        return "Error";
    
    }
    
    
    
    
    //covert everything to lowercase
    
    std::string obvious_message = lower_case(plaintext);
    
    std::string hidden_message = (secret_message);
    
    
    
    
    std::string encoded_text = ""; //the final message
    
    int index = 0;//the index of the obvious_message
    
    
    
    
    //for every character in the hidden_message
    
    for(auto ch: hidden_message){
    
        std::string char_code = to_binary(ch); //the corresponding binary string
        
        int encoded_binary_index = 0; //how far along the binary string is
        
        
        
        
        //while the end of the encoded character string hasn't been reached
        
        while(encoded_binary_index <= 4){
        
            char next_char = obvious_message[index];
            
            
            
            
            //if the next letter is a random character
            
            if (!(next_char < 123 && next_char >96) && 
            
            !(next_char>64 && next_char< 91)){
                
                //add the random character
                
                encoded_text.push_back((next_char));
            
            }
            
            //if the binary code index is on a 1
            
            else if (char_code[encoded_binary_index] == '1') {
            
                //add the capitalized num and move to the next binary number
                
                encoded_text.push_back(toupper(next_char));
                
                encoded_binary_index += 1;
            
            }
            
            else{
            
                //if the code index points to a zero, don't capitalize
                
                encoded_text.push_back((next_char));
                
                //move to the next part of the code
                
                encoded_binary_index += 1;
            
            }
            
            //move to the next letter in the obvious_message
            
            index +=1;
        
        }
    
    }
    
    
    
    
    //if theres extra letters in the obvious message, add it to the end
    
    if ((index+1) < obvious_message.length()) {
        
        encoded_text = encoded_text + obvious_message.substr(index);
    
    }
    
    return encoded_text;

}




//takes a 5 letter string and decodes it into a five number binary code
string string_to_binary(std::string letters){
    std::string binary_string = "" ;//holds the binary number
    
    
    //for every letter
    for(auto ch: letters) 
    {
        
        //if its lowercase 
        if (ch< 123 && ch >96){ 
            //append 0 to the binary code 
            binary_string = binary_string + '0'; 
    }
    
    else 
        { 
        //if its CAPS, add 1 
        binary_string = binary_string + '1'; 
    }
    
    }
    
    
    return binary_string; 
}

//decode from a secret message to a string
string decode(const string& encoded_message){

std::string just_letters = "";//holds the obvious message without odd characters


//for each character
for(auto ch: encoded_message){ 
    //count it if its a capital or lowercase letter 
    if((ch< 123 && ch >96) || (ch>64 && ch< 91)){ 
        just_letters= just_letters + ch;

}
}


//if the lenth of the string of letters is ragged
if(!just_letters.length() % 5) {    
    //get rid of the ragged edge    
    just_letters = just_letters.substr(0,just_letters.length() - just_letters.length() % 5);
}


//holds the secret message
std::string secret_message = "";


//for every five letters
for(size_t i = 0; i < just_letters.length()-1; i+=5)
{
    
    //turn the five letters to a binary string, 
   
    //turn the binary string to a character
    
    //and append the character
    
    
    
    
    secret_message = secret_message + from_binary(string_to_binary(just_letters.substr(i, 5)));
    



}

return secret_message;

}
