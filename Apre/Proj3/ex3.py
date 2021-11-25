from scipy.io import arff
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPRegressor
from sklearn.metrics import ConfusionMatrixDisplay
import matplotlib.pyplot as plt
from sklearn.model_selection import cross_val_predict, cross_val_score

data = arff.loadarff('kin8nm.arff')
df = pd.DataFrame(data[0])

X = df.iloc[:, 0:8]
y = df.iloc[:, -1]
y = y.astype("float64")

fig, ax = plt.subplots()

clf_noreg = MLPRegressor(hidden_layer_sizes=(3,2),activation="relu",random_state=0, alpha=0).fit(X.values, y.values)
y_pred_noreg = cross_val_predict(clf_noreg, X.values, y.values, cv=5)
scores = cross_val_score(clf_noreg, X.values, y.values, cv=5)
print("Accuracy no regularization= ", scores.mean())
residues_noreg = y-y_pred_noreg

clf_reg = MLPRegressor(hidden_layer_sizes=(3,2),activation="relu",random_state=0, alpha=0.01).fit(X.values, y.values)
y_pred_reg = cross_val_predict(clf_reg, X.values, y.values, cv=5)
scores = cross_val_score(clf_reg, X.values, y.values, cv=5)
print("Accuracy with regularization= ", scores.mean())
residues_reg = y-y_pred_reg

ax.boxplot([residues_noreg, residues_reg])
ax.set_xticklabels(['no regularization', 'with regularization (alpha = 0.01)'])

plt.show()