import os
img_path = os.listdir("../data")
print(len(img_path))
path = []
for i in range(len(img_path)):
    img_path[i] = "../data/" + img_path[i]
    path.append(img_path[i])
    print(img_path[i])
with open("dataset.txt", "w") as f:
    for i in range(len(path)):
        f.write(path[i] + "\n")
