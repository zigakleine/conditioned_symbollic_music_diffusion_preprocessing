
import numpy as np


# a = np.zeros((8,4,3))
#
# for i in range(8):
#     for j in range(4):
#         for k in range(3):
#             a[i,j,k] = i
#
# print(a.shape)
#
# b = []
#
# for i in range(19):
#     b.append(a[i%8])
#
#
# c = np.array(b)
#
# print(c.shape)


import numpy as np

# Create your original numpy array of shape (68, 42)
original_array = np.random.rand(129, 42)

# Calculate the number of batches
batch_size = 32
num_batches = original_array.shape[0] // batch_size

# Calculate the new shape of the reshaped array
new_shape = (num_batches, batch_size, original_array.shape[1])

# Reshape the original array into the new shape
reshaped_array = original_array[:num_batches * batch_size].reshape(new_shape)

print(reshaped_array.shape)  # Should print (2, 32, 42)

