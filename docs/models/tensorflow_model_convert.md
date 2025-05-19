# tensorflow 模型训练及转换

```markdown
由于 TensorFlow 在 Windows 下编译安装比较麻烦，推荐使用 Docker 容器进行模型训练及转化。
- **参考网址**: [TensorFlow Docker 安装指南](https://www.tensorflow.org/install/docker?hl=zh-cn)
- **注意**: Docker 容器的拉取安装可以参考本仓库的开发环境搭建文档
- **注意**: Docker 容器的拉取安装可以参考本仓库的开发环境搭建文档

```

## 模型训练及转换例子

### 训练模型：

以下代码展示了如何使用 MobileNetV2 进行图像分类模型的训练。

- **数据预处理**：利用 `ImageDataGenerator` 对图像进行缩放和分割成训练集与验证集。
- **模型构建**：基于预训练的 MobileNetV2 构建自己的分类模型，并通过冻结基础模型层来微调模型。
- **模型训练**：在训练数据上训练模型，并在验证集上评估其性能。
- **导出模型**：训练完成后，将模型导出以便后续使用或部署。

```python
import tensorflow as tf
from tensorflow.keras import layers, models, regularizers
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.preprocessing.image import ImageDataGenerator

# 假设你有两类或更多类别的图像，每类放在单独的文件夹中，如 'dataset/1/', 'dataset/2/' ...

IMG_SIZE = 224  

# 数据预处理
datagen = ImageDataGenerator(
    rescale=1./255,  # 缩放像素值到0-1之间
    validation_split=0.2  # 分割数据集，这里使用20%作为验证集
)

train_generator = datagen.flow_from_directory(
    './catdog',  # 根目录
    target_size=(IMG_SIZE, IMG_SIZE),
    batch_size=32,
    class_mode='sparse',
    subset='training'  # 使用训练集
) 

validation_generator = datagen.flow_from_directory(
    './catdog',
    target_size=(IMG_SIZE, IMG_SIZE),
    batch_size=32,
    class_mode='sparse',
    subset='validation'  # 使用验证集
)

# 构建 MobileNetV2 模型
base_model = MobileNetV2(
    input_shape=(IMG_SIZE, IMG_SIZE, 3),
    include_top=False,
    weights='imagenet',
    alpha=0.75
)

base_model.trainable = False

global_avg_pooling = layers.GlobalAveragePooling2D()(base_model.output)
dense1 = layers.Dense(128, activation='relu', kernel_regularizer=regularizers.l2(1e-4))(global_avg_pooling)
output = layers.Dense(train_generator.num_classes, activation='softmax')(dense1)  # 动态获取类别数

model = models.Model(inputs=base_model.input, outputs=output)

# 编译模型
model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])

# 训练模型
history = model.fit(
    train_generator,
    epochs=10,  # 示例中仅训练一个epoch，实际应用中应根据情况调整
    validation_data=validation_generator
)

# 评估模型（可选）
# 注意：ImageDataGenerator生成器不支持evaluate方法，需要手动加载测试集进行评估

# 导出模型
model.export('./cd')
```

### 推理预测

