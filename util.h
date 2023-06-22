//
// STARTER CODE: util.h
//
// TODO:  Write your own header
//

#pragma once

#include <queue>

typedef hashmap hashmapF;
typedef unordered_map<int, string> hashmapE;

struct HuffmanNode {
  int character;
  int count;
  HuffmanNode *zero;
  HuffmanNode *one;
};

struct compare {
  bool operator()(const HuffmanNode *lhs, const HuffmanNode *rhs) {
    return lhs->count > rhs->count;
  }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode *node) {
  if (node == nullptr)
    return;
  freeTree(node->zero);
  freeTree(node->one);
  delete node;
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
  
  if (isFile) {
    ifbitstream inFS;
    string temp;
    inFS.open(filename);
    while (getline(inFS, temp)) {
      for (size_t i = 0; i < temp.size(); i++) {
        if (map.containsKey(int(temp[i]))) {
          int count = map.get(int(temp[i]));
          count++;
          map.put(int(temp[i]), count);
        } else {
          map.put(int(temp[i]), 1);
        }
      }
      if (map.containsKey(int('\n'))) {
        int count = map.get(int('\n'));
        count++;
        map.put(int('\n'), count);
      } else {
        map.put(int('\n'), 1);
      }
    }
  } else {
    for (auto c : filename) {
      if (map.containsKey(int(c))) {
        int count = map.get(int(c));
        count++;
        map.put(int(c), count);
      } else {
        map.put(int(c), 1);
      }
    }
  }
  map.put(PSEUDO_EOF, 1);
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode *buildEncodingTree(hashmapF &map) {
  priority_queue<HuffmanNode*,  vector<HuffmanNode*>, compare> pq;
  vector<int> keys = map.keys();
  
  for (size_t i = 0; i < keys.size(); i++) {
    HuffmanNode* temp = new HuffmanNode;
    temp->zero = nullptr;
    temp->one = nullptr;
    temp->character = keys.at(i);
    temp->count = map.get(keys.at(i));
    pq.push(temp);
  }
  while (pq.size() > 1) {
    HuffmanNode* parent = new HuffmanNode;
    HuffmanNode* temp1 = pq.top();
    pq.pop();
    HuffmanNode* temp2 = pq.top();
    pq.pop();
    parent->character = NOT_A_CHAR;
    parent->count = temp1->count + temp2->count; 
    parent->zero = temp1;
    parent->one = temp2;
    pq.push(parent);
  }
  return pq.top();
}

//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode *node, hashmapE &encodingMap, string str,
                       HuffmanNode *prev) {

  if (node == nullptr) {
    return;
  }
  if (prev->zero == node) {
    str += "0";
  }
  if (prev->one == node) {
    str += "1";
  }
  if (node->character != NOT_A_CHAR) {
    encodingMap.insert(make_pair(node->character, str));
    str.clear();
  }
  prev = node;
  _buildEncodingMap(node->zero, encodingMap, str, prev);
  _buildEncodingMap(node->one, encodingMap, str, prev);
  
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode *tree) {
  hashmapE encodingMap;
  string str = "";
  _buildEncodingMap(tree, encodingMap, str, tree);
  return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream &input, hashmapE &encodingMap, ofbitstream &output,
              int &size, bool makeFile) {
  string str = "";
  string temp;
  while (getline(input, temp)) {
    for (size_t i = 0; i < temp.size(); i++) {
      size += encodingMap.at(temp[i]).size();
      str += encodingMap[temp[i]];
      if (makeFile)
        for (size_t j = 0; j < encodingMap.at(temp[i]).size(); j++) {
          if (encodingMap[temp[i]][j] == '0')
            output.writeBit(0);
          if (encodingMap[temp[i]][j] == '1')
            output.writeBit(1);
          //output.writeBit(stoi(encodingMap[temp[i]].substr(j, 1)));
        }
    }
    size += encodingMap.at('\n').size();
    str += encodingMap['\n'];
    if (makeFile)
      for (size_t i = 0; i < encodingMap['\n'].size(); i++) {
        if (encodingMap['\n'][i] == '0')
          output.writeBit(0);
        if (encodingMap['\n'][i] == '1')
          output.writeBit(1);
        //output.writeBit(stoi(encodingMap['\n'].substr(i, 1)));
      }
  }
  size += encodingMap.at(PSEUDO_EOF).size();
  str += encodingMap.at(PSEUDO_EOF);
  if (makeFile)
    for (size_t i = 0; i < encodingMap.at(PSEUDO_EOF).size(); i++) {
      if (encodingMap[PSEUDO_EOF][i] == '0')
        output.writeBit(0);
      if (encodingMap[PSEUDO_EOF][i] == '1')
        output.writeBit(1);
      //output.writeBit(stoi(encodingMap[PSEUDO_EOF].substr(i, 1)));
    }
  return str;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode *encodingTree, ofstream &output) {
  HuffmanNode* curr = encodingTree;
  string str;
  int currBit = 0;
  while (currBit != -1) {
    currBit = input.readBit();
    if (currBit == 1)
      curr = curr->one;
    if (currBit == 0)
      curr = curr->zero;
    if (curr->zero == nullptr && curr->one == nullptr) {
      if (curr->character == PSEUDO_EOF)
        break;
      str += curr->character;
      output << curr->character;
      curr = encodingTree;
    }
  }
  return str;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
  hashmapF mapF;
 
  string str;
  int size = 0;
  buildFrequencyMap(filename, true, mapF);
  HuffmanNode* tree = buildEncodingTree(mapF);
  hashmapE mapE = buildEncodingMap(tree);
  ofbitstream output(filename += ".huf");
  output << mapF;
  ifstream input(filename);
  str = encode(input, mapE, output, size, true);
  
  return str;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
  hashmapF mapF;
  string str;
  ifbitstream input(filename);
  filename = filename.substr(0, filename.size() - 8);
  ofstream output(filename + "_unc.txt");
  input >> mapF;
  HuffmanNode* tree = buildEncodingTree(mapF);
  str = decode(input, tree, output);
  return str;
}
