/**************************************************************************************************************
 * Author: sourav parmar
 *
 * Advanced data structure : treemap implementation using red black tree
 * *************************************************************************************************************
 * Features supported
 * Increase(theID, m):Increase the count of the event theID by m. IftheID is not present, insert it.
 * Reduce(theID, m):Decrease the count of theID by m. If theID’scount becomes less than or equal to 0,remove theID
 * Count(theID):Print the count of theID.
 * InRange(ID1, ID2):Print the total count for IDs between ID1 and ID2 inclusively
 * Next(theID):Print the ID and the count of the event with the lowest ID that is greater that theID
 * Previous(theID):Print the ID and the count of the event with the greatest key that is less that theID.
 * levelorder: Print the RB tree according to level
 * Running instruction: ./bbst <input_file>
 * command : input from command line:  command <param> .... eg increase 100 5
 **************************************************************************************************************/

#include<iostream>
#include<string>
#include <fstream>
#include<sstream>
#include<queue>
using namespace::std;
/**************************************************************************************************************
 * A red-black tree is a binary search tree where each node has a color attribute, the value of which is either
 * red or black
 *
 *  requirements of a valid  red-black tree:
 *  1) The root is black.
 *  2) All leaves are black.
 *  3) Both children of each red node are black.
 *  4) The paths from each leaf up to the root each contain the same
 *     number of black nodes.
 *
 **************************************************************************************************************/
#define RED 0
#define BLACK 1
struct RBNode{
    int mkey;
    int mvalue;
    RBNode* parent;
    RBNode* left;
    RBNode* right;
    RBNode* successor;
    bool mcolor; // 0 RED 1 BLACK
    RBNode(int key, int value, bool color):mkey(key)
      ,mvalue(value)
      ,mcolor(color)
      ,parent(NULL)
      ,left(NULL)
      ,right(NULL)
      ,successor(NULL)
    {}
    ~RBNode(){}
};
/****************************************************************************************************************
 * senitel nil node is used to represent black null nodes
 * parent of root points to senitel nil node hence avoid check for NULL pointers
 *
 * RB function:
 * buildtree : creates a BST from the sorted input.
 * colortee: converts created BST to redblack with odd level colored as RED (root is at level 0)
 * insert: inserts a node in RB BST
 * insertFixup: maintains RB invariants during insertion
 * deletenode: deletes a node from RB BST.
 * deleteFixup: maintains RB invariants during delete
 *
 * map function:
 * increase: increase the value associated with key, if key is not found insert it in RB BST
 * decrease:  reduce the value associated with key, if value decreased to 0 delete that key from RB BST
 * cout: count of value associated with key.
 * inrange: sum of count of values between given key ranges
 * next:Print the key and the value of the event with the lowest key  that is greater than given key
 * previous:Print the key and the value of the event with the greatest key that is less than given key
 *
 ***************************************************************************************************************/
class treemap{
    RBNode *root;
    RBNode *nil;
    void rotateleft(RBNode* &, RBNode*&);
    void rotateright(RBNode*&, RBNode* &);
    void insertFixup(RBNode* &, RBNode*&);
    void deleteFixup(RBNode*&, RBNode* &);
    RBNode* inserthelper(RBNode*,RBNode*);
    RBNode* successor(RBNode* , RBNode* );
    RBNode* predecessor(RBNode* , RBNode* );
    RBNode* buildhelper(vector<pair<int,int> >&, int start, int end,int level, int &maxlevel);
    void inrangehelper(RBNode*, int , int, int&);
    void levelorder(RBNode*);
    RBNode* findmin(RBNode*);
    RBNode* findmax(RBNode*);
public:
    int buildtree(vector<pair<int,int> >&);
    void increase(int key, int value);
    void decrease(int key, int value);
    void count(int key);
    void inrange(int key1, int key2);
    void next(int key);
    void previous(int key);
    void insert(int key, int value);
    bool findvalidnode(int key, RBNode *root, RBNode* &prev, RBNode* &curr,bool );
    void deletenode(RBNode* &todelete, RBNode* &root, int key);
    void inorder(RBNode*,RBNode*&,int , int maxlevel);
    void deletetree(RBNode* );
    void colortree(int maxlevel);
    RBNode* searchkey(RBNode* root, int key);
    void levelorderprint();
    treemap ():root(NULL){
        nil = new RBNode(-1,-1,BLACK);// senitel nil node
        nil->parent=nil;
        nil->left= nil;
        nil->right=nil;
    };
    inline RBNode* getroot(){return root;}
    inline RBNode* rbnil(){return nil;}
    ~treemap()
    {
        deletetree(this->root);
        delete this->nil;
    }
};
/************************************************************************************************************
 * Destroy tree on exit: called from treemap destructor
 ************************************************************************************************************/
