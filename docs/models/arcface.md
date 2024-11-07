```bash
cd /path/to/LockzhinerVisionModule

rm -rf mobileface_v1.0_infer.tar
wget https://paddle-model-ecology.bj.bcebos.com/model/insight-face/mobileface_v1.0_infer.tar
tar -xvf mobileface_v1.0_infer.tar

rm LZ-ArcFace.onnx
paddle2onnx --model_dir mobileface_v1.0_infer \
            --model_filename inference.pdmodel \
            --params_filename inference.pdiparams \
            --save_file LZ-ArcFace.onnx \
            --deploy_backend rknn

rm face_demo.zip
rm face_0.jpg
rm face_1.jpg
rm face_2.jpg
wget https://bj.bcebos.com/paddlehub/fastdeploy/rknpu2/face_demo.zip
unzip face_demo.zip

rm dataset.txt
touch dataset.txt
echo face_0.jpg > dataset.txt
echo face_1.jpg > dataset.txt
echo face_2.jpg > dataset.txt

rm LZ-ArcFace.rknn
python utils/export.py \
    --config_path=./configs/LZ-ArcFace.yaml \
    --model_load_path=./LZ-ArcFace.onnx \
    --model_save_path=./LZ-ArcFace.rknn \
    --target_platform=RV1106
```