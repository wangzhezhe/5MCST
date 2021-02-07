
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


//1137. N-th Tribonacci Number
class Solution {
public:
    int tribonacci(int n) {
        
        //edge case
        if(n<=1){
            return n;
        }
        
        if(n==2){
            return 1;
        }
        
        //init array
        vector<long int> dp (n+1,0);
        dp[0]=0;
        dp[1]=1;
        dp[2]=1;
        
        //iterate
        for(int i=3;i<n+1;i++){
            dp[i]=dp[i-1]+dp[i-2]+dp[i-3];
        }
        
        return dp[n];
        
    }
};

//63. Unique Paths II
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int m = obstacleGrid.size();
        int n = obstacleGrid[0].size();
        vector<vector<int>> dp (m, vector<int>(n,0));
        if(obstacleGrid[0][0]==1){
            return 0;
        }
        
        //two dimentions of case
        //if it is 0 different positions
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                if(obstacleGrid[i][j]==1){
                    //keep zero do nothing
                    continue;
                }
                if(i==0 && j==0){
                    dp[i][j]=1;
                    continue;
                }
                //not zero, consider differnet cases
                if(i==0){
                    dp[i][j]=dp[i][j-1];
                    continue;
                }        
                if(j==0){
                    dp[i][j]=dp[i-1][j];
                    continue;
                }            
                //i>0 j>0
                dp[i][j]=dp[i-1][j]+dp[i][j-1];            
            }
        }
        return dp[m-1][n-1];    
    } 
};

//64. Minimum Path Sum

class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size();
        int n = grid[0].size();
        vector<vector<int>> dp (m, vector<int>(n, INT_MAX));
        
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                if(i==0 && j==0){
                    dp[i][j]=grid[i][j];
                        continue;
                }
                if(i>0 && j==0){
                    dp[i][j]=min(dp[i][j],dp[i-1][j]+grid[i][j]);
                    continue;
                }
                if(i==0 && j>0){
                    dp[i][j]=min(dp[i][j],dp[i][j-1]+grid[i][j]);
                    continue;
                }
                
                dp[i][j]=grid[i][j]+min(dp[i-1][j],dp[i][j-1]);
                
            }
            
        }
        return dp[m-1][n-1];
    }
};

//120. Triangle
//else if is more efficient then the continue case
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        int m = triangle.size();
        vector<vector<int>> dp (m, vector<int>(1, INT_MAX));
    
        int ans = INT_MAX;
        
        for(int r=0;r<m;r++){
            
            int len = triangle[r].size();
            dp[r].resize(len, INT_MAX);
            for(int c=0;c<len;c++){
                //four typical csaes
                if(r == 0){
                    //when r = 0 c is explicit
                    dp[r][c]= min(triangle[r][c],dp[r][c]);
                }else if(r>0 && c ==0){
                    dp[r][c]= min(dp[r][c], triangle[r][c] + dp[r-1][c]);
                }else if(r>0 && c == (len-1)){
                    dp[r][c]= min(dp[r][c], triangle[r][c] + dp[r-1][c-1]);
                }else {
                    dp[r][c]= triangle[r][c] + min(dp[r-1][c], dp[r-1][c-1]);
                }
                
                //go through last row to get the min ans
                if(r==m-1){
                    ans = min(ans, dp[r][c]);
                }
            }
        }
        return ans;
    }    
};

//174. Dungeon Game 
//it is important to know that the minimal value is 1 
//and the defination of the element in dp matrix
class Solution {
public:
    int calculateMinimumHP(vector<vector<int>>& dungeon) {
        
        int r= dungeon.size();
        int c= dungeon[0].size();
        
        //edge case
        
        //set dp
        vector< vector<int> > dp (r, vector<int>(c,0));
        
        
        //iterate
        //start from the right bottom to the left up
        //start r,c end 0,0
        int min = helper(0,0,r,c,dp,dungeon);
        //std::cout << "min " << min << std::endl;
        if(min<=0){
            min=1;
        }
        int ans = min-dungeon[0][0];
        if(ans<=0){
            ans = 1;
        }
        return ans;
        
    }
    
