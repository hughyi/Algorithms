#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <map>

using namespace std;

const int BYTES_PER_INT = 4;
const int BITS_PER_BYTE = 8;
const int MASK_LAST_8_BITS = 255;

class HuffmanTreeNode
{
public:
  bool isEOF;
  bool isLeaf;
  char symbol;
  int frequency;
  HuffmanTreeNode *left = nullptr;
  HuffmanTreeNode *right = nullptr;

  HuffmanTreeNode(char inputChar, bool isEOF, int frequency)
      : symbol(inputChar), isEOF(isEOF), frequency(frequency), isLeaf(!isEOF)
  {
  }

  HuffmanTreeNode(int frequency)
      : frequency(frequency), isLeaf(false), isEOF(false)
  {
  }
  // recursive function to generate Huffman codes for each character
  void generateHuffmanCode(string &huffmanCode, map<char, string> &huffmanCodeMap, string &eofCode)
  {
    if (isLeaf)
    {
      huffmanCodeMap[symbol] = huffmanCode;
      return;
    }

    if (isEOF)
    {
      eofCode = huffmanCode;
      return;
    }

    if (left != nullptr)
    {
      huffmanCode.push_back('0');
      left->generateHuffmanCode(huffmanCode, huffmanCodeMap, eofCode);
      huffmanCode.pop_back();
    }

    if (right != nullptr)
    {
      huffmanCode.push_back('1');
      right->generateHuffmanCode(huffmanCode, huffmanCodeMap, eofCode);
      huffmanCode.pop_back();
    }
  }
};

// compare nodes in priority_queue
struct NodeCompare
{
  bool operator()(HuffmanTreeNode *node1, HuffmanTreeNode *node2)
  {
    return node1->frequency > node2->frequency;
  }
};

// build the Huffman Tree based on the character frequencies
HuffmanTreeNode *buildHuffmanTree(map<char, int> &frequencyMap)
{
  priority_queue<HuffmanTreeNode *, vector<HuffmanTreeNode *>, NodeCompare> pq;

  for (auto it : frequencyMap)
  {
    pq.push(new HuffmanTreeNode(it.first, false, it.second));
  }
  pq.push(new HuffmanTreeNode('a', true, 1));

  while (pq.size() > 1)
  {
    HuffmanTreeNode *node1 = pq.top();
    pq.pop();
    HuffmanTreeNode *node2 = pq.top();
    pq.pop();
    HuffmanTreeNode *newNode = new HuffmanTreeNode(node1->frequency + node2->frequency);
    newNode->left = node1;
    newNode->right = node2;
    pq.push(newNode);
  }

  return pq.top();
}

// recursively delete nodes in the Huffman Tree
void deleteHuffmanTree(HuffmanTreeNode *root)
{
  if (root)
  {
    deleteHuffmanTree(root->left);
    deleteHuffmanTree(root->right);
    delete root;
  }
}

// create a frequency map from the input file
void createFrequencyMapFromInput(map<char, int> &frequencyMap, fstream &input_file)
{
  char symbol;
  while (input_file >> noskipws >> symbol)
  {
    frequencyMap[symbol]++;
  }
}

// convert an integer to a byte array
void intToByteArray(int n, char byteArray[])
{
  for (int i = 0; i < BYTES_PER_INT; i++)
  {
    byteArray[i] = ((n >> ((BYTES_PER_INT - i - 1) * BITS_PER_BYTE)) & MASK_LAST_8_BITS);
  }
}

// compress the input file using Huffman coding
void compress(map<char, int> &frequencyMap, map<char, string> &huffmanCodeMap, string eofCode, fstream &input_file, fstream &compressed_file)
{
  char symbol;
  char byteArray[BYTES_PER_INT];
  queue<char> charQueue;

  unsigned long cnt = 0;
  for (auto it : frequencyMap)
  {
    intToByteArray(it.second, byteArray);
    cnt += 1 + BYTES_PER_INT;
  }
  compressed_file << cnt << '\t';

  for (auto it : frequencyMap)
  {
    compressed_file << it.first;
    intToByteArray(it.second, byteArray);
    for (int i = 0; i < BYTES_PER_INT; i++)
    {
      compressed_file << byteArray[i];
    }
  }

  while (input_file >> noskipws >> symbol)
  {
    for (auto it : huffmanCodeMap[symbol])
    {
      charQueue.push(it);
    }
  }
  for (auto it : eofCode)
  {
    charQueue.push(it);
  }

  while (!charQueue.empty())
  {
    char outputInt = 0;
    int val = 0;
    for (int i = 7; i >= 0; i--)
    {
      if (!charQueue.empty())
      {
        val = (charQueue.front() == '0') ? 0 : 1;
        charQueue.pop();
      }
      outputInt |= val << i;
    }
    compressed_file << outputInt;
  }
}

