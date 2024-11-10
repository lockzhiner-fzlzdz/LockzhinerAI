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
    parser.add_argument("--target_platform", required=False, help="The target platform")
    parser.add_argument("--model_save_path", required=False, help="The path of rknn model save"
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

    # Prune ONNX Model
    print("--> Prune ONNX Model")
    # "input:1,3,224,224"
    input_shapes = []
    assert len(config_input_size_list) == len(
        config_inputs
    ), "len(config_input_size_list) != len(config_inputs)"
    for i in range(len(config_inputs)):
        shape_string = ",".join(map(str, config_input_size_list[i]))
        input_shapes.append(config_inputs[i] + ":" + shape_string)
    print(input_shapes)

    if config_outputs is not None:
        slim_prune_onnx_model = slim(
            config.model_load_path, input_shapes=input_shapes, outputs=config_outputs
        )
    else:
        slim_prune_onnx_model = slim(config.model_load_path, input_shapes=input_shapes)
    onnx.save(slim_prune_onnx_model, "/tmp/slim_prune_onnx_model.onnx")
    print("done")

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
    target_platform = config.target_platform
    model.config(
        mean_values=mean_values, std_values=std_values, target_platform=target_platform
    )
    print("done")

    # Load ONNX model
    print("--> Load ONNX model")
    model_path = "/tmp/slim_prune_onnx_model.onnx"
    ret = model.load_onnx(model=model_path)
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