    //the come out of the i,j what is the minimal hp
    int helper(int i, int j, int m, int n, vector<vector<int>>& dp, vector<vector<int>>& dungeon){
        
        if(dp[i][j]!=0){
            return dp[i][j];
        }
        
        if(i==m-1 && j==n-1){
            //there is 1 point at the start
            return 1;
        }else if(j==n-1){
            int dep = helper(i+1,j,m,n,dp,dungeon);
            dp[i][j] = dep-dungeon[i+1][j];
            
            
        }else if(i==m-1){
            int dep = helper(i,j+1,m,n,dp,dungeon);
            dp[i][j] = dep-dungeon[i][j+1];
            
        }else{
            int dep1 = helper(i,j+1,m,n,dp,dungeon);
            int dep2 = helper(i+1,j,m,n,dp,dungeon);

            dp[i][j] = min(dep1-dungeon[i][j+1],dep2-dungeon[i+1][j]);
         
        }
        if(dp[i][j]<=0){
            dp[i][j] = 1;
        }
        return dp[i][j];
    }
};


// 931 Minimum Falling Path Sum
// consider different cases clearly (if i is edge then if j is edge)
class Solution {
public:
    int minFallingPathSum(vector<vector<int>>& A) {
        int r = A.size();
        int c = A[0].size();
        
        //init dp
        vector<vector<int>> dp(r, vector<int>(c,0));
        
        //edge case
        
        int ans = INT_MAX;
        //iterate
        for(int i=0;i<r;i++){
            for(int j=0;j<c;j++){
                if(i==0){
                    dp[i][j]=A[i][j];
                }else{
                    
                    if(j==0){
                        if(j==c-1){
                             //there is one column
                             dp[i][j]=dp[i-1][j]+A[i][j];
                        }else{
                            dp[i][j]=min(dp[i-1][j],dp[i-1][j+1])+A[i][j];
                        }
                        
                    }else if(j==c-1){
                        dp[i][j]=min(dp[i-1][j-1],dp[i-1][j])+A[i][j];
                    }else{
                        //other cases
                        dp[i][j]=min(dp[i-1][j-1],min(dp[i-1][j],dp[i-1][j+1]))+A[i][j];
                    }
                }
                
                //select the min one when it is the end line
                if(i==r-1){
                    ans = min (ans, dp[i][j]);
                }
                
            }
        }
        return ans;
    }
};


//1210. Minimum Moves to Reach Target with Rotations
struct Entry {
  Entry(int x, int y, int d) : m_x(x), m_y(y), m_d(d){};
  int m_x = 0;
  int m_y = 0;
  // 0 is the horizon
  // 1 is the vertical
  int m_d = -1;
  ~Entry(){};
};

struct Status {
  bool h = false;
  bool v = false;
  int step_h = INT_MAX;
  int step_v = INT_MAX;
};

