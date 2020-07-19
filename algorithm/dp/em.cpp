
//53. Maximum Subarray
//by sequential search based on the fact
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int curSum = 0;
        int maxSum = nums[0];
        for(int i=0;i<nums.size();i++){
            curSum=curSum+nums[i];
            if(nums[i]>curSum){
                curSum=nums[i];
            }
            maxSum = max(curSum,maxSum);
        }
        return maxSum;
    }
};

//53. Maximum Subarray
//by divide and conqure
class Solution {
 public:
  int helper(int l, int r, vector<int>& nums) {
    //std::cout << "l " << l << " r " << r << std::endl;
    // edge case
    if (l == r) {
      return nums[l];
    }

    int mid = (l + r) / 2;
    // mid in solution
    int maxmidSum = nums[mid];
    int tempSum = maxmidSum;

    // range left
    if (mid - 1 >= l) {
      for (int i = mid - 1; i >= l; i--) {
        tempSum = tempSum + nums[i];
        maxmidSum = max(tempSum, maxmidSum);
      }
    }

    // range right
    if (mid + 1 <= r) {
      tempSum = maxmidSum;
      for (int i = mid + 1; i <= r; i++) {
        tempSum = tempSum + nums[i];
        maxmidSum = max(tempSum, maxmidSum);
      }
    }

    // mid element not in solution
    int maxLspan = INT_MIN, maxRspan = INT_MIN;
    if (mid - 1 >= l) {
      maxLspan = helper(l, mid - 1, nums);
    }
    if (mid + 1 <= r) {
      maxRspan = helper(mid + 1, r, nums);
    }

    return max(max(maxLspan, maxRspan), maxmidSum);
  }

  int maxSubArray(vector<int>& nums) {
    int size = nums.size();
    if (size == 1) {
      return nums[0];
    }

    return helper(0, size - 1, nums);
  }
};


//62. Unique Paths
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<vector<int>> dp (m, vector<int>(n,0));
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                if(i==0 || j==0){
                    dp[i][j]=1;
                    continue;
                }
                dp[i][j]=dp[i-1][j]+dp[i][j-1];
            }
        }
        return dp[m-1][n-1];    
    }
};

//85. Maximal Rectangle
//there are many problem for the second parts

class Solution {
public:
    int maximalRectangle(vector<vector<char>>& matrix) {
        //get the r and c number
        int r = matrix.size();
        if(r==0){
            return 0;
        }
        int c = matrix[0].size();
        
        //caculte the continus column with 1 in it
        vector<vector<int>> conOne (r, vector<int>(c,0));
        
        for(int i=0;i<r;i++){
            for(int j=0;j<c;j++){
                if(matrix[i][j]=='0'){
                    continue;
                }else{
                    if(j==0){
                         //edge case, there is no left element
                         conOne[i][j]=1;
                    }else{
                        conOne[i][j]= conOne[i][j-1]+1;
                    }
                }

            }
        }
        
        //caculate the area level by level
        int minLen;
        int maxArea=0;
        for(int i=0;i<r;i++){
            for(int j=0;j<c;j++){
                if(conOne[i][j]==0){
                    continue;
                }
                //init the value at the beginning of the loop
                minLen = conOne[i][j];
                maxArea = max(maxArea,minLen);
                for(int k=i-1;k>=0 && conOne[k][j] != 0;--k){
  
                    //store the min
                    minLen=min(minLen,conOne[k][j]);
                    maxArea=max(maxArea,minLen*(i-k+1));
                }
            }
            
        }
        return maxArea;
    }
};

//198 House Robber
class Solution {
public:
    int rob(vector<int>& nums) {
        int size = nums.size();
        if(size==0){
            return 0;
        }
        if(size==1){
            return nums[0];
        }
        vector<int> dp(size+1,0);
        dp[1] = nums[0];
        for(int i=2;i<=size;i++){
            //dp[i] coresponds the nums[i-1]
            dp[i]=max(dp[i-1],nums[i-1]+dp[i-2]);
        }
        return dp[size];   
    }
};

// 279. Perfect Squares
// the memorization plus the iteration could save the time
// only some of the elements need to be evaluated
// the idea here is a little bit similar to the BST
// this method belong to the search method
class Solution
{
  public:
    /**
     * @param n: a positive integer
     * @return: An integer
     */
    int search(vector<int> &dp, int n)
    {
        if (n == 0)
        {
            return 0;
        }

        if (n == 1)
        {
            return 1;
        }

        if (n == 2)
        {
            return 2;
        }

        int minv = INT_MAX;
        if (dp[n] != 0)
        {
            return dp[n];
        }
        else
        {
            //search
            int i = 0;
            int sqrtn = sqrt(n);
            int temp;
            for (i = 1; i <= sqrtn; i++)
            {
                //the largest possible number is sqrtn
                temp = search(dp, n - pow(i,2));
                minv = min(minv, temp);
            }
            dp[n] = minv+1;
            return dp[n];
        }
    }

