//
// Project 6 – File Compression
// creating a binary tree and priority queue-based file compression algorithm.
//
// CS 251, Fall 2022
// Imran Khan
// UIN: 654046372
// using vscode on windows
// 11/28/2022
//

#pragma once
#include <unordered_map>
#include "bitstream.h"
#include "hashmap.h"
#include <queue>
#include <string>     


typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

struct compare
{
    bool operator()(const HuffmanNode *lhs,
        const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};

//
// This method frees the memory allocated for the Huffman tree.
//
void _freeTreehelper(HuffmanNode* node){

  //if the tree is empty
  if(node == nullptr)
  {
    return;
  }
  
  _freeTreehelper(node->zero);
  _freeTreehelper(node->one);
  
  delete node;
  
}

//frees all the memory in the tree
void freeTree(HuffmanNode* node) {
    
    _freeTreehelper(node);
    
}


// The frequency map is created by this function. It reads from the filename if isFile is true. It reads from a string filename if isFile is false.
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
  
    //checks if the isFIle is true or false
    if (isFile)
    {
      char temp;
      ifstream inFS(filename);
      
      while (inFS.get(temp))
      {
        if(map.containsKey(temp)){//checks the map for the key
          map.put(temp, map.get(temp) + 1);
        } 
        else 
        {
          map.put(temp,1);
        }

      }

    } 
    else 
    { 
    for (char temp : filename)
      {
      if(map.containsKey(temp))
      {
        map.put(temp, map.get(temp) + 1);
      } 
      else 
      {
        map.put(temp,1);
      }
    }
  } 
  map.put(PSEUDO_EOF,1);//checks for the end of the file
}

//
// The frequency map is used by this function to create an encoding tree.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {

  //makes an endocing tree
  priority_queue<HuffmanNode*, vector<HuffmanNode*>, compare> temp; 
  int counter = 1;
  for (auto run: map.keys())
  {
    //makes a new node
    HuffmanNode* newTemp = new HuffmanNode; 
    newTemp -> character = run; 
    newTemp-> one = nullptr;
    newTemp-> zero = nullptr; 
    newTemp-> count = map.get(run); 
    temp.push(newTemp);
  }
  
  while(temp.size() != counter)
  {
    //adds the elements inside and uses them
    HuffmanNode* first = temp.top(); 
    temp.pop(); 
    HuffmanNode* second = temp.top(); 
    temp.pop(); 
    HuffmanNode* newTemp = new HuffmanNode;
    newTemp-> count = first-> count + second -> count; 
    newTemp -> character = NOT_A_CHAR; 
    newTemp-> one = second; 
    newTemp-> zero = first; 
    
    temp.push(newTemp);
  }

  return temp.top(); //returns the made tree
}

//
//For creating the encoding map, a recursive helper function is used.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str,HuffmanNode* prev) {

  //check if the tree is empty
  if(node == nullptr){
    return;
  }
  else {
    //adding in the ones and zeros if needed recursively
     if(prev->character == NOT_A_CHAR&& node->character != NOT_A_CHAR)
     {
     encodingMap[node->character] = str;
     }

    _buildEncodingMap(node->zero,encodingMap,str+"0",prev);
    _buildEncodingMap(node->one,encodingMap,str+"1",prev);
    
  }


    
}

//
// From an encoding tree, this function creates the encoding map.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
  hashmapE encodingMap;
  string temp = "";
  _buildEncodingMap(tree, encodingMap, temp, tree);

  return encodingMap;  //returns the encoding map
}

