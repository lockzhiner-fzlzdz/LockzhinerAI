```bash
cd /path/to/LockzhinerVisionModule

rm lprnet.onnx
wget https://ftrg.zbox.filez.com/v2/delivery/data/95f00b0fc900458ba134f8b180b3f7a1/examples/LPRNet/lprnet.onnx

rm LZ-LPRNet.onnx
mv lprnet.onnx LZ-LPRNet.onnx

python utils/export.py \
    --config_path=./configs/LZ-LPRNet.yaml \
    --model_load_path=./LZ-LPRNet.onnx
```