void treemap::deletetree( RBNode *root )
{
    if ( root != rbnil() )
    {
        deletetree( root->left );
        deletetree( root->right );
        delete( root );
    }
}

/****************************************************************************************************************
 * Helper funtion for Red black tree insert.
 * This funtion insert a node in it's appropriate postion in a BST
 * Inserted node is colored red
 * **************************************************************************************************************/
RBNode* treemap::inserthelper(RBNode* root, RBNode* curr) // Binary search tree insert
{
    if(root == NULL || root == rbnil())
        return curr;
    if(root->mkey > curr->mkey)
    {
        root->left = inserthelper(root->left,curr);
        root->left->parent = root;
    }
    else if(root->mkey < curr->mkey) // insert at right
    {
        root->right = inserthelper(root->right, curr);
        root->right->parent = root;
    }
    else // value equal just increment count
        root->mvalue += curr->mvalue;
    return root;
}
/***********************************************************************************************
 * rotate left: Rotates the treenode in anti clockwise direction with respect to current node
************************************************************************************************/
void treemap::rotateleft(RBNode*& root, RBNode*& curr)
{
    RBNode* currright = curr->right;
    curr->right = currright->left;
    if(curr->right != rbnil())curr->right->parent = curr;
    currright->parent = curr->parent;
    if(curr->parent == rbnil()) // curr is root node
        root = currright;
    else if(curr->parent->left == curr)// current is left child
        curr->parent->left = currright;
    else
        curr->parent->right = currright; // curr is right child, curr parent right now point to currright
    currright->left = curr;
    curr->parent = currright;

}

/***********************************************************************************************
 * rotate right: Rotates the treenode in  clockwise direction with respect to current node
************************************************************************************************/
void treemap::rotateright(RBNode*& root, RBNode*& curr)
{
    RBNode* currleft = curr->left;
    curr->left = currleft->right;
    if(curr->left != rbnil())curr->left->parent = curr;
    currleft->parent = curr->parent;
    if(curr->parent == rbnil()) // curr is root node
        root = currleft;
    else if(curr->parent->left == curr)// current is left child
        curr->parent->left = currleft;
    else
        curr->parent->right = currleft; // curr is right child, curr parent right now point to currleft
    currleft->right = curr;
    curr->parent = currleft;

}
/* **********************************************************************************************************************
*          THis function restores RB tree invarients during insert
*           case 1:uncle of curr is red
*           case 2:uncle of curr is black and curr is right child of parent: simply leftrotate and transform to case 3
*           case 3:uncle of curr is black and curr is left child of parent:
                    change color of parent as black
                    change color of grandparent as red
                    rightrotate along grandparent
*************************************************************************************************************************/
void treemap::insertFixup(RBNode*& root, RBNode*& curr)
{
    while(curr != root &&  curr->parent->mcolor == RED) // loop till parent is black
    {
        RBNode* parent = curr->parent;
        RBNode* g_parent = parent->parent;
        RBNode* uncle = NULL;
        if(g_parent->left == parent) // parent is left child
        {
            uncle = g_parent->right;
            //case 1 uncle of curr is red
            if(uncle->mcolor == RED)// no need for null check as senitel node is nil which is always black
            {
                g_parent->mcolor = RED;
                uncle->mcolor = BLACK;
                parent->mcolor = BLACK;
                curr = g_parent;
            }
            else
            {
                if(curr == parent->right) // case 2:Transform to case 3
                {
                    rotateleft(root,parent);
                    curr = parent; // no need to adjust parent field as it is already done in rotateleft
                    parent = curr->parent;
                }
                parent->mcolor = BLACK;
                g_parent->mcolor = RED;
                rotateright(root, g_parent);
            }
        }
        else{ // symmetic case parent is right child
            uncle  = g_parent->left;
            if(uncle && uncle->mcolor == RED)
            {
                g_parent->mcolor = RED;
                uncle->mcolor = BLACK;
                parent->mcolor = BLACK;
                curr = g_parent;
            }
            else
            {
                if(curr == parent->left) // case 2:Transform to case 3
                {
                    rotateright(root,parent);
                    curr = parent; // no need to adjust parent field as it is already done in rotateleft
                    parent = curr->parent;
                }
                parent->mcolor = BLACK;
                g_parent->mcolor = RED;
                rotateleft(root, g_parent); // parent is now black loop invarient are satisfied
            }
        }
    }
    root->mcolor = BLACK; // property 2 of RBT i.e root shall be black.
}
/************************************************************************************************************
 * This function inserts a node in RBTree
 * Firstly, call inserthelper to isert node in appropriate postion in BST
 * secondly,calls insertFixup to resolve violation of RB tree invariants
 * **********************************************************************************************************/