class Solution {
 public:
  int minimumMoves(vector<vector<int>>& grid) {
    int r = grid.size();
    int c = grid[0].size();

    // this is the struct used for dp
    // it is not efficient to index
    // if we put this value in Entry
    vector<vector<Status>> status(r, vector<Status>(c));

    // init the status
    status[1][0].step_h = 0;
    status[1][0].h = true;

    // use the bfs to go through all solution tree
    queue<Entry> bfsq;

    bfsq.push(Entry(1, 0, 0));

    int res = INT_MAX;
    while (!bfsq.empty()) {
      // pop element go through
      Entry entry = bfsq.front();
      bfsq.pop();

      int x = entry.m_x;
      int y = entry.m_y;
      int d = entry.m_d;

      //std::cout << "front x " << x << " y " << y << " d " << d << std::endl;

      if (x == (c - 1) && y == (r - 1) && d == 0) {
        res = min(res, status[x][y].step_h);
      }

      if (d == 0) {
        // move right
        if (x + 1 < c && grid[y][x + 1] == 0) {
          bool first = true;
          if (status[x + 1][y].h) {
            first = false;
          }

          status[x + 1][y].step_h =
              min(status[x + 1][y].step_h, status[x][y].step_h + 1);
          status[x + 1][y].h = true;
          // only push if this node it not been to through
          // to avoid the circular
          if (first) {
            //std::cout << "push x " << x + 1 << " y " << y << " d " << 0
            //          << std::endl;
            bfsq.push(Entry(x + 1, y, 0));
          }
        }

        // move down
        if (y + 1 < r && grid[y + 1][x] == 0 && grid[y + 1][x - 1] == 0) {
          bool first = true;
          if (status[x][y + 1].h) {
            first = false;
          }
          status[x][y + 1].step_h =
              min(status[x][y + 1].step_h, status[x][y].step_h + 1);
          status[x][y + 1].h = true;

          if (first) {
            //std::cout << "push x " << x << " y " << y + 1 << " d " << 0
            //          << std::endl;
            bfsq.push(Entry(x, y + 1, 0));
          }
        }

        // rotate
        if (x - 1 >= 0 && y + 1 < r && grid[y + 1][x] == 0 &&
            grid[y + 1][x - 1] == 0) {
          bool first = true;
          if (status[x - 1][y + 1].v) {
            first = false;
          }
          status[x - 1][y + 1].step_v =
              min(status[x - 1][y + 1].step_v, status[x][y].step_h + 1);
          status[x - 1][y + 1].v = true;

          if (first) {
            //std::cout << "push x " << x - 1 << " y " << y + 1 << " d " << 1
            //          << std::endl;
            bfsq.push(Entry(x - 1, y + 1, 1));
          }
        }
      }

      if (d == 1) {
        // move down
        if (y + 1 < r && grid[y + 1][x] == 0) {
          bool first = true;
          if (status[x][y + 1].v) {
            first = false;
          }

          status[x][y + 1].step_v =
              min(status[x][y + 1].step_v, status[x][y].step_v + 1);
          status[x][y + 1].v = true;

          // only push if this node it not been to through
          // to avoid the circular
          if (first) {
            //std::cout << "push x " << x << " y " << y + 1 << " d " << 1
            //          << std::endl;
            bfsq.push(Entry(x, y + 1, 1));
          }
        }

        // move right
        if (x + 1 < c && grid[y][x + 1] == 0 && grid[y - 1][x + 1] == 0) {
          bool first = true;
          if (status[x + 1][y].v) {
            first = false;
          }

          status[x + 1][y].step_v =
              min(status[x + 1][y].step_v, status[x][y].step_v + 1);
          status[x + 1][y].v = true;
          // only push if this node it not been to through
          // to avoid the circular
          if (first) {
            //std::cout << "push x " << x + 1 << " y " << y << " d " << 1
            //          << std::endl;
            bfsq.push(Entry(x + 1, y, 1));
          }
        }

        // rotate
        if (x + 1 < c && y - 1 >= 0 && grid[y][x + 1] == 0 &&
            grid[y - 1][x + 1] == 0) {
          bool first = true;
          if (status[x + 1][y - 1].h) {
            first = false;
          }

          status[x + 1][y - 1].step_h =
              min(status[x + 1][y - 1].step_h, status[x][y].step_v + 1);
          status[x + 1][y - 1].h = true;
          // only push if this node it not been to through
          // to avoid the circular
          if (first) {
            //std::cout << "push x " << x + 1 << " y " << y - 1 << " d " << 0
            //          << std::endl;
            bfsq.push(Entry(x + 1, y - 1, 0));
          }
        }
      }
    }
    if(res==INT_MAX){
        return -1;
    }
    return res;
  }
};

//304. Range Sum Query 2D - Immutable
class NumMatrix {
public:
    NumMatrix(const vector<vector<int>>& matrix) {
        sums.clear();
        if (matrix.empty()) return;
        //construct the matrix
        int m = matrix.size();
        int n = matrix[0].size();

        // check the zero input in the real project
        //init when x==0 or y==0
        //if not use the dummy value, the boundry need to be initilized
        //for this problem, there are more benifis that use the dummy valus
        //when caculate the subRegion, and the coresponds value is at the 0,0
        //there are benifits for using the dummy value
        sums = vector<vector<int>>(m+1,vector<int>(n+1,0)); 
        
        //the real value start from the 1 for the sums matrix
        
        //caculate others
        for(int i=1;i<=m;i++){
            for(int j=1;j<=n;j++){
                sums[i][j]=matrix[i-1][j-1]+sums[i-1][j]+sums[i][j-1]-sums[i-1][j-1];
            }
        }
        
    }
    
    int sumRegion(int row1, int col1, int row2, int col2) {
        //if there is only dummy positions
        int sum=0;
        //caculate the subregion
        sum = sums[row2+1][col2+1]-sums[row2+1][col1]-sums[row1][col2+1]+sums[row1][col1];
        return sum;
    }
private:
    vector<vector<int>> sums;
};


//1277. Count Square Submatrices with All Ones
class Solution {
public:
    int countSquares(vector<vector<int>>& matrix) {
        if(matrix.empty()) return 0;
        int r = matrix.size();
        int c = matrix[0].size();
        
        //init the status with dummy value
        vector<vector<int>>dp(r+1,vector<int>(c+1,0));
        
        int ans = 0;
        //cacualte status
        //dp[i][j] coresponds to matrix[i-1][j-1]
        //matrix[i][j] coresponds to dp[i+1][j+1]
        for(int i=1;i<=r;i++){
            for(int j=1;j<=c;j++){
                if(matrix[i-1][j-1]==1){
                    dp[i][j]=min(dp[i-1][j-1],min(dp[i][j-1],dp[i-1][j]))+1;
                }else{
                    dp[i][j]=0;
                }
               ans += dp[i][j];
            }
        }
        return ans; 
    }
};

