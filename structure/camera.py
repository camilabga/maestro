import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'build'))
import random

import psmove
import time
import numpy as np
import cv2


if __name__ == '__main__':

    if psmove.count_connected() < 1:
        print('No controller connected')
        sys.exit(1)

    move = psmove.PSMove()

    if move.connection_type != psmove.Conn_Bluetooth:
        print('Please connect controller via Bluetooth')
        sys.exit(1)
    
    
    

    cap = cv2.VideoCapture(2)

    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    if str(sys.argv[1]) is not None:
        out = cv2.VideoWriter(str(sys.argv[1]) + str(random.random()*100) + '.avi',fourcc, 20.0, (640,480))
    else:
        np.savetxt('videoTest.csv',csvPoints,delimiter=",",fmt = '%4.2f')
    

    if (not cap.isOpened()):
        print('Error opening video stream')
        exit()

    main_win = "Main"
    cv2.namedWindow(main_win, cv2.WINDOW_KEEPRATIO)
    cv2.resizeWindow(main_win, 640, 480)

    # Color range of the mice un the subtracted image
    minBGR = np.array([44, 54, 63])
    maxBGR = np.array([71, 255, 255])

    points = []
    keep = True
    
    while(cap.isOpened() and keep):

        move.set_leds(255, 0, 255)
        move.update_leds()

        # Get the latest input report from the controller
        while move.poll(): pass
        
        trigger_value = move.get_trigger()
        #move.set_leds(trigger_value, trigger_value - 80, 0)
        #move.update_leds()

        buttons = move.get_buttons()
        if buttons & psmove.Btn_START:
            keep = False
        else:
            move.set_rumble(0)

        
        ret, frame = cap.read()

        if(not ret):
            print('Error readning video stream')
            exit()
       
        
        frameBlur = cv2.GaussianBlur(frame, (7, 7), 0)
        img_hsv = cv2.cvtColor(frameBlur, cv2.COLOR_BGR2HSV)

        #Retorna uma imagem binary do resultado do filtro
        mask1 = cv2.inRange(img_hsv, (0, 0, 230), (9,15,255))

        circles = cv2.HoughCircles(mask1, cv2.HOUGH_GRADIENT,1,150,
                            param1=50,param2=10,minRadius=0,maxRadius=0)
        print('circles:',circles)
 
        # ensure at least some circles were found
        if circles is not None:
            # convert the (x, y) coordinates and radius of the circles to integers
            #print('Found something')
            circles = np.round(circles[0, :]).astype("int")

            if buttons & psmove.Btn_MOVE:

                #print('accel:', (move.ax, move.ay, move.az))
                accel = (move.ax, move.ay, move.az)
                #print('gyro:', (move.gx, move.gy, move.gz))
                gyro = (move.gx, move.gy, move.gz)
                #print('magnetometer:', (move.mx, move.my, move.mz))


                print(circles[0])
                point =(circles[0])
                points.append((point,accel,gyro))
                print(points[0][0])

            # loop over the (x, y) coordinates and radius of the circles
            for (x, y, r) in circles:
                # draw the circle in the output image, then draw a rectangle
                # corresponding to the center of the circle
                cv2.circle(frame, (x, y), r, (0, 255, 0), 4)
                cv2.rectangle(frame, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)
            
        for point in points:

            cv2.circle(frame,(int(point[0][0]),int(point[0][1])), int(point[0][2]), (0,0,255), -1)

        out.write(frame)
        cv2.imshow(main_win, frame)

        key = cv2.waitKey(5)
        if(key == 27):
            cv2.destroyAllWindows()
            cap.release()
            exit()
        if(key == 32):
            while True:
                cv2.imshow(main_win, frame)

                key2 = cv2.waitKey(5)
                if(key2 == 32):
                    break
                elif(key2 == 27):
                    cv2.destroyAllWindows()
                    cap.release()
                    exit()
        
    csvPoints = np.array(points)
    csvPoints = csvPoints.reshape(csvPoints[:,0,0].size,9)
    print('saving',sys.argv[1])

    if str(sys.argv[1]) is not None:
        np.savetxt(str(sys.argv[1]),csvPoints,delimiter=",",fmt = '%4.2f')
    else:
        np.savetxt('dataIn.csv',csvPoints,delimiter=",",fmt = '%4.2f')