void treemap::insert(int key, int value)
{
    RBNode * node = new RBNode(key, value,RED); // inserted node red in color
    node->left = rbnil(); // left  points to senitel nil
    node->right= rbnil(); // right  points to senitel nil
    root = inserthelper(root,node);
    root->parent = rbnil();// parent of root points to senitel nil
    insertFixup(root, node);
}
/*************************************************************************************************************
 * Helper function to compare two string independent of case
 * Used to parse commands
 * ***********************************************************************************************************/

bool strequal(const string & first ,const string& second)
{
    unsigned int len= first.size();
    if(len != second.size() )
        return false;
    for(int i = 0 ; i <len;++i)
    {
        if(tolower(first[i])!= tolower(second[i]))
            return false;
    }
    return true;
}
/***********************************************************************************************************
  * Utility funtion :color last level node as RED if it is not root
  **********************************************************************************************************/
void treemap::inorder(RBNode * root,RBNode*& prev,int level, int maxlevel)
{
    if(root == NULL || root == rbnil()) return;
    inorder(root->left,prev,level+1, maxlevel);
    //cout<< "key "<<root->mkey <<" color "<<root->mcolor<<endl;
    if(prev) prev->successor = root;
    prev = root;
    if(level && level == maxlevel) root->mcolor = RED;// color last level as RED, if maxlevel is 0 don't recolor
    inorder(root->right,prev,level+1 ,maxlevel);
}
/***********************************************************************************************************
  *convert created BST to RB BST: calls inorder to color
  **********************************************************************************************************/
void treemap::colortree(int maxlevel)
{
    RBNode* prev = NULL;
    inorder(root,prev,0, maxlevel);
}
/*****************************************************************************************************************
 * Utility function for Debug: level order travesal of a RBTree
 * **************************************************************************************************************/
void treemap::levelorder(RBNode* root)
{
    if(root == NULL ||  root == rbnil())
        return;
    queue<RBNode*>q;
    q.push(root);
    while(1)
    {
        int size = q.size();
        if(size == 0)
            break;
        RBNode* temp = NULL;
        while(size)
        {
            temp = q.front();
            q.pop();
            cout<<" key " <<temp->mkey<<" color "<<temp->mcolor<<" parent ";
            if(temp->parent) cout<<temp->parent->mkey<<" ";
            cout<<endl;
            if(temp->left !=rbnil())q.push(temp->left);
            if(temp->right!=rbnil())q.push(temp->right);
            size--;
        }
        cout<<"-----------Next Level-----------"<<endl;
    }
}

