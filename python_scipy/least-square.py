from scipy.optimize import least_squares
import numpy as np
import matplotlib.pyplot as plt

def sample_y(theta, t):
    # this is a logistic function
    return theta[0] / (1 + np.exp(- theta[1] * (t - theta[2])))

ts = np.linspace(0, 1)
K = 1; r = 10; t0 = 0.5; noise = 0.1
raw_y = sample_y([K, r, t0], ts) + noise * np.random.rand(ts.shape[0])
#print(raw_y)

def sample_fun(theta):
    return sample_y(theta, ts) - raw_y

def ls_sample():

    # train the model
    theta0 = [1,2,3]
    res1 = least_squares(sample_fun, theta0)

    print(res1.x)

    f_predicted = sample_y(res1.x, ts)

    fig, ax = plt.subplots()

    ax.plot(raw_y)
    ax.plot(f_predicted)

    plt.savefig("sample.png",bbox_inches='tight')


# x is procs number
# theta are other parameters

def rr_model_temp1(theta, proc_num):
    return theta[0]*pow(theta[1],-proc_num)

def rr_model_temp2(theta, proc_num):
    print("temp2 coeff:", theta[2],theta[3],theta[4])
    return (theta[2]*pow(proc_num,2)+theta[3]*proc_num+theta[4])

def rr_model(theta, proc_num):
    # x times this equation as a minimal value
    # when there is constant term, after times x this becomes increase

    # this looks good and is similar to the actual value
    return theta[0]/(theta[1]+proc_num)+theta[2]
    
    #this is the most accurate one(the extra term is a 1/one)
    #return theta[0]/proc_num + theta[1]
    #return theta[0]*pow(theta[1],-proc_num)+theta[2]
    #return theta[0]*pow(theta[1],-proc_num)+theta[2]/proc_num + theta[3]
    #return theta[0]*pow(theta[1],-proc_num)+(theta[2]*pow(proc_num,2)+theta[3]*proc_num+theta[4])
    #return theta[0]+theta[1]*proc_num+theta[2]*pow(proc_num,2)
    #return theta[0]*pow(theta[1],-proc_num)+ theta[2] / (1 + np.exp(-(proc_num - theta[3])))
    #return theta[0]*pow(theta[1],-proc_num)+ theta[2] / (1 + np.exp(theta[3]*(proc_num - theta[4])))
    
def rr_fun(theta):
    proc_num=np.array([2,4,8,16,32,64,128])
    raw_y=np.array([78901.10938,35120.34375,13301.74219,6998.071777,3714.635742,2373.825195,1900.957275])
    #raw_y=np.log(raw_y)
    return rr_model(theta, proc_num) - raw_y

def ls_render_resources():
    theta0 = [2,1,2]
    res1 = least_squares(rr_fun, theta0)
    print(res1.x)
    proc_num=np.array([2,4,8,16,32,64,128,256,512,1024])
    rr_predicted = rr_model(res1.x, proc_num)
    
    fig, ax = plt.subplots()
    dist = 1.0
    offset=0
    xindex=[]
    for i in range (0,10):
        xindex.append(offset+i*dist)
    ax.set_xticks(xindex)

    ax.set_xticklabels(('2','4','8','16','32','64','128','256','512','1024'), fontsize='large')

    #ax.plot(raw_y)
    rr_raw_y=np.array([78901.10938,35120.34375,13301.74219,6998.071777,3714.635742,2373.825195,1900.957275])
    #rr_raw_y=np.log(rr_raw_y)
    ax.plot(rr_raw_y)
    ax.plot(rr_predicted,linestyle='--')
    
    plt.savefig("cost.png",bbox_inches='tight')
    return


if __name__ == "__main__":
    ls_render_resources()