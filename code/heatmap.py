'''
This scripts is used to plot the data collected after an attack

@author: Matteo Oldani
'''

import numpy as np 
import seaborn as sns
import matplotlib.pylab as plt
import sys 


plt.style.use("seaborn")

data = []
data_paper = [[0] * 256]*256 

with open(sys.argv[1]) as f:
	lines = f.readlines()
	lines = lines[7:]
	new_ls = []
	for l in lines:
		l = l.split(' ')
		l = l[:-1]
		new_l = []
		for x in l:
			new_l.append(int(x))
		new_ls.append(new_l)


	data = np.matrix(new_ls)


plt.figure(figsize=(12, 10))
sns.set(font_scale=1.3)
heat_map = sns.heatmap(data, xticklabels=16, yticklabels=16, 
	cmap="crest")
plt.xlabel("plaintext[0]", fontsize=18)
plt.ylabel("address", fontsize=18)

plt.show()
