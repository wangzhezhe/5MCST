
# This code is based on the example come from 
# https://www.youtube.com/watch?v=FHdlXe1bSe4&list=PLblh5JKOoLUIxGDQs4LFFD--41Vzf-ME1&index=22

# NOTE: Even though we use the PyTorch module, we import it with the name 'torch', which was the original name.
# torch provides basic functions, from setting a random seed (for reproducability) to creating tensors.
import torch
import torch.nn as nn  # torch.nn allows us to create a neural network.
# nn.functional give us access to the activation and loss functions.
import torch.nn.functional as F
# optim contains many optimizers. Here, we're using SGD, stochastic gradient descent.
from torch.optim import SGD

import matplotlib.pyplot as plt  # matplotlib allows us to draw graphs.
import seaborn as sns  # seaborn makes it easier to draw nice-looking graphs.


# create a neural network class by creating a class that inherits from nn.Module.
class BasicNN(nn.Module):

    # __init__() is the class constructor function, and we use it to initialize the weights and biases.
    def __init__(self):

        # initialize an instance of the parent class, nn.Model.
        super().__init__()

        # Now create the weights and biases that we need for our neural network.
        # Each weight or bias is an nn.Parameter, which gives us the option to optimize the parameter by setting
        # requires_grad, which is short for "requires gradient", to True. Since we don't need to optimize any of these
        # parameters now, we set requires_grad=False.
        ##
        # NOTE: Because our neural network is already fit to the data, we will input specific values
        # for each weight and bias. In contrast, if we had not already fit the neural network to the data,
        # we might start with a random initalization of the weights and biases.
        self.w1_00 = nn.Parameter(torch.tensor(1.7), requires_grad=False)
        self.w1_10 = nn.Parameter(torch.tensor(12.6), requires_grad=False)

        self.b1_0 = nn.Parameter(torch.tensor(-0.85), requires_grad=False)
        self.b1_1 = nn.Parameter(torch.tensor(0.0), requires_grad=False)

        self.w2_00 = nn.Parameter(torch.tensor(-40.8), requires_grad=False)
        self.w2_01 = nn.Parameter(torch.tensor(2.7), requires_grad=False)

        self.final_bias = nn.Parameter(torch.tensor(-16.), requires_grad=False)

    def forward(self, input):  # forward() takes an input value and runs it though the neural network
        # illustrated at the top of this notebook.

        # the next three lines implement the top of the neural network (using the top node in the hidden layer).
        input_to_top_relu = input * self.w1_00 + self.b1_0
        top_relu_output = F.relu(input_to_top_relu)
        scaled_top_relu_output = top_relu_output * self.w2_00

        # the next three lines implement the bottom of the neural network (using the bottom node in the hidden layer).
        input_to_bottom_relu = input * self.w1_10 + self.b1_1
        bottom_relu_output = F.relu(input_to_bottom_relu)
        scaled_bottom_relu_output = bottom_relu_output * self.w2_01

        # here, we combine both the top and bottom nodes from the hidden layer with the final bias.
        input_to_final_relu = scaled_top_relu_output + \
            scaled_bottom_relu_output + self.final_bias

        output = F.relu(input_to_final_relu)

        return output  # output is the predicted effectiveness for a drug dose.

