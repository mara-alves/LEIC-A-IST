from scipy.io import arff
import pandas as pd
import numpy as np
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.model_selection import validation_curve
import matplotlib.pyplot as plt

data = arff.loadarff('breast.w_modified.arff')
df = pd.DataFrame(data[0])

X = df.iloc[:, 0:9]
y = df.iloc[:, -1]
y = y.astype('string')

X_train, X_test, y_train, y_test = train_test_split(X, y)

param_range = np.arange(1, 10, 2)

train_score, test_score = validation_curve(DecisionTreeClassifier(), X, y,
                                       param_name = "max_features",
                                       param_range = param_range,
                                       scoring = "accuracy")

# Calculating mean and standard deviation of training score
mean_train_score = np.mean(train_score, axis = 1)
std_train_score = np.std(train_score, axis = 1)
 
# Calculating mean and standard deviation of testing score
mean_test_score = np.mean(test_score, axis = 1)
std_test_score = np.std(test_score, axis = 1)
 
# Plot mean accuracy scores for training and testing scores
plt.plot(param_range, mean_train_score,
     label = "Training Score", color = 'b')
plt.plot(param_range, mean_test_score,
   label = "Testing Score", color = 'g')
 
# Creating the plot
plt.xticks(param_range, param_range)
plt.title("Validation Curve with Decision Tree Classifier")
plt.xlabel("Number of Features")
plt.ylabel("Accuracy")
plt.tight_layout()
plt.legend(loc = 'best')
plt.show()
plt.savefig("featuresplot.png")