void treemap::levelorderprint()
{
    levelorder(root);
}
/******************************************************************************************************
 * Utility function: Returns Node if the search key matches any node in RB BST else returns NULL
 ******************************************************************************************************/
RBNode* treemap::searchkey(RBNode* root, int key)
{
    if(root == NULL || root->mkey == key)
        return root;
    if(root == rbnil()) return NULL;
    if(root->mkey > key ) return searchkey(root->left,key );
    if(root->mkey < key ) return searchkey(root->right,key);
}
/******************************************************************************************************
 * Utility function for next and previous methods:
 * sets curr node which is used by next and previous method
 * curr equals root if root key matches search key
 * if key is in range (prev.key,root,key) then
 *       curr equals previous observed inorder node if findsucc flag is true
 *       curr equals root if findsucc flag is true
 *
 * findsucc : used to differentiate call from next or previos method
 ******************************************************************************************************/
bool treemap::findvalidnode(int key, RBNode *root, RBNode* &prev, RBNode* &curr, bool findsucc)
{
    if(root == NULL || root == rbnil())
    {
        return false;
    }
    if(root->mkey == key)
    {
        curr = root; // if node is found in tree update curr as root and return
        return true;
    }
    if(key < root->mkey && findvalidnode(key, root->left,prev,curr ,findsucc))
        return true; // early exit if validnode found
    if((prev && (prev->mkey < key && root->mkey > key)))
    {
        if(findsucc)
            curr = prev; // if finding next update curr as previous observed inoder node
        else
            curr = root; // if finding previous update curr as root
        //cout<<prev->mkey<< " "<<key<<" "<<root->mkey<<" "<<endl;
        return true;
    }
    prev = root;

    if(key > root->mkey) findvalidnode(key, root->right,prev,curr,findsucc);
}
/******************************************************************************************************
 * Utility function: Returns minimum node in BST
 ******************************************************************************************************/
RBNode* treemap::findmin(RBNode* root)
{
    while(root->left!=rbnil())
        root = root->left;
    return root;
}
/******************************************************************************************************
 * Utility function: Returns maximum node in BST
 ******************************************************************************************************/
RBNode* treemap::findmax(RBNode* root)
{
    while(root->right!=rbnil())
        root = root->right;
    return root;
}
/******************************************************************************************************
 * Utility function: Returns inorder succesor of a node in BST
 * if rightsubtee exist, inorder successor is minimum value on right subtree
 * else inorder successor the parentof node when current node is leftchild of its parent
 ******************************************************************************************************/
RBNode* treemap::successor(RBNode* curr, RBNode* root)
{
    if(curr->right != rbnil())
    {
        curr = curr->right;
        while(curr->left != rbnil())
        {
            curr = curr->left;
        }
        return curr;
    }
    RBNode* p_curr = curr->parent;
    while(p_curr!=rbnil() && curr == p_curr->right)
    {
        curr = p_curr;
        p_curr = curr->parent;
    }
    return p_curr;
}
/******************************************************************************************************
 * Utility function: Returns inorder predecessor  of a node in BST
 ******************************************************************************************************/
RBNode* treemap::predecessor(RBNode* curr, RBNode* root)
{
    if(curr->left !=rbnil())
    {
        curr = curr->left;
        while(curr->right !=rbnil())
        {
            curr = curr->right;
        }
        return curr;
    }
    RBNode* p_curr = curr->parent;
    while(p_curr!=rbnil() && curr != p_curr->right)
    {
        curr = p_curr;
        p_curr = curr->parent;
    }
    return p_curr;
}
/****************************************************************************************************************
 * Maintains RB invariant while delete
 * case A when current node is left child of parent
 * case 1:curr node sibling is red: leftrotate along parent and change sibling color to black and parent to red
 * case 2: curr node sibling is black and both child of sibling black : change color of sibling to red and curr
 *         points to parent
 * case 3: curr node sibling is black, sibling left child is red and sibling right child is black:
 *         swtich color of sibling and it's left child and rotateright along sibling, new sibling is now leftchild
 *         sibling
 * case 4: curr node sibling is black, sibling right child is red
 *         color sibling as color of parent
 *         color parent of curr as black
 *         color sibling right child as black
 *         leftrotate along parent
 *         since property restored in this set curr as root to terminate loop
 *
 * case B when current node is right child of its parent
 *        this is symmetric to case A subscases.
 ****************************************************************************************************************/
