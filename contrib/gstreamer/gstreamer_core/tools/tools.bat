gst-launch filesrc location=c:\\data\\warning.wav ! wavparse ! audioconvert ! audioresample ! devsoundsink
gst-launch filesrc location=c:\\data\\Crnival.mp3 !  devsoundsink
gst-launch filesrc location=c:\\data\\test.raw !  devsoundsink