# Another example to train the model
# create a neural network by creating a class that inherits from nn.Module.
# NOTE: This code is the same as before, except we changed the class name to BasicNN_train and we modified
# final_bias in two ways:
# 1) we set the value of the tensor to 0, and
# 2) we set "requires_grad=True".
class BasicNN_train(nn.Module):

    # __init__ is the class constructor function, and we use it to initialize the weights and biases.
    def __init__(self):

        # initialize an instance of the parent class, nn.Module.
        super().__init__()

        # parameters about w and b are same with previous cases
        self.w1_00 = nn.Parameter(torch.tensor(1.7), requires_grad=False)
        self.w1_10 = nn.Parameter(torch.tensor(12.6), requires_grad=False)

        self.b1_0 = nn.Parameter(torch.tensor(-0.85), requires_grad=False)
        self.b1_1 = nn.Parameter(torch.tensor(0.0), requires_grad=False)

        self.w2_00 = nn.Parameter(torch.tensor(-40.8), requires_grad=False)
        self.w2_01 = nn.Parameter(torch.tensor(2.7), requires_grad=False)

        # we want to modify final_bias to demonstrate how to optimize it with backpropagation.
        # The optimal value for final_bias is -16...
        # self.final_bias = nn.Parameter(torch.tensor(-16.), requires_grad=False)
        # ...so we set it to 0 and tell Pytorch that it now needs to calculate the gradient for this parameter.
        self.final_bias = nn.Parameter(torch.tensor(0.), requires_grad=True)
        
        # setting the learning rate
        self.learning_rate=0.1


    # the forward process is same with the previous one
    def forward(self, input):

        # the next three lines implement the top of the neural network (using the top node in the hidden layer).
        input_to_top_relu = input * self.w1_00 + self.b1_0
        top_relu_output = F.relu(input_to_top_relu)
        scaled_top_relu_output = top_relu_output * self.w2_00

        # the next three lines implement the bottom of the neural network (using the bottom node in the hidden layer).
        input_to_bottom_relu = input * self.w1_10 + self.b1_1
        bottom_relu_output = F.relu(input_to_bottom_relu)
        scaled_bottom_relu_output = bottom_relu_output * self.w2_01

        # here, we combine both the top and bottom nodes from the hidden layer with the final bias.
        input_to_final_relu = scaled_top_relu_output + \
            scaled_bottom_relu_output + self.final_bias

        output = F.relu(input_to_final_relu)

        return output