    int numSquares(int n)
    {
        // write your code here
        vector<int> dp(n + 1, 0);

        int num = search(dp, n);

        return num;
    }
};

// 139. Word Break
class Solution {
 public:
  bool wordBreak(string s, vector<string>& wordDict) {
    unordered_set<string> wordSet(wordDict.cbegin(),wordDict.cend());
    //for (int i = 0; i < wordDict.size(); i++) {
    //  wordSet.insert(wordDict[i]);
   // }
      
    return search(s, wordSet);
  }

  bool search(string s, unordered_set<string>& wordSet) {
    // return condition
    if (status.count(s) > 0) {
      return status[s];
    }

    if (wordSet.count(s) > 0) {
      // update the dpstatus
      return status[s] = true;
    }

    // rnage the string
    // check the complete string
    if (wordSet.find(s) != wordSet.end()) {
      status[s] = true;
      return true;
    }
    string l,r;
    bool ifL,ifR;
    for (int i = 1; i < s.size(); i++) {
      // the length is i
      l = s.substr(0, i);
      r = s.substr(i);
      ifL = (wordSet.find(l) != wordSet.end());
      if (ifL) {
        status[s] = true;
      }
      ifR = search(r, wordSet);
      if (ifL && ifR) {
        return true;
      }
    }

    // fail for all positions
    // No solution for s, memorize and return
    // it is important to remember this
      return status[s] = false;
  }

 private:
  unordered_map<string, bool> status;
};

//300. Longest Increasing Subsequence
//O(n^2)
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        int size = nums.size();
        if(size==0){
            return 0;
        }
        if(size==1){
            return 1;
        }
        int rev = 0;
        //the value of the lis is 1 at least
        vector<int> dp(size,1);
        dp[0]=1;
        for(int i=1;i<size;i++){
            for(int j=0;j<=i-1;j++){
              //it is important to realize this fact
                if(nums[i]>nums[j]){
                    dp[i]=max(dp[i],dp[j]+1);    
                }
                
            }
            rev = max(rev,dp[i]);
        }
        return rev;
        
    }
};

//300. Longest Increasing Subsequence
//O(NlogN) patient sort
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        //edge case
        int size = nums.size();
        if(size==0){
            return 0;
        }
        if(size==1){
            return 1;
        }
        
        //update piles
        vector<int> dp;
        dp.push_back(nums[0]);
        for(int i=1;i<size;i++){
            int L = 0;
            //Attention, the index of the R is the current length of the dp
            int R = dp.size();
            //I'm still confused about this process, why the following operation of the binary search 
            //can find out the suitable value
            //the length of the dp array is changing for every iteration
            //maybe just consider the last three elements and to see how the following algorithm works
            while(L<R){
                //find elements
                int mid = (R+L)/2;
                if(dp[mid]<nums[i]){
                    L = mid+1;
                }else{
                    R = mid;
                }
            }
            //if not find required elements
            //all elements are less than the nums[i]

            //case 1, R not change for all checking, L is larger than R to break,
            //all elements are smaller than targeted value

            //case 2, R is changed, there are case that element is larger than targeted value
            //when L is larger than R, the current value of R is the first value than targeted elements

            if(R>=dp.size()){
                dp.push_back(nums[i]);
            }else{
                dp[R] = nums[i];
            }
        }
        return dp.size();
    }
};

//674. Longest Continuous Increasing Subsequence
//easy case, maybe two pointer thoughts, when to come back to initial status
class Solution {
public:
    int findLengthOfLCIS(vector<int>& nums) {
        int size = nums.size();
        //edge case
        if(size==0){
            return 0;
        }
        if(size==1){
            return 1;
        }
        
        int maxLen = 1;
        int tempLen = 1;
        for(int i=1;i<=size-1;i++){
            if(nums[i]>nums[i-1]){
                ++tempLen;
            }else{
                tempLen=1;
            }
            //keep the max value
            maxLen = max(maxLen,tempLen);
        }
        return maxLen;
    }
};

