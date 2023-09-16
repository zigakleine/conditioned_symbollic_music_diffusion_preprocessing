
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


# import numpy as np
#
# # Create your original numpy array of shape (68, 42)
# original_array = np.random.rand(129, 42)
#
# # Calculate the number of batches
# batch_size = 32
# num_batches = original_array.shape[0] // batch_size
#
# # Calculate the new shape of the reshaped array
# new_shape = (num_batches, batch_size, original_array.shape[1])
#
# # Reshape the original array into the new shape
# reshaped_array = original_array[:num_batches * batch_size].reshape(new_shape)
#
# print(reshaped_array.shape)  # Should print (2, 32, 42)
#

import pickle
import json

metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))

file = open('./db_metadata/nesmdb/nesmdb_updated2808_BACKUP.pkl', 'wb')
pickle.dump(metadata, file)
file.close()

y = json.dumps(metadata, indent=4)
file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808_BACKUP.json', 'w')
file_json.write(y)
file_json.close()


# metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))
#
# file = open('./db_metadata/nesmdb/nesmdb_updated2808_BACKUP.pkl', 'wb')
# pickle.dump(metadata, file)
# file.close()
#
# y = json.dumps(metadata, indent=4)
# file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808_BACKUP.json', 'w')
# file_json.write(y)
# file_json.close()





# import numpy as np
# import pickle
#
# fb256_slices = pickle.load(open('./fb256_slices.pkl', "rb"))
# fb256_slices = np.array(fb256_slices)
#
# fb256_mask = np.zeros((512,), dtype=bool)
# fb256_mask[fb256_slices] = True
#
#
# aaa = np.random.rand(3, 32, 512)
#
# bbb = aaa[:, :, fb256_mask]
#
#
# bbb = np.take_along_axis(aaa, fb256_slices, axis=-1)
#
# hh = pickle.load(open("./lakh_encoded/0/03f3e2c02f0f61e8142fd1049bd6dd5d_enc.pkl", "rb"))
# print("hh")