//
// Using the encodingMap, this function encodes the data from the input stream and writes it to the //output stream. The size argument, which is given by reference, is used to set the outcome of this //function's calculation of the number of bits sent to the output stream. A string representation of //the output file is also returned by this method, which is especially helpful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,int &size, bool makeFile) {

  char templet;
  string tempstring = "";
  string tempchar = "";
  size = 0;

  // getitng the values
  while (input.get(templet)) {
    tempstring += encodingMap[templet];
  }
  tempstring += encodingMap[PSEUDO_EOF];//adding to the string for the encode returns this

  
  if(makeFile){
      if(makeFile){
        for (int i = 0; i < tempstring.size(); i++) {
          
          templet  = tempstring[i];
          if(templet == '1'){//adding 1
            output.writeBit(1);
          }
          else if (templet == '0'){//adding 0
            output.writeBit(0);
          }
         
        }
       
      }
  }
 
  size = tempstring.size();//size of the encoded string
    
  return tempstring;//encoded string type

  
}


//
// Using the encodingTree, this function decodes the input stream and publishes the outcome to the //output stream. A string representation of the output file is also returned by this method, which is //especially helpful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    
	HuffmanNode* tempecodingTree = encodingTree;
  string tempstorage = ""; 
  int amountofBit = 0; 
  
  //reading the file
  while(!input.eof()){
    amountofBit = input.readBit();//getting the bit
    if (amountofBit == 0){
      tempecodingTree = tempecodingTree -> zero;
    }
    else if (amountofBit == 1){//trying to covert it back
      tempecodingTree = tempecodingTree -> one;
    }
    if(tempecodingTree -> character == PSEUDO_EOF){
        break;
      }
    

    if (tempecodingTree -> character != NOT_A_CHAR){
      tempstorage += (tempecodingTree -> character); 
      output.put(tempecodingTree->character);
      tempecodingTree = encodingTree; 
    }
      
 
    
  }
  return tempstorage; //decoded string is returned
}



//
// The entire compression process is finished by this function. This function, given a file, generates //a frequency map, an encoding tree, an encoding map, and an encoded file. Don't forget to include the //frequency map in the output file's header. The bit pattern should be sent as a string together with a //compressed file created with the name (filename + ".huf").
//
string compress(string filename) {
  
  string tempFilename = filename;
  bool isFile = true;
  hashmapF map;
  buildFrequencyMap( filename,  isFile, map);//making the frequency map from the file
  
  HuffmanNode* encodingTree = nullptr;
  encodingTree =  buildEncodingTree(map);//making the treee from the map 
  
  hashmapE encodedMap;
  encodedMap =  buildEncodingMap(encodingTree);//building the encoding map


  ofbitstream output(tempFilename + ".huf");//naming the new file and opening it

  string compressedFile;

  ifstream input(tempFilename);
  output << map;
  
  
  int size = 0;
  bool makeFile = true;
  compressedFile = encode(input,encodedMap, output, size,  makeFile);//holds the compressed file string


  freeTree(encodingTree);//frees the tree so no memory leak
  return compressedFile; //returns the compressed file
  
  
}

//
// The full decompression procedure is finished by this function. With the file's filename, which must //end in ".huf," perform the following steps: (1) remove the header and create a frequency map; (2) use //the frequency map to create an encoding tree; and (3) decode the file using the encoding tree. The //following protocol should be followed when this function creates a compressed file.
// The uncompressed file should be called "example unc.txt" if filename = "example.txt.huf." The //uncompressed file's string representation should be the function's return value. Take note that this //function should undo the effects of the compress function.
//
string decompress(string filename) {
  string tempFilename =filename;
  size_t pos = filename.find(".txt");
  //gets the real filename from the file name
  if ((int)pos >= 0) {
      tempFilename = tempFilename.substr(0, pos);
  }
  ifbitstream input(filename);
  tempFilename += "_unc.txt";//adds the new file name with the filename
  

  hashmapF map;
  
  input>>map;
  
  HuffmanNode* encodingTree = nullptr;
  encodingTree =  buildEncodingTree(map);
  

  ofstream output(tempFilename);//opens the new file and inputs the decompressed data
  string decompressedFile;
  
  decompressedFile = decode( input, encodingTree, output);//holds the decoding string
  freeTree(encodingTree);//frees the tree so no memeory leaks
  return decompressedFile;  //returns the decoding string 
}