//96. Unique Binary Search Trees
class Solution {
public:
    int numTrees(int n) {
        if(n==0){
            return 0;
        }
        
        if(n==1){
            return 1;
        }
        
        if(n==2){
            return 2;
        }
        
        vector<int> dp (n+1,0);
        dp[0]=1;
        dp[1]=1;
        dp[2]=2;
        for(int i=3;i<=n;i++){
            for(int j=0;j<=i-1;j++){
                dp[i]=dp[i]+(dp[j]*dp[i-j-1]);
            }
        }
        return dp[n];
    }
};


//131
// the code come from the https://zxi.mytechroad.com/blog/searching/leetcode-131-palindrome-partitioning/
// I'm still not understand it
class Solution {
public:
    vector<vector<string>> partition(string s) {
        
        //init records
        int size = s.size();
        vector<vector<vector<string>>> dp(size + 1); 
        
        
        for(int len =1; len <=size; len++){
            for(int i=0; i<len;i++){
                string right = s.substr(i,len-i);
                if(!isPlin(right)) continue;
                if(i==0){
                    dp[len].push_back({right});
                }
                for (const auto& p : dp[i]) {
                    dp[len].push_back(p);
                    dp[len].back().push_back(right);
                } 
            }
        }
        return dp[size];
    }
        

    
    bool isPlin(string s){
        int size = s.size();
        if(size==1){
            return true;
        }
        for(int i=0;i<size/2;i++){
            int j = size-1-i;
            if(s.at(i)!=s.at(j)){
                return false;
            }
        }
        return true;
    }
     
};


//72 edit distance
class Solution {
public:
    int minDistance(string word1, string word2) {
        int r = word1.length();
        int c = word2.length();
        //edge case
        if(r == 0){
            return c;
        }
        if(c == 0){
            return r;
        }
        
        vector<vector<int>> dp (r+1, vector<int>(c+1,0));
        //dp index i,j
        //string index i-1, j-1
        for(int i=0;i<=r;i++){
            for(int j=0;j<=c;j++){
                //init the ghost area
                if(i==0){
                    dp[i][j]=j;
                    continue;
                }
                if(j==0){
                    dp[i][j]=i;
                    continue;
                }
                if(word1[i-1]==word2[j-1]){
                    dp[i][j]=dp[i-1][j-1];
                }else{
                    dp[i][j]=min(min(dp[i][j-1],dp[i-1][j]),dp[i-1][j-1])+1;
                }
            }
        } 
        return dp[r][c];
    }
};


// 1139 . Largest 1-Bordered Square

class Solution {
public:
    int largest1BorderedSquare(vector<vector<int>>& matrix) {
        
        int r = matrix.size();
        int c = matrix[0].size();
        
        vector<vector<int>> cleft (r, vector<int>(c,0));
        vector<vector<int>> ctop  (r, vector<int>(c,0));
        
        for(int i=0;i<r;i++){
            for(int j=0;j<c;j++){
                if(matrix[i][j]==1){
                    if(j==0){
                        cleft[i][j]=1;
                    }else{
                        cleft[i][j]=cleft[i][j-1]+1;
                    }
                    if(i==0){
                        ctop[i][j]=1;
                    }else{
                        ctop[i][j]=ctop[i-1][j]+1;
                    }
                }
            }
        }
        
        //global record
        int maxLen=0;
        int currLen;
        for(int i=0;i<r;i++){
            for(int j=0;j<c;j++) {
                currLen = min(cleft[i][j],ctop[i][j]);
                if(currLen <= maxLen){
                    continue;
                }
                
                while(currLen>maxLen){
                    //check other edge
                    int k=0;
                    for(k=0;k<currLen;k++){
                        if(matrix[i-k][j-currLen+1]==0 || matrix[i-currLen+1][j-k]==0){
                            break;
                        }
                    }
                    //all checking is passed 
                    //std::cout<<i<<","<<j<<","<< currLen << "," << k<<std::endl;

                    if(k==currLen){
                        maxLen=max(maxLen,currLen);
                    }                      
                        currLen--;
                }
            }
        }
        return maxLen*maxLen;
    }
      
};


