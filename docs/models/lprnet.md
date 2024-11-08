```bash
cd /path/to/LockzhinerVisionModule

rm lprnet.onnx
wget https://ftrg.zbox.filez.com/v2/delivery/data/95f00b0fc900458ba134f8b180b3f7a1/examples/LPRNet/lprnet.onnx

rm LZ-LPRNet.onnx
mv lprnet.onnx LZ-LPRNet.onnx

export EXPORT_MODEL_NAME=LZ-LPRNet
mkdir -p ${EXPORT_MODEL_NAME}
pnnx ${EXPORT_MODEL_NAME}.onnx \
    pnnxparam=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.pnnx.param \
    pnnxbin=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.pnnx.bin \
    pnnxpy=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.py \
    pnnxonnx=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.pnnx.onnx \
    ncnnparam=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.ncnn.param \
    ncnnbin=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.ncnn.bin \
    ncnnpy=${EXPORT_MODEL_NAME}/${EXPORT_MODEL_NAME}.py \
    inputshape=[1,3,24,94]

zip -r -9 ${EXPORT_MODEL_NAME}.zip ${EXPORT_MODEL_NAME}
```