void treemap::deleteFixup(RBNode*& root,RBNode*& curr)
{
    while( curr!= root &&  curr->mcolor == BLACK)
    {
        RBNode* sibling = NULL;
        if(curr == curr->parent->left)
        {
            sibling = curr->parent->right;
            if(sibling->mcolor == RED) // case 1
            {
                //cout<<"case 1"<<endl;
                sibling->mcolor = BLACK;
                curr->parent->mcolor = RED;
                rotateleft(root,curr->parent);
                sibling = curr->parent->right;
            }
            if(sibling->left->mcolor == BLACK && sibling->right->mcolor == BLACK)//case 2
            {
                //cout<<"case 2"<<endl;
                sibling->mcolor = RED;
                curr = curr->parent;
                //cout<<"curr after case 2 "<<curr->mkey << " "<<curr->parent->mkey<<endl;
            }
            else
            {
                //cout<<"case 3"<<endl;
                if(sibling->right->mcolor == BLACK)//case 3
                {
                    sibling->left->mcolor = BLACK;
                    sibling->mcolor = RED;
                    rotateright(root,sibling);
                    sibling = curr->parent->right;
                }
                //cout<<"case 4"<<endl;
                sibling->mcolor = curr->parent->mcolor;//case 4
                curr->parent->mcolor = BLACK;
                sibling->right->mcolor = BLACK;
                rotateleft(root,curr->parent);
                curr = root;
            }
        }
        else // symmetric case if current is right child of its parent, symmetric to left case
        {
            sibling = curr->parent->left;
            if(sibling->mcolor == RED)
            {
                //cout<<"case 5"<<endl;
                sibling->mcolor = BLACK;
                curr->parent->mcolor = RED;
                rotateright(root,curr->parent);
                sibling = curr->parent->left;
            }
            if(sibling->right->mcolor == BLACK && sibling->left->mcolor == BLACK)
            {
                //cout<<"case 6"<<endl;
                sibling->mcolor = RED;
                curr = curr->parent;
            }
            else
            {
                if(sibling->left->mcolor == BLACK)
                {
                    // cout<<"case 7"<<endl;
                    sibling->right->mcolor = BLACK;
                    sibling->mcolor = RED;
                    rotateleft(root,sibling);
                    sibling = curr->parent->left;
                }
                // cout<<"case 8"<<endl;
                sibling->mcolor = curr->parent->mcolor;
                curr->parent->mcolor = BLACK;
                sibling->left->mcolor = BLACK;
                rotateright(root,curr->parent);
                curr = root;
            }
        }
    }
    //cout<<" rootafter fixup  "<<root->mkey<<endl;
    curr->mcolor = BLACK;
}
/*********************************************************************************************************
 * delete node from RB BST
 * if deleted node is red no heigt changes
 * if delted node is BLACK calls deleteFixup to preserve RB invariants as black height is decreased after
 *          deletion
 *******************************************************************************************************/