```python
import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import numpy as np
from sklearn.metrics import classification_report, accuracy_score

# 参数设置
IMG_SIZE = 224  # 确保与训练时一致
BATCH_SIZE = 32

# 数据预处理
datagen = ImageDataGenerator(
    rescale=1./255,  # 缩放像素值到0-1之间
)

# 加载并预处理测试数据
test_generator = datagen.flow_from_directory(
    './catdog',  # 替换为你的测试数据目录
    target_size=(IMG_SIZE, IMG_SIZE),
    batch_size=BATCH_SIZE,
    class_mode='sparse',
    shuffle=False  # 关闭shuffle确保顺序正确
)

# 获取一批数据看看输入形状
sample_input, _ = next(test_generator)
input_shape = sample_input.shape[1:]  # 应该是 (224, 224, 3)

# 构建包含 TFSMLayer 的 Keras 模型
model = tf.keras.Sequential([
    tf.keras.Input(shape=input_shape),
    tf.keras.layers.TFSMLayer('./cd', call_endpoint='serving_default')
])

# 进行预测
predictions = model.predict(test_generator)

# 查看 predictions 结构（调试用）
print("Raw predictions type:", type(predictions))
if isinstance(predictions, dict):
    print("Keys in predictions:", predictions.keys())
    predictions = next(iter(predictions.values()))  # 提取第一个输出作为 logits

# 判断 predictions 维度，决定是否需要 argmax
if predictions.ndim == 2:
    predicted_classes = np.argmax(predictions, axis=1)
else:
    predicted_classes = predictions.astype(int).flatten()

# 获取真实标签
true_classes = test_generator.classes

# 验证预测与真实标签维度是否匹配
assert len(predicted_classes) == len(true_classes), \
    f"Length mismatch: predicted_classes={len(predicted_classes)}, true_classes={len(true_classes)}"

# 计算准确率
accuracy = accuracy_score(true_classes, predicted_classes)
print(f"Test Accuracy: {accuracy * 100:.2f}%")

# 分类报告
target_names = list(test_generator.class_indices.keys())  # 动态获取类别名称
print("\nClassification Report:")
print(classification_report(true_classes, predicted_classes, target_names=target_names))
```

### 模型转换:导出为.pb

此部分介绍了如何将训练好的 Keras 模型转换为冻结的 `.pb` 文件格式，适用于需要在其他平台（如 TensorFlow Serving）上部署的情况。

- **变量转常量**：将模型中的所有变量转换为常量以确保它们在模型被加载时不会改变。

```python
import tensorflow as tf
from tensorflow.python.framework.convert_to_constants import convert_variables_to_constants_v2

# 输入SavedModel路径和输出.pb文件名
input_saved_model_dir = "./cd/"
output_node_names = ["output_0"]  # 替换为实际输出节点名
output_graph_filename = 'frozen_graph.pb'

# 加载 SavedModel
saved_model = tf.saved_model.load(input_saved_model_dir)

# 获取默认签名函数
concrete_function = saved_model.signatures[tf.saved_model.DEFAULT_SERVING_SIGNATURE_DEF_KEY]

# 转换变量为常量
frozen_func = convert_variables_to_constants_v2(concrete_function)

# 获取 GraphDef 并保存为 .pb 文件
graph_def = frozen_func.graph.as_graph_def()

with tf.io.gfile.GFile(output_graph_filename, "wb") as f:
    f.write(graph_def.SerializeToString())

print(f"Frozen graph saved to {output_graph_filename}")
```

导出为 tflite 模型

TFLite 是一种轻量级的模型格式，非常适合移动设备和嵌入式系统。该段落展示了如何从 SavedModel 转换到 TFLite 格式。

```python
import tensorflow as tf

# 定义SavedModel的路径
saved_model_dir = './catdog'

# 创建转换器对象
converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)

# 如果需要的话，可以设置优化选项等参数
# converter.optimizations = [tf.lite.Optimize.DEFAULT]

# 执行转换
tflite_model = converter.convert()

# 保存转换后的模型到文件
with open('digitalhand.tflite', 'wb') as f:
    f.write(tflite_model)
```

### 导出成onnx模型

```bash
python -m tf2onnx.convert --saved-model ./pb_model  --output ./test.onnx --opset 13
```

### 固定onnx输入

```python
import onnx

# 加载 ONNX 模型
model_path = 'mobile.onnx'
model = onnx.load(model_path)

# 获取输入节点
input_tensor = model.graph.input[0]

# 修改输入节点的形状
batch_size = 1  # 将批次大小固定为 1
new_shape = [batch_size, 224, 224, 3]  # 假设你的输入形状是 [batch_size, 224, 224, 3]

# 更新输入节点的形状
dim_proto = input_tensor.type.tensor_type.shape.dim
dim_proto[0].dim_value = batch_size  # 固定批次大小

# 保存修改后的模型
output_model_path = 'mobile.onnx'
onnx.save(model, output_model_path)

print(f"Model with fixed batch size saved to {output_model_path}")
```

