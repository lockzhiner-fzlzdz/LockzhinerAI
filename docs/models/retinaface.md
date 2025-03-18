cd /path/to/LockzhinerVisionModule

rm LZ-Retainface*
wget https://gitee.com/LockzhinerAI/LockzhinerVisionModule/releases/download/v0.0.3/LZ-Retainface-2024-1121-1436.onnx
mv LZ-Retainface-2024-1121-1436.onnx LZ-RetinaFace.onnx

rm test.jpg
# 下载不了去 https://github.com/airockchip/rknn_model_zoo/blob/main/examples/RetinaFace/model/test.jpg 下载
wget -O test.jpg https://s21.ax1x.com/2024/11/07/pAyN4VU.jpg 

rm dataset.txt
touch dataset.txt
echo test.jpg > dataset.txt

rm LZ-RetinaFace.rknn
python utils/export.py \
    --config_path=./configs/LZ-RetinaFace.yaml \
    --model_load_path=./LZ-RetinaFace.onnx \
    --model_save_path=./LZ-RetinaFace.rknn \
    --target_platform=RV1106