// convert a byte array to an integer
int bytesToInt(char byteArray[])
{
  int result = 0;

  for (int i = 0; i < BYTES_PER_INT; i++)
  {
    result |= ((byteArray[i] & MASK_LAST_8_BITS) << ((BYTES_PER_INT - i - 1) * BITS_PER_BYTE));
  }
  return result;
}

// create a frequency map from the header of the compressed file
void createFrequencyMapFromHeader(map<char, int> &frequencyMap, fstream &header_file)
{
  char symbol;
  char byteArray[BYTES_PER_INT];
  unsigned long header_size;
  char padding;
  header_file >> header_size;
  header_file >> noskipws >> padding;

  for (int i = 0; i < (header_size / 5); i++)
  {
    header_file >> noskipws >> symbol;
    for (int i = 0; i < BYTES_PER_INT; i++)
    {
      if (!(header_file >> byteArray[i]))
      {
        cerr << "Error reading header file\n";
        exit(1);
      }
    }
    frequencyMap[symbol] = bytesToInt(byteArray);
  }
}

// read the binary stream from the compressed file
void readBinaryStream(map<char, int> &frequencyMap, vector<int> &binaryString, fstream &compressed_file)
{
  char symbol;

  while (compressed_file >> noskipws >> symbol)
  {
    for (int i = 7; i >= 0; i--)
    {
      binaryString.push_back((symbol >> i) & 1);
    }
  }
}

// decompress the binary string and writes to the output file
void decompress(vector<int> &binaryString, fstream &output_file, HuffmanTreeNode *root)
{
  HuffmanTreeNode *ptr = root;

  if (root->isEOF)
    return;

  for (auto it : binaryString)
  {
    if (it == 1)
    {
      ptr = ptr->right;
    }
    else
    {
      ptr = ptr->left;
    }
    if (ptr->isEOF)
      break;
    if (ptr->isLeaf)
    {
      output_file << ptr->symbol;
      ptr = root;
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    cerr << "Usage: ./mp3_20171461 <user_command> <input_filename>\n";
    return 1;
  }

  string operation = argv[1];
  string input_filename = argv[2];
  fstream infile, outfile;
  map<char, int> frequencyMap;

  infile.open(argv[2], ios::in | ios::binary);
  if (!infile.is_open())
  {
    cerr << "Error: Unable to open input file.\n";
    return 1;
  }

  if (operation == "-c")
  {
    input_filename += ".zz";
    outfile.open(input_filename, ios::trunc | ios::out | ios::binary);

    map<char, string> huffmanCodeMap;
    string huffmanCode = "";
    string eofCode;

    createFrequencyMapFromInput(frequencyMap, infile);
    infile.clear();
    infile.seekg(0);

    HuffmanTreeNode *root = buildHuffmanTree(frequencyMap);
    root->generateHuffmanCode(huffmanCode, huffmanCodeMap, eofCode);

    compress(frequencyMap, huffmanCodeMap, eofCode, infile, outfile);
    deleteHuffmanTree(root);
  }
  else if (operation == "-d")
  {
    input_filename += ".yy";
    outfile.open(input_filename, ios::trunc | ios::out | ios::binary);

    vector<int> compressedFileBinaryString;

    createFrequencyMapFromHeader(frequencyMap, infile);
    readBinaryStream(frequencyMap, compressedFileBinaryString, infile);
    HuffmanTreeNode *root = buildHuffmanTree(frequencyMap);

    decompress(compressedFileBinaryString, outfile, root);
    deleteHuffmanTree(root);
  }
  else
  {
    cerr << "Error: user command must be \"-c\" or \"-d\"\n";
    return 1;
  }

  infile.close();
  outfile.close();
  return 0;
}