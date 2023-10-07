from ultralytics import YOLO
from PIL import Image


model = YOLO("turtle_bestv8n_openvino_model/",task="detect")


results = model.predict("./test/images/Turtle_610.jpg")
result = results[0]

# Draw detections
im = Image.fromarray(result.plot()[:,:,::-1])
im.show()