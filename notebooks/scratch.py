import torch;
import numpy as np;

W = torch.tensor([[1, 2], [3, 4]]).float()
b = torch.tensor([[5], [6]]).float()
print(W) 
print(b)
print(W @ b)