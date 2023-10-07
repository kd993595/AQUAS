"""
Only run on raspberry pi
Raspberry Pi file for testing video recording on raspberry pi
"""
from picamera2 import Picamera2, Preview, MappedArray
from picamera2.encoders import H264Encoder
import time, cv2

picam2 = Picamera2()
picam2.start_preview(Preview.QTGL,x=100,y=100,width=960,height=720)
video_config = picam2.create_video_configuration({"size":(960,720)})
picam2.configure(video_config)


encoder = H264Encoder()
output = "test.h264"

color = (0,255,0)
origin = (0,30)
font = cv2.FONT_HERSHEY_SIMPLEX
scale = 1
thickness = 2

def apply_timestamp(request):
    timestamp = time.strftime("%Y-%m-%d %X")
    with MappedArray(request,"main") as m:
        cv2.putText(m.array,timestamp,origin,font,scale,color,thickness)

picam2.post_callback = apply_timestamp
picam2.start_recording(encoder,output)
time.sleep(10)
picam2.stop_recording()