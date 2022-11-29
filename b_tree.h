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
      addAtLeaf(nullptr, root, data);      
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
    parent->list_of_keys.erase(remove(parent->list_of_keys.begin(), parent->list_of_keys.end(), -1),
                       parent->list_of_keys.end());
    while (parent->list_of_keys.size() < degree) parent->list_of_keys.push_back(-1);

    leftNode->parent = parent;
    rightNode->parent = parent;

    if (parent->child.at(index) != leftNode)
      parent->child.insert(parent->child.begin() + index, leftNode);
    parent->child.insert(parent->child.begin() + index + 1, rightNode);

    parent->child.erase(
        remove(parent->child.begin(), parent->child.end(), nullptr),
        parent->child.end());
    while (parent->child.size() < degree + 1)
      parent->child.push_back(nullptr);
  }

  // Helper function to add the midKey to the parent and add the children to the
  // parent without affecting the parent's other children. (Does not check for
  // if the parent is full)
  void addchildToParent(node *parent, node *leftNode, node *rightNode,int midKey) {
    int i;
    for (i = 0; i < degree - 1; i++)
      if (parent->list_of_keys.at(i) > midKey || parent->list_of_keys.at(i) == -1) break;

    // If the slot is empty in parent, insert
    if (parent->list_of_keys.at(i) == -1)
        parent->list_of_keys.at(i) = midKey, parent->size++;
    else
      validateChildren(parent, leftNode, rightNode, i, midKey);

    parent->child.at(i) = leftNode;
    parent->child.at(i + 1) = rightNode;
    parent->leaf = false;

    if (parent->parent == nullptr)
        root = parent;

    leftNode->parent = parent;
    rightNode->parent = parent;
    if (leftNode->child[0] != nullptr) leftNode->leaf = false;
    if (rightNode->child[0] != nullptr) rightNode->leaf = false;
  }

  // Adds the new node into the tree at first available leaf,
  // if leaf is full uses splitChild to create a new array
  void addAtLeaf(node *parent, node *n, int data) {
    // If n is a leaf
    if (n->leaf) {
      int i = n->size;
      // Find the first spot where data is less than list_of_keys[i - 1]
      while (i != 0 && data < n->list_of_keys[i - 1]) {
        // Pointer of arrays can change size dynamically
        n->list_of_keys[i] = n->list_of_keys[i - 1];
        i--;
      }
      
      n->list_of_keys[i] = data;
      n->parent = parent;
      n->size++;
    }

    // Else find the first index where the data is bigger that list_of_keys[i] and
    // call addAtLeaf() again to add it to traverse the tree and add it to a
    // leaf
    else if (get_next_spot(n, data) != -1) {
      node *temp = n;
      while (!temp->leaf && get_next_spot(temp, data) != -1)
        temp = temp->child[get_next_spot(temp, data)];

      addAtLeaf(temp->parent, temp, data);
    }

    // Need to determine if node is a leaf within splitChild() if it is, then
    // split node accordingly
    if (n->size == degree){
      if (n == root) {
        node *temp = new node(degree);
        temp->leaf = false;
        temp->child[0] = n;
        temp->size = 0;
        splitChild(temp, n);
        root = temp;
      }
      else
        splitChild(parent, n);
    }
  }

  // n is a full child, split it into two children with parent as the new
  // parent node. Need to find out if n is a leaf or not, pass pointers
  // depending.
  void splitChild(node *parent, node *leftNode) {
    node *rightNode = new node(degree);
    int mid = (degree - 1) / 2;

    int midKey = leftNode->list_of_keys[mid];

    // copy half of left node to right node
    for (int x = 0; x < leftNode->size - mid; x++) {
      rightNode->list_of_keys[x] = leftNode->list_of_keys[x + mid + 1];
      rightNode->size++;

      leftNode->list_of_keys[x + mid + 1] = -1;
      leftNode->size--;
    }

    // Remove midKey from leftNode
    leftNode->list_of_keys[mid] = -1;
    leftNode->size--;

    leftNode->parent = parent;
    rightNode->parent = parent;

    // If passed node (leftNode) is not a leaf, copy the pointers to rightNode
    if (!leftNode->leaf) {
      // If not a leaf, has children, using the middle index, transfer bigger
      // children to rightNode from leftNode
      int x = 0;
      for (int i = mid + 1; i < degree + 1; i++) {
        rightNode->child.at(x) = leftNode->child.at(i);
        rightNode->child.at(x++)->parent = rightNode;
        leftNode->child.at(i) = nullptr;
      }

      // copy half the pointer of the left node to the right node
      /* --------------------- Done Above --------------------- */

      // find the correct position to add the new array within the parent
      // child array, need to add middlekey first because we are using add
      // at leaf.

      if (parent == nullptr) {
        // Create a new parent
        parent = new node(degree);
        // Add leftNode and rightNode to parent
        parent->leaf = false;
        parent->child[0] = leftNode;
        parent->child[1] = rightNode;
        leftNode->parent = parent;
        rightNode->parent = parent;
        parent->parent = nullptr;
        root = parent;
      }

      addchildToParent(parent, leftNode, rightNode, midKey);
      if (parent->size == degree) splitChild(parent->parent, parent);

    }

    else if (leftNode->leaf) {
      addchildToParent(parent, leftNode, rightNode, midKey);
      if (parent->size == degree) splitChild(parent->parent, parent);
    }
  }
};
