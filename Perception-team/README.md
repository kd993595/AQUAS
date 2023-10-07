# ObjectDetectionYolo

main.py - holds sample code for a model.predict() with image passed through

train.py - holds code used for training model using ultralytics own model as basis and data pointed from 'data.yaml'

detection_onnx.py - testing code for using different types of model configuration (ie. onnx,openvino,torchlite)

current model "turtle_bestv8n.pt" trained on image data runs ~113ms per inference mAP50-95 = 0.69045

"yolov8n(s).pt" models that come from ultralytics package trained off COCO dataset

Current testing with openvino and onnx for quantized version of "turtle_bestv8n.pt" reduces inference time to ~60ms on laptop not tested on raspberry pi