void treemap::deletenode(RBNode* &todelete, RBNode* &root, int key)
{
    if(todelete == NULL)
    {
        // cout<<"Error: Node not found"<<endl;
        return;
    }
    // cout<<" deletenode enter"<<endl;
    RBNode* del = rbnil();
    //cout<<"todelete "<<todelete->mkey<< "left "<<todelete->left->mkey<<"right "<<todelete->right->mkey<<endl;
    if(todelete->left == rbnil() || todelete->right == rbnil())
        del = todelete;
    else
        del = successor(todelete, root);
    //cout<<"todelete "<<todelete->mkey<< "left "<<todelete->left->mkey<<"right "<<todelete->right->mkey<<" del "<<del->mkey<<endl;
    RBNode* child = del->left == rbnil()?del->right : del->left;
    child->parent = del->parent;
    if(del->parent == rbnil()) // node to be deleted is root
        root = child;
    //attach child at appropriate postion
    if(del->parent->left == del)
        del->parent->left = child;
    else
        del->parent->right = child;
    //cout<<"before fixup"<<endl;
    if(todelete != del)
    {
        todelete->mkey = del->mkey;
        todelete->mvalue = del->mvalue;
    }
    if(del->mcolor == BLACK) // call fixup only when deleted node is black as it will violate black node count invarient
    {
        //cout<<"deleteFixup call"<< child->mkey<< " "<<endl;
        deleteFixup(root,child);
    }
    delete del;
}

/*********************************************************************************************************************
 * Utility function: decrease count associated with a key
 * if count drops to zero calls deltenode procedure to remove node from RB BST
 * Prints updated count of node with key
 *********************************************************************************************************************/
void treemap:: decrease(int key, int value)
{
    RBNode* todecrease = searchkey(root,key);
    if(todecrease == NULL)// no need to handle for rbnil() as search will return null for nil node
    {
        cout<<"0"<<endl;
        return;
    }
    todecrease->mvalue -= value;
    if( todecrease->mvalue <= 0)
    {
        deletenode(todecrease, root, key);
        cout<<"0"<<endl;
    }
    else
        cout<< todecrease->mvalue<<endl;
}

/*********************************************************************************************************************
 * Utility function: Increase count associated with a key
 * if key not found calls insert procedure to insert node into RB BST
 * Prints updated count of node with key
 *********************************************************************************************************************/
void treemap::increase(int key, int value){
    RBNode* toincrease = searchkey(root,key);
    if(toincrease == NULL) // no need to handle for rbnil() as search will return null for nil node
    {
        insert(key, value);
        cout<<value<<endl;
    }
    else
    {
        toincrease->mvalue += value;
        cout<<toincrease->mvalue<<endl;
    }
}
/*********************************************************************************************************************
 * Utility function: Print count associated with a key
 * when key not found prints 0
 *********************************************************************************************************************/
void treemap::count(int key)
{
    RBNode* curr = searchkey(root,key);
    if(curr)
        cout<< curr->mvalue <<endl;
    else
        cout<<"0"<<endl;
}
/*********************************************************************************************************************
 * Utility function: print  the lowest key that is greater than search key and associated count.
 * if none exist print " 0 0"
 *********************************************************************************************************************/
void treemap::next(int key)
{
    RBNode* minimum = findmin(root);
    if(key < minimum->mkey) // if key is less than minimum of tree the minimum key is succesor
    {
        cout<<minimum->mkey<<" "<<minimum->mvalue<<endl;
        return;
    }
    RBNode* curr = NULL;
    RBNode* prev = NULL;

    findvalidnode(key, root, prev , curr,true);
    RBNode* succ = NULL;
    if(curr) succ = successor(curr,root); // find inorder successor
    if(succ && succ!=rbnil()) cout<<succ->mkey<<" "<<succ->mvalue<<endl;
    else
        cout<<"0 0"<<endl;
}
/*********************************************************************************************************************
 * Utility function: print  the greatest key that is smaller than search key and associated count.
 * if none exist print " 0 0"
 *********************************************************************************************************************/
void treemap::previous(int key)
{
    RBNode* maximum = findmax(root);
    if(key > maximum->mkey)// if key is greater than maximum of tree the maximum key is predecessor
    {
        cout<<maximum->mkey<<" "<<maximum->mvalue<<endl;
        return;
    }
    RBNode* curr = NULL;
    RBNode* prev = NULL;
    findvalidnode(key, root, prev , curr,false);
    RBNode* pre = NULL;
    if(curr) pre = predecessor(curr,root);// find inorder predecessor
    if(pre && pre!=rbnil()) cout<<pre->mkey<<" "<<pre->mvalue<<endl;
    else
        cout<<"0 0"<<endl;
}
/*********************************************************************************************************************
 * Utility function: compute sum of count within given key ranges [key1,key2].
 *********************************************************************************************************************/
