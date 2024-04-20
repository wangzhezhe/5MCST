import numpy as np
import torch
#import gym
import gymnasium as gym
from matplotlib import pyplot as plt

# the code comes from 
# https://towardsdatascience.com/policy-gradient-methods-104c783251e0
# https://github.com/jorditorresBCN/Deep-Reinforcement-Learning-Explained/blob/master/DRL_19_REINFORCE_Algorithm.ipynb

# car pole example
# https://gymnasium.farama.org/environments/classic_control/cart_pole/
# original car pole code
# https://github.com/Farama-Foundation/Gymnasium/blob/main/gymnasium/envs/classic_control/cartpole.py

# another example
# https://github.com/xccds/Ten_Minute_RL/blob/main/ch10/ch10.ipynb
# https://www.bilibili.com/video/BV1e64y1j7bm/?buvid=YF4C23F4EA56CCFF4265A15AB8C0F300EF83&from_spmid=main.space-contribution.0.0&is_story_h5=false&mid=i6O4YheLFqTwGiSb2ly7fn8FTQ%2FSZMtL1rElX6M3iMo%3D&p=1&plat_id=114&share_from=ugc&share_medium=iphone&share_plat=ios&share_session_id=7D2218E1-4371-4CB1-8B57-29E25CA98B5C&share_source=WEIXIN&share_tag=s_i&spmid=united.player-video-detail.0.0&timestamp=1713494796&unique_k=5eMTG2p&up_id=346591207&vd_source=84e49edbada3d8cd23c5aa1d7957d087

#env = gym.make('CartPole-v0')
env = gym.make('CartPole-v0')
print('observation space:', env.observation_space)
print('action space:', env.action_space)
print('threshold: ', env.spec.reward_threshold)

# policy networkn can be a NN
# it accept the state vectors as input and produce a
# discrete probability distribution over possible actions
obs_size = env.observation_space.shape[0] 
n_actions = env.action_space.n  
HIDDEN_SIZE = 256
model = torch.nn.Sequential(
             torch.nn.Linear(obs_size, HIDDEN_SIZE),
             torch.nn.ReLU(),
             torch.nn.Linear(HIDDEN_SIZE, n_actions),
             torch.nn.Softmax(dim=0)
     )

print (model)

print ("model parameters:")
print (model.parameters())

#The REINFORCE training loop
#initial parameters
# alpha value
learning_rate = 0.003
optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)
# H value, how many G in total 
Horizon = 500
MAX_TRAJECTORIES = 500
# the gamma value before the R for each time
gamma = 0.99
score = []
index=0
# how many round of game we will play
for trajectory in range(MAX_TRAJECTORIES):
    index+=1
    #each time we reset the states
    #it comes to the start situation for the env
    curr_state = env.reset()[0]
    done = False
    transitions = [] 
    #print ("curr_state",curr_state,len(curr_state))
    #maximal round for each play
    #when it goes to the end of Horizon, we assume it pass the game, do not try further
    for t in range(Horizon):
        # map the state into the action
        # the forward process of the policy network
        # the output contains two values, either move to left or move to right
        act_prob = model(torch.from_numpy(curr_state).float())
        # choose one action from action spaces (0,1) 
        # the probability to sample from each entry is specified by p
        action = np.random.choice(np.array([0,1]), 
                 p=act_prob.data.numpy())
        # record the last states we have
        prev_state = curr_state
        # the env take the action and get the new state
        # the current state is also updated
        curr_state, _, done, _, info = env.step(action) 
        # record the prev states and action into the list transitions
        # this value can be used as the reward/score
        # we want to maximise this value
        transitions.append((prev_state, action, t+1)) 
        # if the env decide status as false, such as the pole is fallen
        # and the status can not be recovered, it means the game is over
        # the transistions can be think as the rounds of states updating
        if done: 
            break

    # update score value
    if index==1:
        print("transisitions len",len(transitions))
        print(transitions)

    score.append(len(transitions))
    # extract rewards values from transitions
    # the flip reverse the order of array values and start form l
    # the order is the first dimention
    # the return value here is the largest rounds to the smallest one
    reward_batch = torch.Tensor([r for (s,a,r) in 
                   transitions]).flip(dims=(0,)) 
    
    if index==1:
         print("reward_batch",reward_batch)
    
    # compute the rewards
    # the i is from 0 to len(tran)
    # the j is from ith position to the end of len(tran)
    # we accumulate r_t + gamma*r_{t+1} + gamma^2*r_{t+2} ... 
    batch_Gvals =[]
    for i in range(len(transitions)):
        new_Gval=0
        power=0
        for j in range(i,len(transitions)):
             new_Gval=new_Gval+((gamma**power)*reward_batch[j]).numpy()
             power+=1
        batch_Gvals.append(new_Gval)
    
    if index==1:
         print("batch_Gvals:")
         print(batch_Gvals)
    
    expected_returns_batch=torch.FloatTensor(batch_Gvals)
    # normalize resutls into 0 to 1
    expected_returns_batch /= expected_returns_batch.max()
    # this expected word may introduce the confusion, it is just the g values
    if index==1:
         print("expected_returns_batch:")
         print(expected_returns_batch)

    state_batch = torch.Tensor([s for (s,a,r) in transitions]) 
    action_batch = torch.Tensor([a for (s,a,r) in transitions]) 
    # recompute the probability to take actions under each states
    # we did not record the action prob when computing the transactions
    pred_batch = model(state_batch) 
    if index==1:
         print("pred_batch:")
         print(pred_batch)
         print("action_batch:")
         print(action_batch)   
         print("action_batch")
         print(action_batch.long().view(-1,1))
    # the policy is ensentially the probability
    prob_batch = pred_batch.gather(dim=1,index=action_batch
                 .long().view(-1,1)).squeeze() 

    if index==1:
        print("prob_batch")
        print(prob_batch)

    # should we use mean value of all summed up results?
    loss= -torch.sum(torch.log(prob_batch)*expected_returns_batch) 
    # optimizer to do the backward propagation
    # there is connection between backward and optimization
    # https://stackoverflow.com/questions/53975717/pytorch-connection-between-loss-backward-and-optimizer-step
    optimizer.zero_grad()
    # trace back the gradient to the input (parameters need to be optimized)
    loss.backward()
    optimizer.step()

    if trajectory % 50 == 0 and trajectory>0:
            print('Trajectory {}\tAverage Score: {:.2f}'.format(trajectory, np.mean(score[-50:-1])))
   
# 
def running_mean(x):
    N=50
    kernel = np.ones(N)
    conv_len = x.shape[0]-N
    y = np.zeros(conv_len)
    for i in range(conv_len):
        y[i] = kernel @ x[i:i+N]
        y[i] /= N
    return y


score = np.array(score)
avg_score = running_mean(score)

plt.figure(figsize=(15,7))
plt.ylabel("Trajectory Duration",fontsize=12)
plt.xlabel("Training Epochs",fontsize=12)
plt.plot(score, color='gray' , linewidth=1)
plt.plot(avg_score, color='blue', linewidth=3)
plt.scatter(np.arange(score.shape[0]),score, color='green' , linewidth=0.3)

plt.savefig("REINFORCE_Carpole.png",bbox_inches='tight')