//213. House Robber II
class Solution {
     struct Entry{
         int r=0;
         int nr=0;
     };
public:
    int rob(vector<int>& nums) {
        
        int l = nums.size();
        if(l==0){
            return 0;
        }
        if(l==1){
            return nums[0];
        }
        if(l==2){
            return max(nums[0],nums[1]);
        }
        
        //init status
        vector<Entry> status(l);
        //case1 first one is robbed
        status[0].r=nums[0];
        
        //TODO optimize the space
        for(int i=1;i<l-1;i++){
            status[i].r = status[i-1].nr+nums[i];
            status[i].nr = max(status[i-1].nr,status[i-1].r);
        }
        //not rob last, since the first one is rubbed
        int res1 = max(status[l-2].r,status[l-2].nr);
        
        //case2 first one is not rub, last one can be rubbed
        status[0].r=0;
        for(int i=1;i<l;i++){
            status[i].r = status[i-1].nr+nums[i];
            status[i].nr = max(status[i-1].nr,status[i-1].r);
        }
        int res2 = max(status[l-1].r,status[l-1].nr);
        
        
        
        return max(res1, res2);
        
    }
};

//488 Zuma Game
class Solution {
 public:
  int findMinStep(string board, string hand) {
    unordered_map<char, int> handCount;
    // init the map
    for (int i = 0; i < hand.size(); i++) {
      char character = hand[i];
      if (handCount.count(character) > 0) {
        handCount[character]++;
      } else {
        handCount[character] = 1;
      }
    }

    // remove >=3 continuous in current str
    string boardNew = eraseRepeat(board);
    // execute helper
    int ret = helper(boardNew, handCount);
    return ret == INT_MAX ? -1 : ret;
  }

  // helper function current board current hand and map
  int helper(string board, unordered_map<char, int>& handCount) {
    // return status
    if (board.size() == 0) {
      return 0;
    }

    // iterate board balls to decide how many need to be inserted to erase
    //std::cout << "current board: " << board << std::endl;
    int minInserted = INT_MAX;
    int i, j;
    for (i = 0; i < board.size();) {
      for (j = i; j < board.size(); j++) {
        // find bound of the same str
        if (board[j] != board[i]) {
          break;
        }
      }

      // decouple
      int need = 3 - (j - i);
      //std::cout << "i " << i << " j " << j << " need " << need << std::endl;
      char ballColor = board[i];
      if (need > 0) {
        // remaining is >=1 since all the >=3 case is removed
        if (handCount.count(ballColor) == 0 || handCount[ballColor] < need) {
          // not exist
          //std::cout << "---not exist" << std::endl;
          i=j;
          continue;
        }
      } else {
        // do not need to insert in this iteration
        need = 0;
      }

      // get the new string after 1 level inserting, it is ok if j is at the
      // end position, the substr will return empty
      string boardNew = board.substr(0, i) + board.substr(j);

      //std::cout << "boardNew " << boardNew << std::endl;

      handCount[ballColor] -= need;
      int sublevel = helper(boardNew, handCount);
      // std::cout << "boardNew: " << boardNew << " sublevel " << sublevel
      //          << std::endl;
      if (sublevel != INT_MAX) {
        minInserted = min(minInserted, need + sublevel);
      }
      // std::cout << "i " << i << " board: " << board << " sublevel " <<
      // sublevel
      //          << std::endl;

      // it is important to set the previous try back
      handCount[ballColor] += need;
      // update the new i
      i = j;
    }

    return minInserted;
  }

  // the function that delete the repeat ball
  // ok for the case that contains multiple continuous str
  string eraseRepeat(string str) {
    // delete the ball that has three continuous
    int i = 0, j = 0;
    for (i = 0; i < str.size(); i++) {
      int count = 0;
      for (j = i; j < str.size(); j++) {
        if (str[j] == str[i]) {
          count++;
        } else {
          break;
        }
      }
      if (count >= 3) {
        // process the str
        string tempstr = str.substr(0, i) + str.substr(j);
        str = eraseRepeat(tempstr);
      }
    }
    return str;
  }
};