from scipy.io import arff
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import ConfusionMatrixDisplay
import matplotlib.pyplot as plt
from sklearn.model_selection import cross_val_predict, cross_val_score

data = arff.loadarff('breast.w_modified.arff')
df = pd.DataFrame(data[0])

X = df.iloc[:, 0:9]
y = df.iloc[:, -1]
y = y.astype('string')

def make_cm(es):
    clf = MLPClassifier(hidden_layer_sizes=(3,2),activation="relu",early_stopping=es,random_state=0,alpha=0.0001,max_iter=2000).fit(X.values, y)
    y_pred = cross_val_predict(clf, X.values, y, cv=5)
    scores = cross_val_score(clf, X.values, y, cv=5)
    print("Accuracy when es =", es, ":", scores.mean())

    ConfusionMatrixDisplay.from_predictions(y, y_pred)
    plt.title("early_stopping =" + str(es))

make_cm(False)
make_cm(True)
plt.show()