// 688. Knight Probability in Chessboard
// some test case may beyond the upper bound of the integer, it is better to use the double
class Solution {
public:
    double knightProbability(int N, int K, int r, int c) {
        
        if (K == 0) return 1;
        
        //status
        vector<vector<double>> dp (N, vector<double>(N, 0));
        vector<vector<double>> dpNew (N, vector<double>(N, 0));
        vector<vector<int>> dist = {{-1,-2},{-2,-1},{-2,1},{-1,2},{1,2},{2,1},{2,-1},{1,-2}};
        
        //k move, k+1 status
        //init status
        dp[r][c] = 1;
        //there some large value that is larger than the int
        double valid=0;
        for(int s=0;s<K;s++){
            valid=0;
            for(int x=0;x<N;x++){
                for(int y=0;y<N;y++){
                    //the data come from the 8 places
                    //must go for the current step
                    dpNew[x][y]=0;
                    //current position come from the 8 neigobors
                    for(auto d : dist){
                        int newx = x+d[0];
                        int newy = y+d[1];
                        if(newx >=0 && newx < N && newy >=0 && newy < N){
                            //the current position come from the neigobor values
                            dpNew[x][y]+=dp[newx][newy];               
                        }
                        
                    }
                    //std::cout<< s << "x " <<x<<" y "<< y << " v " << dpNew[x][y] << std::endl;
                    valid+=dpNew[x][y];  
                }
            }
            //for every step update dp
            dp=dpNew;
        }
        
        return 1.0*valid/pow(8,K);
    }
        
};

//coin change
//attention how to keep status to avoid the time exceeds
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        
        vector<int>dp(amount+1,INT_MAX);
        //init status
        //if 0 amount, do not need any options
        //edge case
        dp[0]=0;        
        return helper(coins,amount,dp);
    }
    
    int helper(vector<int>& coins, int amount, vector<int>& dp){
        if(dp[amount]!=INT_MAX){
            return dp[amount];
        }
        
        for(auto v : coins){
            if(amount>=v){
               int temp = helper(coins,amount-v,dp);
                if (temp==-1){
                    //record status
                    continue;
                }
                if (temp!=INT_MAX && temp!=-1){
                    dp[amount]=min(dp[amount],temp+1); 
                }
            }
        }  
        if(dp[amount]==INT_MAX){
            //record the status
            dp[amount]=-1;
            return -1;
        }else{
           return dp[amount];  
        }
          
    }
};


//lintcode 89 k sum
//attention with the mismatch between the array index and the dp index
class Solution {
public:
    /**
     * @param A: An integer array
     * @param k: A positive integer (k <= length(A))
     * @param target: An integer
     * @return: An integera
     */
    int kSum(vector<int> &A, int k, int target) {
        // write your code here
        int size = A.size();
        
        //special cses
        if(size==0){
            return 0;
        }
        if(k==0){
            if(target == 0){
                return 1;
            }else{
                return 0;
            }
        }
        
        //size + 1 for the ghost area
        int dp[size+1][k+1][target+1];
        
        //init the status
        memset(dp, 0, sizeof(int) * (size+1) * (k+1) * (target+1));
        for(int i=0;i<=size;i++){
            dp[i][0][0] = 1;
        }
        //iterate dp
        
        for(int n=1;n<size+1;n++){
            for(int kk=1;kk<k+1 && kk<=n;kk++){
                for(int t=1;t<target+1;t++){
                    
                    //the array index is the n-1 for the number n
                    if(t>=A[n-1]){
                        //v[n] is in or v[n] is not in
                        dp[n][kk][t] = dp[n-1][kk][t] + dp[n-1][kk-1][t-A[n-1]];
                    }else{
                        dp[n][kk][t] = dp[n-1][kk][t];
                    }
                    
                    //it is still 0 other wise
                    //std::cout << "n " << n << " k " << kk << " t " << dp[n][kk][t] << std::endl;
                    
                }
            }
        }
        return dp[size][k][target];
        
    }
};

//746. Min Cost Climbing Stairs
// memorize
// the d means ith position
// it needs to jump to the ith+1 position without pay
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        int size = cost.size();
        //edge case
        if(size<2){
            return 0;
        }
        if(size==2){
            return min(cost[0],cost[1]);
        }
        
        //init dp
        vector<int> m (size+1,0);     
        return helper(cost,m,size);
    }
    
    int helper(vector<int>& cost,vector<int>& m, int i){
        //first two cases
        if(i<=1) return 0;
        //history
        if(m[i]>0) return m[i];
        m[i] = min(helper(cost,m,i-1)+cost[i-1],helper(cost,m,i-2)+cost[i-2]);
        return m[i];
    }
};

//bottom to up
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        int size = cost.size();
        //edge case
        if(size<2){
            return 0;
        }
        if(size==2){
            return min(cost[0],cost[1]);
        }
        //init dp
        vector<int>dp(size+1,0);
        //iterate, i is the ith one
        //get to the ith+1 position, without the pay
        for(int i=2;i<size+1;i++){
            //if want to move forward plust cost
            dp[i]=min(dp[i-1]+cost[i-1], dp[i-2]+cost[i-2]);
            //std::cout << "i " << i << " " << dp[i] << std::endl;
        }
        return dp[size];  
    }
};