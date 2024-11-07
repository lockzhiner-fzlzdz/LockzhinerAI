```bash
cd /path/to/LockzhinerVisionModule

rm RetinaFace_mobile320.onnx
wget https://ftrg.zbox.filez.com/v2/delivery/data/95f00b0fc900458ba134f8b180b3f7a1/examples/RetinaFace/RetinaFace_mobile320.onnx

rm LZ-RetinaFace.onnx
mv RetinaFace_mobile320.onnx LZ-RetinaFace.onnx

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
```