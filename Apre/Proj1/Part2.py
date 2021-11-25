import plotly.express as px
import pandas as pd
from scipy.io import arff

from sklearn.model_selection import train_test_split
from sklearn.model_selection import cross_val_score
from sklearn.neighbors import KNeighborsClassifier
from sklearn import metrics
from sklearn.naive_bayes import GaussianNB

data = arff.loadarff('breast.w_modified.arff')
df = pd.DataFrame(data[0])

X = df.iloc[:, 0:9]
y = df.iloc[:, -1]
y = y.astype('string')

X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=57)

knn = KNeighborsClassifier(n_neighbors=3) # mudar 3 para o k desejado
scores = cross_val_score(knn, X, y, cv=10, scoring='accuracy')
print(scores)
print("Accuracy kNN:", scores.mean())

gnb = GaussianNB()
gnb.fit(X_train, y_train)
y_pred = gnb.predict(X_test)
print("Accuracy Naive Bayes:", metrics.accuracy_score(y_test, y_pred))

# Histogramas
fig = px.histogram(df, x="Clump_Thickness", color="Class")
fig.show()
fig = px.histogram(df, x="Cell_Size_Uniformity", color="Class")
fig.show()
fig = px.histogram(df, x="Cell_Shape_Uniformity", color="Class")
fig.show()
fig = px.histogram(df, x="Marginal_Adhesion", color="Class")
fig.show()
fig = px.histogram(df, x="Single_Epi_Cell_Size", color="Class")
fig.show()
fig = px.histogram(df, x="Bare_Nuclei", color="Class")
fig.show()
fig = px.histogram(df, x="Bland_Chromatin", color="Class")
fig.show()
fig = px.histogram(df, x="Normal_Nucleoli", color="Class")
fig.show()
fig = px.histogram(df, x="Mitoses", color="Class")
fig.show()