if __name__ == '__main__':
    # create the neural network.
    model = BasicNN()

    # print out the name and value for each parameter
    for name, param in model.named_parameters():
        print(name, param.data)

    # test some data for testing
    # now create the different doses we want to run through the neural network.
    # torch.linspace() creates the sequence of numbers between, and including, 0 and 1
    # actual value starts from 0 to 1
    input_doses = torch.linspace(start=0, end=1, steps=11)

    # now print out the doses to make sure they are what we expect...
    print(input_doses)

    # this is the forward process, we input the input_doses
    # and output the output values
    output_values = model(input_doses)
    # Now draw a graph that shows the effectiveness for each dose.
    ##
    # First, set the style for seaborn so that the graph looks cool.
    sns.set(style="whitegrid")

    # create the graph (you might not see it at this point, but you will after we save it as a PDF).
    sns.lineplot(x=input_doses,
                 y=output_values,
                 color='green',
                 linewidth=2.5)

    # now label the y- and x-axes.
    plt.ylabel('Effectiveness')
    plt.xlabel('Dose')

    # optionally, save the graph as a PDF.
    plt.savefig('BasicNN.png')

    # Now let's graph the output of `BasicNN_train`, which is currently not optimized,
    # and compare it to the graph we drew earlier of the optimized neural network.

    # create the neural network.
    model_untrained = BasicNN_train()

    # now run the different doses through the neural network.
    # the current model does not fit the data
    # since it has the untrained parameters
    output_values = model_untrained(input_doses)
    
    # clear previous image results
    plt.clf()

    # Now draw a graph that shows the effectiveness for each dose.
    # set the style for seaborn so that the graph looks cool.
    sns.set(style="whitegrid")

    sns.lineplot(x=input_doses,
             y=output_values.detach(),  
             # NOTE: because final_bias has a gradident, we call detach()
             # to return a new tensor that only has the value and not the gradient.
             # be careful about it, this is for figure drawing
             color='green',
             linewidth=2.5)

    # now label the y- and x-axes.
    plt.ylabel('Effectiveness')
    plt.xlabel('Dose')

    # lastly, save the graph as a PNG.
    plt.savefig('model_untrained.png')

    # The graph shows that when the dose is **0.5**, the output from the unoptimized neural network is **17**, 
    # which is wrong, since the output value should be **1**. So, now that we have a parameter we can optimize, 
    # let's create some training data that we can use to optimize it.
    
    # generating the original input data
    # and the labeled data for training the model
    ## create the training data for the neural network.
    inputs = torch.tensor([0., 0.5, 1.])
    labels = torch.tensor([0., 1., 0.])

    model_train = BasicNN_train()
    
    print("original parameters for the untrained model:")
    for name, param in model_train.named_parameters():
        print(name, param.data)
    print(model_train.learning_rate)

    ## here we're creating an optimizer to train the neural network.
    ## NOTE: There are a bunch of different ways to optimize a neural network.
    ## In this example, we'll use Stochastic Gradient Descent (SGD). However,
    ## another popular algortihm is Adam (which will be covered in a StatQuest).
    optimizer = SGD(model_train.parameters(), model_train.learning_rate) 
    print("Final bias, before optimization: " + str(model_train.final_bias.data) + "\n")

    ## this is the optimization loop. 
    ## Each time the optimizer sees all of the training data is called an "epoch".
    for epoch in range(1000):
        
        ## we create and initialize total_loss for each epoch so that we can evaluate how well model fits the
        ## training data. At first, when the model doesn't fit the training data very well, total_loss
        ## will be large. However, as gradient descent improves the fit, total_loss will get smaller and smaller.
        ## If total_loss gets really small, we can decide that the model fits the data well enough and stop
        ## optimizing the fit. Otherwise, we can just keep optimizing until we reach the maximum number of epochs. 
        total_loss = 0
    
        ## this internal loop is where the optimizer sees all of the training data and where we 
        ## calculate the total_loss for all of the training data
        ## the len of iteration is len(inputs) here
        for iteration in range(len(inputs)):
            # this is a naive way to load the data
            # the better way is to use the dataLoader
        
            input_i = inputs[iteration] ## extract a single input value (a single dose)...
            label_i = labels[iteration] ## ...and its corresponding label (the effectiveness for the dose).
        
            output_i = model_train(input_i) ## calculate the neural network output for the input (the single dose).

            ## calculate the loss for the single value.
            ## NOTE: Because output_i = model(input_i), "loss" has a connection to "model"
            ## and the derivative (calculated in the next step) is kept and accumulated in "model".        
            loss = (output_i - label_i)**2 

            # backward() calculates the derivative for that single value and adds it to the previous one.
            # parameters in model are actually updated here
            # when there are multiple inputs
            # loss backward remember the derivative that we calculated for the first point
            # loss backward accumulates the derivatives each time we go through nested loop
            loss.backward() 
             
            total_loss += float(loss) # accumulate the total loss for this epoch.
             
        if (total_loss < 0.0001):
            print("Num steps: " + str(epoch))
            break
        
        ## take a step toward the optimal value.
        ## optimizer can access the drtivatives stored in the model
        optimizer.step() 
        optimizer.zero_grad()
        ## This zeroes out the gradient stored in "model". otherwize, the gradients are accumulated
        ## at the next epoch
        ## Remember, by default, gradients are added to the previous step (the gradients are accumulated),
        ## and we took advantage of this process to calculate the derivative one data point at a time.
        ## NOTE: "optimizer" has access to "model" because of how it was created with the call 
        ## (made earlier): optimizer = SGD(model.parameters(), lr=0.1).
        ## ALSO NOTE: Alternatively, we can zero out the gradient with model.zero_grad().
    
        print("Step: " + str(epoch) + " Final Bias: " + str(model_train.final_bias.data) + "\n")
        ## now go back to the start of the loop and go through another epoch.
  
    print("Total loss: " + str(total_loss), "epoch number", epoch)
    print("Final bias, after optimization: " + str(model_train.final_bias.data))

    plt.clf()
    ## run the different doses through the neural network
    output_values = model_train(input_doses)

    ## set the style for seaborn so that the graph looks cool.
    sns.set(style="whitegrid")

    ## create the graph (you might not see it at this point, but you will after we save it as a PDF).
    sns.lineplot(x=input_doses, 
             y=output_values.detach(), ## NOTE: we call detach() because final_bias has a gradient
             color='green', 
             linewidth=2.5)

    ## now label the y- and x-axes.
    plt.ylabel('Effectiveness')
    plt.xlabel('Dose')

    ## lastly, save the graph as a PDF.
    plt.savefig('BascNN_optimized.png')