void treemap::inrangehelper(RBNode* root, int key1, int key2,int& count)
{
    if(root == NULL || root == rbnil())
        return;
    if(key1 < root->mkey)
        inrangehelper(root->left, key1,key2, count);
    if((root->mkey >= key1) && (root->mkey <= key2))
    {
       // cout<<root->mkey<<" "<<root->mvalue<<endl;
        count += root->mvalue; // if node value is within given range update count
    }
    if(key2 > root->mkey)
        inrangehelper(root->right,key1,key2, count);
}
/*********************************************************************************************************************
 * Utility function: print  the sum of count within given key ranges [key1,key2].
 * calls inrangehelperto find sum
 *********************************************************************************************************************/
void treemap::inrange(int key1, int key2)
{
    if(key2 <key1)
    {
       cout<<"Error! key1 shall be less than key2 "<<endl;
       return;
    }
    int count = 0 ;
    inrangehelper(root, key1, key2,count);
    cout<<count<<endl;
}
/*********************************************************************************************************************
 * function: Build BST from input vector.
 * senitel nil is used for NULL
 *********************************************************************************************************************/
RBNode* treemap::buildhelper(vector<pair<int,int> > &inp,  int begin, int end ,int level , int &maxlevel)
{
    if(begin > end) return rbnil();
    int mid = begin+ (end - begin)/2;
    RBNode* newnode = new RBNode(inp[mid].first, inp[mid].second,BLACK);//make tree  even nodes black
    maxlevel = max(level,maxlevel);
    newnode->left = buildhelper(inp,begin,mid-1,level+1, maxlevel);
    if(newnode->left) newnode->left->parent = newnode;
    newnode->right = buildhelper(inp, mid+1, end,level+1, maxlevel);
    if(newnode->right) newnode->right->parent = newnode;
    return newnode;
}

int treemap::buildtree(vector<pair<int,int> > &inp)
{
    int size = inp.size();
    int maxlevel = 0 ;
    root = buildhelper(inp,0,size-1,0, maxlevel);
    root->parent = rbnil();// root parent is senitel
    return maxlevel;
}
bool validate(string command)
{
    for(int i = 0 ; i <command.size();++i)
    {
        if(command[i] == ' ')
            continue;
        if(tolower(command[i]) <'a' || tolower(command[i]) >'z')
            return false;
    }
    return true;
}

