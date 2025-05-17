// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "yolov5.h"

static void dump_tensor_attr(rknn_tensor_attr *attr)
{
    printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
           "zp=%d, scale=%f\n",
           attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
           attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
           get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

int init_yolov5_model(const char *model_path, rknn_app_context_t *app_ctx)
{
    int ret;
    int model_len = 0;
    char *model;
    rknn_context ctx = 0;

    ret = rknn_init(&ctx, (char *)model_path, 0, 0, NULL);
    if (ret < 0)
    {
        printf("rknn_init fail! ret=%d\n", ret);
        return -1;
    }

    // Get Model Input Output Number
    rknn_input_output_num io_num;
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret != RKNN_SUCC)
    {
        printf("rknn_query fail! ret=%d\n", ret);
        return -1;
    }
    //printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

    // Get Model Input Info
    //printf("input tensors:\n");
    rknn_tensor_attr input_attrs[io_num.n_input];
    memset(input_attrs, 0, sizeof(input_attrs));
    for (int i = 0; i < io_num.n_input; i++)
    {
        input_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_NATIVE_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            printf("rknn_query fail! ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&(input_attrs[i]));
    }

    // Get Model Output Info
    //printf("output tensors:\n");
    rknn_tensor_attr output_attrs[io_num.n_output];
    memset(output_attrs, 0, sizeof(output_attrs));
    for (int i = 0; i < io_num.n_output; i++)
    {
        output_attrs[i].index = i;
        //When using the zero-copy API interface, query the native output tensor attribute
        ret = rknn_query(ctx, RKNN_QUERY_NATIVE_NHWC_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            printf("rknn_query fail! ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&(output_attrs[i]));
    }

    // default input type is int8 (normalize and quantize need compute in outside)
    // if set uint8, will fuse normalize and quantize to npu
    input_attrs[0].type = RKNN_TENSOR_UINT8;
    // default fmt is NHWC,1106 npu only support NHWC in zero copy mode
    input_attrs[0].fmt = RKNN_TENSOR_NHWC;
    //printf("input_attrs[0].size_with_stride=%d\n", input_attrs[0].size_with_stride);
    app_ctx->input_mems[0] = rknn_create_mem(ctx, input_attrs[0].size_with_stride);

    // Set input tensor memory
    ret = rknn_set_io_mem(ctx, app_ctx->input_mems[0], &input_attrs[0]);
    if (ret < 0) {
        printf("input_mems rknn_set_io_mem fail! ret=%d\n", ret);
        return -1;
    }

    // Set output tensor memory
    for (uint32_t i = 0; i < io_num.n_output; ++i) {
        app_ctx->output_mems[i] = rknn_create_mem(ctx, output_attrs[i].size_with_stride);
        ret = rknn_set_io_mem(ctx, app_ctx->output_mems[i], &output_attrs[i]);
        if (ret < 0) {
            printf("output_mems rknn_set_io_mem fail! ret=%d\n", ret);
            return -1;
        }
    }

    // Set to context
    app_ctx->rknn_ctx = ctx;

    // TODO
    if (output_attrs[0].qnt_type == RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC)
    {
        app_ctx->is_quant = true;
    }
    else
    {
        app_ctx->is_quant = false;
    }

    app_ctx->io_num = io_num;
    app_ctx->input_attrs = (rknn_tensor_attr *)malloc(io_num.n_input * sizeof(rknn_tensor_attr));
    memcpy(app_ctx->input_attrs, input_attrs, io_num.n_input * sizeof(rknn_tensor_attr));
    app_ctx->output_attrs = (rknn_tensor_attr *)malloc(io_num.n_output * sizeof(rknn_tensor_attr));
    memcpy(app_ctx->output_attrs, output_attrs, io_num.n_output * sizeof(rknn_tensor_attr));

    if (input_attrs[0].fmt == RKNN_TENSOR_NCHW) 
    {
        printf("model is NCHW input fmt\n");
        app_ctx->model_channel = input_attrs[0].dims[1];
        app_ctx->model_height  = input_attrs[0].dims[2];
        app_ctx->model_width   = input_attrs[0].dims[3];
    } else 
    {
        printf("model is NHWC input fmt\n");
        app_ctx->model_height  = input_attrs[0].dims[1];
        app_ctx->model_width   = input_attrs[0].dims[2];
        app_ctx->model_channel = input_attrs[0].dims[3];
    } 

    printf("model input height=%d, width=%d, channel=%d\n",
           app_ctx->model_height, app_ctx->model_width, app_ctx->model_channel);

    return 0;
}

int release_yolov5_model(rknn_app_context_t *app_ctx)
{
    if (app_ctx->input_attrs != NULL)
    {
        free(app_ctx->input_attrs);
        app_ctx->input_attrs = NULL;
    }
    if (app_ctx->output_attrs != NULL)
    {
        free(app_ctx->output_attrs);
        app_ctx->output_attrs = NULL;
    }
    for (int i = 0; i < app_ctx->io_num.n_input; i++) {
        if (app_ctx->input_mems[i] != NULL) {
            rknn_destroy_mem(app_ctx->rknn_ctx, app_ctx->input_mems[i]);
        }
    }
    for (int i = 0; i < app_ctx->io_num.n_output; i++) {
        if (app_ctx->output_mems[i] != NULL) {
            rknn_destroy_mem(app_ctx->rknn_ctx, app_ctx->output_mems[i]);
        }
    }
    if (app_ctx->rknn_ctx != 0)
    {
        rknn_destroy(app_ctx->rknn_ctx);

        app_ctx->rknn_ctx = 0;
    }

    printf("Release success\n");
    return 0;
}

int inference_yolov5_model(rknn_app_context_t *app_ctx,  object_detect_result_list *od_results)
{   
    printf("Inference start\n");
    int ret;
    const float nms_threshold = NMS_THRESH;      // 默认的NMS阈值
    const float box_conf_threshold = BOX_THRESH; // 默认的置信度阈值
    printf("rknn_run\n");
    ret = rknn_run(app_ctx->rknn_ctx, nullptr);
    printf("rknn_run end\n");
    printf("rknn_run ret=%d\n", ret);
    if (ret < 0) {
        printf("rknn_run fail! ret=%d\n", ret);
        return -1;
    }
    printf("post_process");
    // Post Process
    post_process(app_ctx, app_ctx->output_mems,  box_conf_threshold, nms_threshold, od_results);
out:
    return ret;
}

/*
预处理
图片填充缩放
*/
float scale;
extern int model_width;
extern int model_height;

extern int width;
extern int height;

extern int leftPadding;
extern int topPadding;
cv::Mat letterbox(cv::Mat input)
{
	float scaleX = (float)model_width / (float)width;
	float scaleY = (float)model_height / (float)height;
	scale = scaleX < scaleY ? scaleX : scaleY;

	int inputWidth = (int)((float)width * scale);
	int inputHeight = (int)((float)height * scale);

	leftPadding = (model_width - inputWidth) / 2;
	topPadding = (model_height - inputHeight) / 2;

	cv::Mat inputScale;
	cv::resize(input, inputScale, cv::Size(inputWidth, inputHeight), 0, 0, cv::INTER_LINEAR);
	cv::Mat letterboxImage(model_width, model_width, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Rect roi(leftPadding, topPadding, inputWidth, inputHeight);
	inputScale.copyTo(letterboxImage(roi));

	return letterboxImage;
}
/*
将经过 letterbox 处理后的坐标映射回原始图像坐标系
*/
void mapCoordinates(int *x, int *y)
{
	int mx = *x - leftPadding;
	int my = *y - topPadding;

	*x = (int)((float)mx / scale);
	*y = (int)((float)my / scale);
}

void draw_detections(int count,						 
					 object_detect_result *results,	 
					 cv::Mat &frame,				 
					 void (*mapCoord)(int *, int *)) 
{
	char text[128];
	for (int i = 0; i < count; i++)
	{
		object_detect_result *det = &results[i];

		// 提取坐标
		int sX = (int)det->box.left;
		int sY = (int)det->box.top;
		int eX = (int)det->box.right;
		int eY = (int)det->box.bottom;

		// 应用坐标映射
		mapCoord(&sX, &sY);
		mapCoord(&eX, &eY);

		// 绘制检测框
		cv::rectangle(frame,
					  cv::Point(sX, sY),
					  cv::Point(eX, eY),
					  cv::Scalar(0, 255, 0), 1);

		// 生成标签文本
		sprintf(text, "%s %.1f%%",
				coco_cls_to_name(det->cls_id),
				det->prop * 100);

		// 绘制文本
		cv::putText(frame, text,
					cv::Point(sX, sY - 8),
					cv::FONT_HERSHEY_SIMPLEX, 1,
					cv::Scalar(0, 255, 0), 1);
	}
}