## rknn 转换测试

config文件内容

```markdowm
load_onnx:
  input_size_list:
    -
      - 1
      - 224
      - 224
      - 3
  outputs:
    - output_0

config:
  # RGB
  mean:
    - 0.406
    - 0.456
    - 0.485
  std:
    - 0.225
    - 0.224
    - 0.229
  type: "fp32"
  quantized_algorithm: mmse
  quantized_dtype: w16a16i
  optimization_level: 0
build:
  do_quantization: True
  dataset: ./dataset.txt
```

### pb转rknn

```python
import os
import yaml
import argparse
from rknn.api import RKNN
import onnx


def get_config():
    parser = argparse.ArgumentParser()
    parser.add_argument("--verbose", default="Debug", help="rknntoolkit verbose")
    parser.add_argument(
        "--config_path", required=True, help="The path of model config file"
    )
    parser.add_argument(
        "--model_load_path", required=True, help="The path of onnx model file"
    )
    parser.add_argument("--target_platform", required=False, help="The target platform")
    parser.add_argument(
        "--model_save_path", required=False, help="The path of rknn model save"
    )
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    config = get_config()
    with open(config.config_path) as file:
        file_data = file.read()
        yaml_config = yaml.safe_load(file_data)
    print(yaml_config)

    # config_inputs = [
    #     str(input_name) for input_name in yaml_config["load_onnx"]["inputs"]
    # ]
    config_input_size_list = yaml_config["load_onnx"]["input_size_list"]
    config_outputs = [
        str(output_name) for output_name in yaml_config["load_onnx"]["outputs"]
    ]
    model = RKNN(config.verbose)
    # Config
    print("--> Config")
    mean_std_type = yaml_config["config"]["type"]
    if mean_std_type == "fp32":
        mean_values = [255 * mean for mean in yaml_config["config"]["mean"]]
        std_values = [255 * std for std in yaml_config["config"]["std"]]
    elif mean_std_type == "int8":
        mean_values = yaml_config["config"]["mean"]
        std_values = yaml_config["config"]["std"]
    else:
        assert False, f"The type({mean_std_type}) is error, need fp32/int8."
    print(f"mean_values is {mean_values}, std_values is {std_values}")

    quantized_dtype = "w8a8"
    if "quantized_dtype" in yaml_config["config"]:
        quantized_dtype = yaml_config["config"]["quantized_dtype"]
    print(f"quantized_dtype is {quantized_dtype}")

    quantized_algorithm = "normal"
    if "quantized_algorithm" in yaml_config["config"]:
        quantized_algorithm = yaml_config["config"]["quantized_algorithm"]
    print(f"quantized_algorithm is {quantized_algorithm}")

    optimization_level = 3
    if "optimization_level" in yaml_config["config"]:
        optimization_level = yaml_config["config"]["optimization_level"]
    print(f"optimization_level is {optimization_level}")

    target_platform = config.target_platform
    print(f"target_platform is {target_platform}")
    model.config(
        mean_values=mean_values,
        std_values=std_values,
        quantized_dtype=quantized_dtype,
        quantized_algorithm=quantized_algorithm,
        optimization_level=optimization_level,
        target_platform=target_platform,
    )
    print("done")

    # Load ONNX model
    print("--> Load ONNX model")
    model_path = config.model_load_path
    ret = model.load_tensorflow(tf_pb='./frozen_graph.pb',inputs=['input_1'],outputs=['StatefulPartitionedCall/model/dense_1/BiasAdd'],input_size_list=[[1,224,224,3]])
    assert ret == 0, "Load model failed!"
    print("done")

    # Build model
    print("--> Build model")
    do_quantization = yaml_config["build"]["do_quantization"]
    dataset = yaml_config["build"]["dataset"]
    ret = model.build(do_quantization=do_quantization, dataset=dataset)
    assert ret == 0, "Build model failed!"
    print("done")

    # Init Runtime
    print("--> Init Runtime")
    ret = model.init_runtime()
    assert ret == 0, "Init runtime environment failed!"
    print("done")

    # Export
    model_save_path = config.model_save_path
    ret = model.export_rknn(model_save_path)
    assert ret == 0, "Export rknn model failed!"
    print("Export OK!")


```

