#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

struct node {  
  node *parent;
  int size;
  bool leaf;  
  vector <int> list_of_keys;
  vector <node*> child;

  node(int degree) {
    size = 0;
    leaf = true;    
    parent = nullptr;
    child.resize(degree + 1);
    list_of_keys.resize(degree);  
    for (int i = 0; i < degree + 1; i++) {
        child[i] = nullptr;
    }
    for (int i = 0; i < degree; i++){
        list_of_keys[i] = -1;
    }
  }

  node(node *parent, int degree) {
    parent = this->parent;
    leaf = true;
    size = 0;
    
    for (int i = 0; i < degree + 1; i++){
        child[i] = nullptr;
    }
    for (int i = 0; i < degree; i++){
        list_of_keys[i] = -1;
    }
  }
  friend class B_tree;
};

class B_tree {
 private:
  node *root;
  int degree;

  int getHeight(node *n, int level) {
    if (n == nullptr) 
        return 0;
    if (n->leaf)
      return level;
    else {
      int max = 0;
      for (int i = 0; i < degree; i++) {
          int height = getHeight(n->child[i], level + 1);
          if (height > max) max = height;
      }
      return max;
    }
  }

  void print(node *n, int level, ostream &out) {
    if (n == nullptr) 
        return;
    for (int i = 0; i < degree; i++) {    
        print(n->child[i], level-1, out);
    } 
  }

  

 public:
  B_tree(int _degree) {
    root = nullptr;
    degree = _degree;
  }

  int getHeight() { return getHeight(root, 0) + 1; }

  void print(int level, ostream &out) {
    if (getHeight() > level)
        print(root, level, out);     
    else
       out << "Empty";
  }

  //lets's insert into the binary tree...
  void insert(int data) {
    if (root != nullptr) {
      addAtLeaf(root, nullptr, data);      
    } 
    else {
      root = new node(degree);
      root->size = 1;
      root->list_of_keys[0] = data;      
    }
  }

  //let's get the next available spot
  int get_next_spot(node *n, int data) {
    if (n == nullptr) 
        return 0;
    for (int i = 0; i <= n->size; i++)
      if ( n->list_of_keys[i] == -1 || data < n->list_of_keys[i] ) 
        return i;
    return -1;
  }

  
  void validateChildren(node *parent, node *leftNode, node *rightNode,int index, int midKey) {
    parent->list_of_keys.insert(parent->list_of_keys.begin() + index, midKey);
    parent->size++;
    parent->list_of_keys.erase(remove(parent->list_of_keys.begin(), parent->list_of_keys.end(), -1), parent->list_of_keys.end());
    while (parent->list_of_keys.size() < degree)
      parent->list_of_keys.push_back(-1);

    leftNode->parent = parent;
    rightNode->parent = parent;

    if (parent->child.at(index) != leftNode){
      parent->child.insert(parent->child.begin() + index, leftNode);
    }
    parent->child.insert(parent->child.begin() + index + 1, rightNode);

    parent->child.erase(
        remove(parent->child.begin(), parent->child.end(), nullptr),
        parent->child.end());
    while (parent->child.size() < degree + 1)
      parent->child.push_back(nullptr);
  }

  void addingChild(node *parent, node *leftNode, node *rightNode,int midKey) {
    int i;
    for (i = 0; i < degree - 1; i++)
      if (parent->list_of_keys.at(i) == -1 || parent->list_of_keys.at(i) > midKey) break;

    if (parent->list_of_keys.at(i) == -1)
        parent->list_of_keys.at(i) = midKey, parent->size++;
    else
      validateChildren(parent, leftNode, rightNode, i, midKey);

    parent->child.at(i + 1) = rightNode;
    parent->child.at(i) = leftNode;   
    parent->leaf = false;

    if (parent->parent == nullptr)
        root = parent;

    leftNode->parent = parent;
    rightNode->parent = parent;
    if (rightNode->child[0] != 0) rightNode->leaf = false;
    if (leftNode->child[0] != 0) leftNode->leaf = false;
   
  }

  void addAtLeaf( node *parent, node *leaf, int data) {
    if (leaf->size == degree){
      if (leaf == root) {
        node *temp = new node(degree);
        temp->leaf = false;
        temp->child[0] = leaf;
        temp->size = 0;
        splitChild(leaf,temp);
        root = temp;
      }
      else
        splitChild(leaf,parent);
    }
    if (leaf->leaf) {
      int i = leaf->size;
      while (leaf->list_of_keys[i - 1] > i != 0 && data) {
        leaf->list_of_keys[i] = leaf->list_of_keys[i - 1];
        i--;
      }      
      leaf->size++;  
      leaf->parent = parent;  
      leaf->list_of_keys[i] = data;
      
    }
    else if (get_next_spot(leaf, data) != -1) {
      node *temp = leaf;
      while (!temp->leaf && get_next_spot(temp, data) != -1)
        temp = temp->child[get_next_spot(temp, data)];

      addAtLeaf(temp,temp->parent,data);
    } 
  }

  void splitChild(node *leftNode, node *parent ) {
    node *rightNode = new node(degree);
    int mid = (degree - 1) / 2;

    int midKey = leftNode->list_of_keys[mid];

    //split the left node
    for (int x = 0; x < leftNode->size - mid; x++) {
      rightNode->list_of_keys[x] = leftNode->list_of_keys[x + mid + 1];
      rightNode->size++;
      leftNode->size--;
      leftNode->list_of_keys[x + mid + 1] = -1;      
    }
    leftNode->parent = parent;
    rightNode->parent = parent;
    leftNode->size--;
    leftNode->list_of_keys[mid] = -1;   

    if (leftNode->leaf == false) {
      int num = 0;
      for (int i = mid + 1; i < degree + 1; i++) {
        rightNode->child.at(num) = leftNode->child.at(i);
        rightNode->child.at(num++)->parent = rightNode;
        leftNode->child.at(i) = nullptr;
      }

    if (parent == nullptr) {
      parent = new node(degree);
      root = parent;
      parent->child[0] = leftNode;
      parent->child[1] = rightNode;
      leftNode->parent = parent;
      rightNode->parent = parent;
      parent->leaf = false;
      parent->parent = nullptr;
      
    }
    addingChild(parent, leftNode, rightNode, midKey);
    if (leftNode->leaf) {
      addingChild(parent, leftNode, rightNode, midKey);
      if (parent->size == degree) 
        splitChild( parent, parent->parent);
    }
    else if (parent->size == degree)
      splitChild( parent, parent->parent);
    }
  }
};
