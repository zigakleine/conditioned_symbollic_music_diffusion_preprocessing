import numpy as np
import pickle

nesmdb_stacked = pickle.load(open("./nesmdb_single_stacked.pkl", "rb"))

nesmdb_std_devs = np.std(nesmdb_stacked, axis=0)


nesmdb_std_devs_indices = [(i, dev) for i, dev in enumerate(nesmdb_std_devs)]
nesmdb_std_devs_indices = sorted(nesmdb_std_devs_indices, key=(lambda x: x[1]))

nesmdb_std_devs_indices_less = nesmdb_std_devs_indices[:76]
nesmdb_std_devs_indices_less_ixs = [i for i, dev in nesmdb_std_devs_indices_less]
nesmdb_std_devs_indices_less_ixs = sorted(nesmdb_std_devs_indices_less_ixs)

file = open('./fb256_slices_76.pkl', 'wb')
pickle.dump(nesmdb_std_devs_indices_less_ixs, file)
file.close()

# file = pickle.load(open('./fb256_slices_76.pkl', 'rb'))
# print(len(file))