### tflite 转rknn

```python
import os
import yaml
import argparse
from rknn.api import RKNN
import onnx


def get_config():
    parser = argparse.ArgumentParser()
    parser.add_argument("--verbose", default="Debug", help="rknntoolkit verbose")
    parser.add_argument(
        "--config_path", required=True, help="The path of model config file"
    )
    parser.add_argument(
        "--model_load_path", required=True, help="The path of onnx model file"
    )
    parser.add_argument("--target_platform", required=False, help="The target platform")
    parser.add_argument(
        "--model_save_path", required=False, help="The path of rknn model save"
    )
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    config = get_config()
    with open(config.config_path) as file:
        file_data = file.read()
        yaml_config = yaml.safe_load(file_data)
    print(yaml_config)

    config_inputs = [
        str(input_name) for input_name in yaml_config["load_onnx"]["inputs"]
    ]
    config_input_size_list = yaml_config["load_onnx"]["input_size_list"]
    config_outputs = [
        str(output_name) for output_name in yaml_config["load_onnx"]["outputs"]
    ]
    model = RKNN(config.verbose)
    # Config
    print("--> Config")
    mean_std_type = yaml_config["config"]["type"]
    if mean_std_type == "fp32":
        mean_values = [255 * mean for mean in yaml_config["config"]["mean"]]
        std_values = [255 * std for std in yaml_config["config"]["std"]]
    elif mean_std_type == "int8":
        mean_values = yaml_config["config"]["mean"]
        std_values = yaml_config["config"]["std"]
    else:
        assert False, f"The type({mean_std_type}) is error, need fp32/int8."
    print(f"mean_values is {mean_values}, std_values is {std_values}")

    quantized_dtype = "w8a8"
    if "quantized_dtype" in yaml_config["config"]:
        quantized_dtype = yaml_config["config"]["quantized_dtype"]
    print(f"quantized_dtype is {quantized_dtype}")

    quantized_algorithm = "normal"
    if "quantized_algorithm" in yaml_config["config"]:
        quantized_algorithm = yaml_config["config"]["quantized_algorithm"]
    print(f"quantized_algorithm is {quantized_algorithm}")

    optimization_level = 3
    if "optimization_level" in yaml_config["config"]:
        optimization_level = yaml_config["config"]["optimization_level"]
    print(f"optimization_level is {optimization_level}")

    target_platform = config.target_platform
    print(f"target_platform is {target_platform}")
    model.config(
        mean_values=mean_values,
        std_values=std_values,
        quantized_dtype=quantized_dtype,
        quantized_algorithm=quantized_algorithm,
        optimization_level=optimization_level,
        target_platform=target_platform,
    )
    print("done")

    # Load ONNX model
    print("--> Load ONNX model")
    model_path = config.model_load_path
    ret = model.load_tflite(model=model_path)
    assert ret == 0, "Load model failed!"
    print("done")

    # Build model
    print("--> Build model")
    do_quantization = yaml_config["build"]["do_quantization"]
    dataset = yaml_config["build"]["dataset"]
    ret = model.build(do_quantization=do_quantization, dataset=dataset)
    assert ret == 0, "Build model failed!"
    print("done")

    # Init Runtime
    print("--> Init Runtime")
    ret = model.init_runtime()
    assert ret == 0, "Init runtime environment failed!"
    print("done")

    # Export
    model_save_path = config.model_save_path
    ret = model.export_rknn(model_save_path)
    assert ret == 0, "Export rknn model failed!"
    print("Export OK!")


```

