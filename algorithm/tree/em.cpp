//450 

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
 public:
  TreeNode* deleteNode(TreeNode* root, int key) {
    return help(root, key, nullptr);
  }

    TreeNode* findmax(TreeNode* root){
        while(root->right!=nullptr){
            root = root -> right;
        }
        return root;
    }

  TreeNode* help(TreeNode* root, int key, TreeNode* parent) {
    // root is null
    if (root == nullptr) {
      return nullptr;
    }

    // find the targeted node
    if (key < root->val) {
      help(root->left, key, root);
    } else if (key > root->val) {
      help(root->right, key, root);
    } else {
      // we find the root->val == key
      // [1]
      // case1 leaf node
      if (root->left == nullptr && root->right == nullptr) {
        if (parent == nullptr) {
          delete (root);
          return nullptr;
        } else {
          if (root->val > parent->val) {
            // rchild
            parent->right = nullptr;
          } else {
            parent->left = nullptr;
          }
          delete (root);
        }
      } else if (root->left == nullptr || root->right == nullptr) {
        // case2 one child
        if (parent == nullptr) {
          if (root->left) {
            return root->left;
          } else {
            return root->right;
          }
          delete (root);
        } else {
          if (root->val > parent->val) {
            // rchild
            if (root->right != nullptr) {
              parent->right = root->right;
            }
            if (root->left != nullptr) {
              parent->right = root->left;
            }
            delete (root);
          } else {
            // lchild
            if (root->right != nullptr) {
              parent->left = root->right;
            }
            if (root->left != nullptr) {
              parent->left = root->left;
            }
            delete (root);
          }
        }

      } else {
        // case3 two children
        TreeNode* leftmaxNode = findmax(root->left);
        //replace the current
        int leftmaxValue = leftmaxNode->val;
                
        //clean the left tree based on existing function
        TreeNode* leftNew = help(root->left, leftmaxValue, root);
                
        //attention, only replace the value here, not the whole node
        root->val = leftmaxValue;
      }
    }

    return root;
  }
};
