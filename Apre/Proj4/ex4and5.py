from scipy.io import arff
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
from sklearn.feature_selection import mutual_info_classif
from sklearn.feature_selection import SelectKBest

data = arff.loadarff('breast.w_modified.arff')
df = pd.DataFrame(data[0])

X = df.iloc[:, 0:9]
y = df.iloc[:, -1]
y = y.astype('string')

def scores(k):
    km = KMeans(n_clusters=k)
    km.fit_predict(X)
    score = silhouette_score(X, km.labels_)
    print("silhouette k=" + str(k) + " score: " + str(score))

    mydict = {i: np.where(km.labels_ == i)[0] for i in range(km.n_clusters)}
    print("ecr k=" + str(k) + " score: " + str(ecr(k, mydict)))


def ecr(k, clusters):
    sum = 0

    # interseção: ver dos elementos do cluster, a classe mais popular
    for i in range(k):
        benigns = 0
        malignants = 0
        for j in clusters[i]:
            if y[j] == "b'benign'":
                benigns += 1
            elif y[j] == "b'malignant'":
                malignants += 1
        sum += len(clusters[i]) - max(malignants, benigns)

    return sum/k


def plot():
    X_new = SelectKBest(mutual_info_classif, k=2).fit_transform(X, y)
    km = KMeans(n_clusters=3)
    km.fit(X_new)
    y_kmeans = km.predict(X_new)

    plt.scatter(X_new[:,0], X_new[:,1], c=y_kmeans)

    centers = km.cluster_centers_
    plt.scatter(centers[:, 0], centers[:, 1], c='black', s=150);

    plt.show()

    print("\n" + str(silhouette_score(X_new, km.labels_)))


scores(2)
scores(3)
plot()