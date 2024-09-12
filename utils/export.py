import os
import yaml
import argparse
from rknn.api import RKNN
from onnxslim import slim
import onnx

def get_config():
    parser = argparse.ArgumentParser()
    parser.add_argument("--verbose", default="Debug", help="rknntoolkit verbose")
    parser.add_argument("--config_path", required=True, help="The path of model config file")
    parser.add_argument("--model_load_path", required=True, help="The path of onnx model file")
    parser.add_argument("--target_platform", required=True, help="The target platform")
    parser.add_argument("--model_save_path", required=True, help="The path of rknn model save")
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    config = get_config()
    with open(config.config_path) as file:
        file_data = file.read()
        yaml_config = yaml.safe_load(file_data)
    print(yaml_config)
    model = RKNN(config.verbose)

    # Prune ONNX Model
    print('--> Prune ONNX Model')
    slim_prune_onnx_model = slim(config.model_load_path, inputs=yaml_config["load_onnx"]["inputs"], outputs=yaml_config["load_onnx"]["outputs"])
    onnx.save(slim_prune_onnx_model, "/tmp/slim_prune_onnx_model.onnx")
    print('done')

    # Config
    print('--> Config')
    mean_values = [255 * mean for mean in yaml_config["config"]["mean"]]
    std_values = [255 * std for std in yaml_config["config"]["std"]]
    target_platform = config.target_platform
    model.config(mean_values=mean_values, std_values=std_values, target_platform=target_platform)
    print('done')

    # Load ONNX model
    print('--> Load ONNX model')
    model_path = "/tmp/slim_prune_onnx_model.onnx"
    inputs = yaml_config["load_onnx"]["inputs"]
    input_size_list = yaml_config["load_onnx"]["input_size_list"]
    outputs = yaml_config["load_onnx"]["outputs"]
    ret = model.load_onnx(model=model_path, inputs=inputs, input_size_list=input_size_list, outputs=outputs)
    assert ret == 0, "Load model failed!"
    print('done')

    # Build model
    print('--> Build model')
    do_quantization = yaml_config["build"]["do_quantization"]
    dataset = yaml_config["build"]["dataset"]
    ret = model.build(do_quantization=do_quantization, dataset=dataset)
    assert ret == 0, "Build model failed!"
    print('done')

    # Init Runtime
    print('--> Init Runtime')
    ret = model.init_runtime()
    assert ret == 0, "Init runtime environment failed!"
    print('done')

    # Export
    model_save_path = config.model_save_path
    ret = model.export_rknn(model_save_path)
    assert ret == 0, "Export rknn model failed!"
    print("Export OK!")