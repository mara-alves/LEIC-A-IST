from matplotlib import pyplot as plt

# cluster 1
c1 = plt.scatter([2, -1, 4], [4, 2, 0])
# cluster 2
c2 = plt.scatter([-1], [-4])
# centroids
c = plt.scatter([1.56611, -0.38411], [2.100757, -3.41387], color = 'black', s=150)

plt.legend((c1, c2, c), ('cluster 1', 'cluster 2', 'centroids'))

plt.show()