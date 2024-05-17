import torch
import torch.nn as nn
import torch.optim as optim
import torchvision
import matplotlib.pyplot as plt

transform = torchvision.transforms.Compose([torchvision.transforms.ToTensor()])
 
trainset = torchvision.datasets.CIFAR10(root='./data', train=True, download=True, transform=transform)
testset = torchvision.datasets.CIFAR10(root='./data', train=False, download=True, transform=transform)
 
batch_size = 32
trainloader = torch.utils.data.DataLoader(trainset, batch_size=batch_size, shuffle=True)
testloader = torch.utils.data.DataLoader(trainset, batch_size=batch_size, shuffle=True)
 

# the loaded data is in pytorch tensor format
print("trainset len",len(trainset))
# each element is a combination of the input and label
# this is a typical data set used for the classification
img, label = trainset[0]
print("img",img,"label",label)
print("img shape",img.shape)
# we need to put the channel at the last position
# in order to make it show

#PyTorch modules processing image data expect tensors in the format Channel × H × W
#Whereas PILLow and Matplotlib expect image arrays in the format H × W × Channel

plt.imshow(img.permute(1, 2, 0))
plt.show()