int main(int argc, char* argv[]){
    treemap mytree;
    long nelem; //first param of line
    string temp;
    ifstream instream;
    instream.exceptions ( ifstream::failbit | ifstream::badbit );
    cout<<" input file " << argv[1]<<endl;
    {
        vector<pair<int,int> > treevec;
        try
        {
            instream.open(argv[1]);
            getline(instream,temp);
            stringstream s_nelem(temp);
            s_nelem >> nelem;
            string key, value;
            int ikey, ivalue;
            cout<<" nelem "<<nelem<<endl;
            while(getline(instream, key, ' '))
            {
                getline(instream, value);
                istringstream s_key(key);
                istringstream s_value(value);
                s_key >> ikey;
                s_value>> ivalue;
                treevec.push_back(make_pair(ikey,ivalue));
                //mytree.insert(ikey, ivalue);// insert key value pair in treemap
            }
        }
        catch (ifstream::failure e) {
            if(!instream.eof())
                std::cout << "Exception opening/reading file ! Wrong file name\n";
        }

        std::cout.flush();
        instream.close();
        //cout<<"before build "<<endl;
        int maxlevel = mytree.buildtree(treevec);
        cout<<"maxlevel "<< maxlevel<<endl;
        mytree.colortree(maxlevel);
    }
    cout<<" Tree built "<<endl;
    std::cout<<" ______________________________________________________________"<<std::endl;
    std::cout<<"| map created, enter commands in specified format              |"<<std::endl;
    std::cout<<"|______________________________________________________________|"<<std::endl;
    std::cout<<"|* command: increase  | format increase <id_INT> <count_INT>   |"<<std::endl;
    std::cout<<"|* command: reduce    | format reduce   <id_INT> <count_INT>   |"<<std::endl;
    std::cout<<"|* command: count     | format count    <id_INT>               |"<<std::endl;
    std::cout<<"|* command: inRange   | format inrange  <id_INT> <id_INT>      |"<<std::endl;
    std::cout<<"|* command: next      | format next     <id_INT>               |"<<std::endl;
    std::cout<<"|* command: previous  | format previous <id_INT>               |"<<std::endl;
    std::cout<<"|* command: levelorder| format levelorder                      |"<<std::endl;
    std::cout<<"|* command: quit      | format quit                            |"<<std::endl;
    std::cout<<"|______________________________________________________________|"<<std::endl;
    while(1)
    {   string inp;
        //istringstream instream(std::cin);
        getline(std::cin, inp);

        stringstream s_command(inp);
        string command;
        s_command>> command;
        if(validate(command) == false)
        {
            cout<<" Error ! command should be string"<<endl;
            continue;
        }
        if(strequal(command, "quit"))
            break; // quit comand issue exit from loop

        int param1;
        int param2;
        if(strequal(command,"increase"))
        {
            s_command >> param1;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param1 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            s_command >> param2;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param 2 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            if(param2 <= 0) { cout<<"Not a valid input param 2 try again with value greater than 0"<<endl;continue;}
            mytree.increase(param1,param2);
        }
        else if(strequal(command,"reduce"))
        {
            s_command >> param1;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param1 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            s_command >> param2;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param 2 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }

            mytree.decrease(param1,param2);
        }
        else if(strequal(command ,"count"))
        {
            s_command >> param1;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param1 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            mytree.count(param1);
        }
        else if(strequal(command,"next"))
        {
            s_command >> param1;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param1 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            mytree.next(param1);
        }
        else if(strequal(command,"previous"))
        {
            s_command >> param1;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param1 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            mytree.previous(param1);
        }
        else if(strequal(command,"inrange"))
        {
            s_command >> param1;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error ! Param1 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            s_command >> param2;
            if(s_command.fail()) // invalid input, expected is int
            {
                s_command.clear();
                cout<<"Error !Param 2 should be a integer value "<<endl;
                continue;// since stream is reset at every iteration no need to ignore
            }
            mytree.inrange(param1,param2);
        }
        else if(strequal(command,"levelorder"))
        {
            mytree.levelorderprint();
        }
        else
        {
            std::cout<<"Error ! Wrong command or command format | enter commands in following format"<<endl;
            std::cout<<" ______________________________________________________________"<<std::endl;
            std::cout<<"| map created, enter commands in specified format              |"<<std::endl;
            std::cout<<"|______________________________________________________________|"<<std::endl;
            std::cout<<"|* command: increase  | format increase <id_INT> <count_INT>   |"<<std::endl;
            std::cout<<"|* command: reduce    | format reduce   <id_INT> <count_INT>   |"<<std::endl;
            std::cout<<"|* command: count     | format count    <id_INT>               |"<<std::endl;
            std::cout<<"|* command: inRange   | format inrange  <id_INT> <id_INT>      |"<<std::endl;
            std::cout<<"|* command: next      | format next     <id_INT>               |"<<std::endl;
            std::cout<<"|* command: previous  | format previous <id_INT>               |"<<std::endl;
            std::cout<<"|* command: levelorder| format levelorder                      |"<<std::endl;
            std::cout<<"|* command: quit      | format quit                            |"<<std::endl;
            std::cout<<"|______________________________________________________________|"<<std::endl;
        }
    }
    return 0;
}