**推理测试**

```cpp
#include <lockzhiner_vision_module/edit/edit.h>
#include <opencv2/opencv.hpp>
#include <lockzhiner_vision_module/vision/deep_learning/runtime/rknpu2_backend/rknpu2_backend.h>
#include <lockzhiner_vision_module/core/logger.h>
#include <lockzhiner_vision_module/core/memmory.h>
#include <lockzhiner_vision_module/core/quantization.h>
#include <lockzhiner_vision_module/vision/deep_learning/utils/softmax.h>
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        LOGGER_ERROR("Usage: ./mobilenet model img_path");
        return -1;
    }


const std::string model_path = argv[1];
lockzhiner_vision_module::vision::RKNPU2Backend backend;

if (!backend.Initialize(model_path))
{
    LOGGER_ERROR("Initialize failed");
    return -1;
}

cv::Mat img = cv::imread(argv[2]);
if (img.empty())
{
    LOGGER_ERROR("img is empty");
    return -1;
}

// 转换为 RGB 格式（如果模型输入是 BGR，则删除此行）
// cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

// 转换为浮点型 [0, 1]
img.convertTo(img, CV_32F, 1.0f / 255.0f);

// 拆分图像通道
std::vector<cv::Mat> channels;
cv::split(img, channels);

// 定义 mean 和 std（根据你的配置）
std::vector<float> mean = {0.485f, 0.456f, 0.406f}; // 注意顺序是 RGB
std::vector<float> std = {0.229f, 0.224f, 0.225f};

// 对每个通道做标准化处理
for (int c = 0; c < channels.size(); ++c)
{
    channels[c] = (channels[c] - mean[c]) / std[c];
}

// 合并回多通道图像
cv::Mat normalized_img;
cv::merge(channels, normalized_img);

// 缩放图像到网络输入尺寸
int input_w = backend.GetInputTensor(0).GetDims()[2]; // Width
int input_h = backend.GetInputTensor(0).GetDims()[1]; // Height
cv::resize(normalized_img, normalized_img, cv::Size(input_w, input_h));

// 将数据复制到输入张量
lockzhiner_vision_module::LockzhinerMemoryCopy(
    backend.GetInputTensor(0).GetData(),
    normalized_img.data,
    backend.GetInputTensor(0).GetElemsBytes());

if (!backend.Run())
{
    LOGGER_ERROR("Inference failed");
    return -1;
}

// 获取输出张量
auto output_tensor = backend.GetOutputTensor(0);

// 反量化准备
auto output_scale = output_tensor.GetScale();
auto output_zero_point = output_tensor.GetZp();

using OutputDataType = int16_t; // 或 uint8_t，取决于模型输出类型

OutputDataType *raw_output_data = static_cast<OutputDataType *>(output_tensor.GetData());

std::vector<float> output_data(output_tensor.GetElemsBytes() / sizeof(OutputDataType));

// 创建 MaxSoftmax 实例
using OutputDataType = int16_t; // 根据模型实际输出类型修改
lockzhiner_vision_module::MaxSoftmax<OutputDataType> max_softmax(output_zero_point, output_scale);

float max_value;
size_t max_index;

// 调用 MaxSoftmax 进行计算
max_softmax.Run(raw_output_data, output_data.size(), &max_value, &max_index);

LOGGER_INFO("Inference success");
std::cerr << "Predicted class index: " << max_index << std::endl;
std::cerr << "Confidence score: " << max_value << std::endl;
return 0;
}


```

**存在问题**

模型虽然能够成功导出为 **rknn**格式并推理，但模型的推理结果是错误的，暂时还没有排查到问题，