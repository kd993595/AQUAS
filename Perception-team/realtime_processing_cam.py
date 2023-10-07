"""
Only run on raspberry pi
Code for realtime object detection processing
Using model "turtle_bestv8s.pt" which is running around projected ~.41fps
Not functional at current speed of inference time
"""
from picamera2 import Picamera2, MappedArray
from picamera2.encoders import H264Encoder
from picamera2.outputs import FfmpegOutput
import cv2, time, numpy
from ultralytics import YOLO
import gc

counter = 0
model = YOLO("turtle_bestv8s.pt")
n_alphachannel = numpy.full([720,960,1],255,dtype=int)
picam2 = Picamera2()
cv2.startWindowThread()

video_config = picam2.create_video_configuration({"size":(960,720)})
picam2.configure(video_config)
picam2.video_configuration.controls['FrameDurationLimits'] = (33333,33333)
#print(picam2.video_configuration.controls['FrameDurationLimits'])

encoder = H264Encoder()
output = FfmpegOutput('test.mpg')

color = (0,255,0)
origin = (0,30)
font = cv2.FONT_HERSHEY_SIMPLEX
scale = 1
thickness = 2


def apply_timestamp(request):
    global counter
    counter += 1
    #timestamp = time.strftime("%Y-%m-%d %X")
    with MappedArray(request,"main") as m:
        #print(m.array.shape)
        #results = model(m.array[:,:,:3])
        if not(counter%30==0 and counter>1):
            return
        x = cv2.cvtColor(m.array,cv2.COLOR_RGBA2RGB)        
        results = model.predict(x,conf=0.65)
        #print(len(results))
        #cv2.putText(m.array,timestamp,origin,font,scale,color,thickness)
        if len(results)>0:
            result = results[0]
            for box in result.boxes:
                cords = [round(x) for x in box.xyxy[0].tolist()]
                cv2.rectangle(m.array,(cords[0],cords[1]),(cords[2],cords[3]),(0,0,255),1)
        
picam2.pre_callback = apply_timestamp
time.sleep(2)
picam2.start_recording(encoder,output)
#time.sleep(10)

timeout = 10 #[seconds]
timeout_start = time.time()
while time.time() < timeout_start+timeout:
    im = picam2.capture_array()
    cv2.imshow("camera",im)
    cv2.waitKey(5)


picam2.stop_recording()
cv2.destroyAllWindows()
print(counter/(time.time()-timeout_start))
print(counter)