import os
import json
from PIL import Image
import shutil
from tqdm import tqdm
import matplotlib.pyplot as plt


# 指定图片文件夹路径
image_root_folder = r'E:\face_mask'

# 指定输出标签文件夹路径
output_folder = './Dataset/annotations'

# 指定的输出图片保存路径
images_folder = './Dataset/images'

# 标签文本保存
flags_txt = './Dataset/flags.txt'

# 确保输出文件夹存在
os.makedirs(output_folder, exist_ok=True)
os.makedirs(images_folder, exist_ok=True)

# 动态生成文件夹与标志位的映射关系
folder_to_flag = {}
# 存储标签名
flag_names=[]


for folder_name in os.listdir(image_root_folder):
    folder_path = os.path.join(image_root_folder, folder_name)
    if os.path.isdir(folder_path):
        folder_to_flag[folder_name] = folder_name
    flag_names.append(folder_name)
with open(flags_txt,'w',encoding='utf-8') as f:
    for flag_name in flag_names:
        f.write(flag_name+'\n')
    f.close()
    print('标签文件创建成功')
# 动态生成 flags 字典
flags = {key: False for key in folder_to_flag.values()}

# 维护每个文件夹的计数器
folder_counters = {folder_name: 0 for folder_name in folder_to_flag.keys()}

# 统计每个类别的图片数量
category_counts = {folder_name: 0 for folder_name in folder_to_flag.keys()}

# 遍历图片根文件夹中的所有子文件夹
for folder_name in os.listdir(image_root_folder):
    folder_path = os.path.join(image_root_folder, folder_name)

    # 确保是一个文件夹
    if os.path.isdir(folder_path):
        # 获取文件夹中的所有文件，并计算总数
        files = [f for f in os.listdir(folder_path) if f.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif'))]
        total_files = len(files)

        # 使用 tqdm 包装迭代器，显示进度条
        for filename in tqdm(files, desc=f'Processing {folder_name}', total=total_files):
            # 构建完整的文件路径
            image_path = os.path.join(folder_path, filename)

            # 获取当前文件夹的计数器值
            counter = folder_counters[folder_name]

            # 构建新的文件名
            new_filename = f'{folder_name}_{counter:04d}.jpg'
            new_image_path = os.path.join(images_folder, new_filename)

            # 存储文件名构造
            save_path_name = os.path.join('..\\images\\', new_filename)

            # 拷贝图片到 images 文件夹
            shutil.copy(image_path, new_image_path)

            # 打开图片并获取高度和宽度
            with Image.open(image_path) as img:
                width, height = img.size

            # 初始化 flags
            current_flags = flags.copy()

            # 设置对应的标志位为 True
            flag_key = folder_to_flag.get(folder_name, None)
            if flag_key is not None:
                current_flags[flag_key] = True

            # 创建标签文件的数据结构
            label_data = {
                "version": "5.5.0",
                "flags": current_flags,
                "shapes": [],
                "imagePath": save_path_name,
                "imageData": None,
                "imageHeight": height,
                "imageWidth": width
            }

            # 构建标签文件的路径
            label_filename = os.path.splitext(new_filename)[0] + '.json'
            label_path = os.path.join(output_folder, label_filename)

            # 将标签数据写入文件
            with open(label_path, 'w') as f:
                json.dump(label_data, f, indent=4)

            # 增加计数器
            folder_counters[folder_name] += 1

            # 更新类别计数
            category_counts[folder_name] += 1

# print('Label files creation complete.')

# 输出每个类别的图片数量统计
# for category, count in category_counts.items():
#     print(f'Category {category} has {count} images.')

# 绘制柱状图
categories = list(category_counts.keys())
counts = list(category_counts.values())

plt.figure(figsize=(10, 6))
plt.bar(categories, counts, color='skyblue')
plt.xlabel('Categories')
plt.ylabel('Number of Images')
plt.title('Image Count by Category')
plt.xticks(rotation=45)
plt.tight_layout()

# 保存柱状图
plt.savefig('category_counts.png')
# plt.show()