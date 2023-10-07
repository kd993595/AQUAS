from ultralytics import YOLO

model = YOLO("yolov8n.pt")#determines model of YOLOv8 to use [n,s,m,l,x]
model.train(data="data.yaml", epochs=20,batch=16)
