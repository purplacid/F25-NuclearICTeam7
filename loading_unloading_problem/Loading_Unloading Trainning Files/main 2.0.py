#This file is used to deploy a CV model
#This file enables object classification from a live video feed
import cv2
from ultralytics import YOLO
import math

# Load the model
yolo = YOLO("runs/detect/train13/weights/best.pt")# Paste in path to your trained model if you have one
#models are locate in runs/detect/train/weights folder


# Load the video capture
videoCap = cv2.VideoCapture(1)

#code setup

# List the diffrent types of people availible
target_Personel = "Personel" 
target_Unauthorised_Personel = "Unauthorised_Personel"

# List of diffrent flask conditions 
target_Flask = "Seal_Flask"
target_Unteathered_Flask = "Unteathered Flask "
target_Broken_Flask ="Broken_Flask"
target_Open_Flask ="Open_Flask"

# Truck type
target_truck = "Truck"
target_Unprepaired = "Unperpaired To ship"
target_preparied = "Prepared To ship"






prev_flask_centre = None

#return class id for a given class name
def get_target_id(result, target_class_name):
    target_id = None
    for class_id, class_name in result.names.items():
        if class_name == target_class_name:
            target_id = class_id
            break
    return target_id

#find centre of a detected object box
def box_center(box):
    x1, y1, x2, y2 = map(int, box.xyxy[0])
    cx = (x1 + x2) // 2
    cy = (y1 + y2) // 2
    return cx, cy

#allows you to pick out a certain zone in the video livestream using percentage of FOV
#(0,0) of a frame is at the top left
# (0,0) ───────────────►  X increases →
#   │
#   │
#   ▼ Y increases
def zone_select(x1,x2,y1,y2,frame):#input zone area as percentage of total FOV
     h, w = frame.shape[:2]
     return (int(x1*w), int(y1*h), int(x2*w), int(y2*h))
    
    

#function definition to detect specific classes counted in FOV
def detection_count(result, target_class_name):
    target_id = None
    for class_id, class_name in result.names.items():
        if class_name == target_class_name:
            target_id = class_id
            break

    if target_id is None:
        return 0
    
    count = 0
    for box in result.boxes:
        if int(box.cls[0]) == target_id:
            count +=1

    return count

#functions counting number of specific class objects in a specific zone

def location_detect(result, target_class_name, zone_rect):
    target_id = get_target_id(result, target_class_name)
    if target_id is None:
        return 0
    count = 0
    for box in result.boxes:
        cls_id = int(box.cls[0])
        if cls_id != target_id:
            continue

        cx, cy = box_center(box)
        x1,y1,x2,y2 = zone_rect
        if cx > x1 and cx < x2 and cy > y1 and cy < y2:
            count += 1

    return count

#function that returns a boolean if a nuclear flask is moving
#nuclear flask must be recognized as an image class first
#assume there can be only 1 nuclear flask in frame at a time
def is_moving(result, target_class_name, dist_thresh_px):
    global prev_flask_centre
    target_id = get_target_id(result, target_class_name)

    #case: no flask right now, no flask from before
    if target_id is None and prev_flask_centre is None: 
        return False

    for box in result.boxes:
        if int(box.cls[0]) == target_id:
            cx,cy = box_center(box)

        #case: object just appeared
        if prev_flask_centre == None:
            prev_flask_centre = (cx,cy)
            return False
        #case: object appeared before and is still in frame
        dist = math.hypot(cx - prev_flask_centre[0], cy - prev_flask_centre[1])
        prev_flask_centre = (cx, cy)
        return dist > dist_thresh_px
        
    #case: object is in last frame but now disappered
    prev_flask_centre = None
    return False

        



while True:
    # --- Capture a frame from the camera ---
    ret, frame = videoCap.read()
    if not ret:   # if the camera fails to capture a frame, skip this iteration
        continue

    # --- Run YOLO tracking on the current frame ---
    results = yolo.track(frame, stream=True)

    # --- Iterate through all results (YOLO can output multiple per frame) ---
    for result in results:
        # Dictionary mapping class IDs to human-readable names
        classes_names = result.names

        # --- Iterate through all detected bounding boxes in this result ---
        for box in result.boxes:
            # Confidence score of detection (between 0 and 1). Use only if > 40%.
            if box.conf[0] > 0.4:
                # Extract coordinates of the bounding box (top-left and bottom-right corners)
                x1, y1, x2, y2 = map(int, box.xyxy[0])  # convert float → int

                # Extract the class ID for this detection (e.g., "0" might mean "person")
                cls = int(box.cls[0])

                # Look up the actual class name (e.g., "person", "car", etc.)
                class_name = classes_names[cls]

                # Pick a color (green in this case) for the box and text
                colour = (0, 255, 0)

                # --- Draw a rectangle around the detected object ---
                cv2.rectangle(frame, (x1, y1), (x2, y2), colour, 2)

                # --- Overlay the class name + confidence percentage above the box ---
                cv2.putText(frame, f'{class_name} {box.conf[0]:.2f}',
                            (x1, y1), cv2.FONT_HERSHEY_SIMPLEX, 1, colour, 2)

        # --- Clear any old text at the bottom of the frame ---
        # This draws a solid black rectangle across the bottom strip of the image
        cv2.rectangle(frame,
                      (0, frame.shape[0] - 40),        # top-left corner
                      (frame.shape[1], frame.shape[0]),# bottom-right corner
                      (0, 0, 0), -1)                   # filled black rectangle

        # --- Decide whether to show a warning message ---
        # Count how many "person" detections are in the result
         
            # If more than one person → display "Unauthorised Personnel" in red

    if sum(1 for box in result.boxes if result.names[int(box.cls[0])] == target_Personel) > 2 or  result.names[int(box.cls[0])] == target_Unauthorised_Personel :
        cv2.putText(frame, "Unauthorised Personel",( 100,frame.shape[0] - 10), cv2.FONT_HERSHEY_COMPLEX,1,(0,0,255),2)

    elif result.names[int(box.cls[0])] == target_Open_Flask or result.names[int(box.cls[0])] == target_Broken_Flask:
         cv2.putText(frame, "DANGER: EXPOSED",( 100,frame.shape[0] - 10), cv2.FONT_HERSHEY_COMPLEX,1,(0,0,255),2)
    
    elif sum(1 for box in result.boxes if result.names[int(box.cls[0])] == target_Personel) == 2 and result.names[int(box.cls[0])] == target_Flask : 
         cv2.putText(frame, "Safe to start loading",( 100,frame.shape[0] - 10), cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)

    elif result.names[int(box.cls[0])] == target_preparied: 
        cv2.putText(frame, "Safe to ship",( 100,frame.shape[0] - 10), cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)
    #else: cv2.putText(frame, 
     #       f"{target_Personel}: {sum(1 for box in result.boxes if result.names[int(box.cls[0])] == target_Personel)}",
      #      (50, frame.shape[0] - 10),   # bottom left corner
       #     cv2.FONT_HERSHEY_SIMPLEX, 1,
        #    (255, 255, 255), 2)



    # --- Display the frame in a window named "frame" ---
    cv2.imshow('frame', frame)

    # --- Exit condition ---
    # If the user presses the 'q' key → break the loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# --- Cleanup after loop ends ---
videoCap.release()          # release the camera
cv2.destroyAllWindows()     # close all OpenCV windows

