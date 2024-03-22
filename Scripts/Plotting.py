import matplotlib.pyplot as plt
import serial
import csv
#import pdb

ser = serial.Serial('COM32',9600)

fig1 = plt.figure(figsize=(7,3))

ax1 = fig1.add_subplot(221)
ax2 = fig1.add_subplot(222)
ax3 = fig1.add_subplot(223)
ax4 = fig1.add_subplot(224)

fig1.show()

a1=[]


current_vals = []
thrust_vals = []
rpm_vals = []
voltage_vals = []

x_vals=[0,0,0,0]
i=0
l=200
ser.close()
ser.open()
ctr =0
k = 0
throttle_percet = 0
throttle_percet_ctr = 1
throttle = []
for i in range(l):
    ser1 = ser.readline().decode('ascii')
    a1=[]
    ser_array = [float(val) for val in ser1[:-2].split()]  # Assuming values are space-separated
    with open('abc.csv', 'a', encoding='UTF8') as f:
        writer = csv.writer(f)
        writer.writerow(ser_array)
    a1.extend(ser_array)
    #pdb.set_trace()
    current_vals.append(a1[2])
    thrust_vals.append(a1[1])
    rpm_vals.append(a1[0])
    voltage_vals.append(a1[3])  
    #print(current_vals[ctr])
    #print(voltage_vals[ctr])
    
    
    if(throttle_percet_ctr == 4):
        throttle_percet_ctr =1
        throttle_percet += 1 

    throttle.append(9*throttle_percet *10/18)
    print(throttle[i])
    
    x_vals[0] =i
    plt.ylabel('Thrust (g)', fontweight='bold', horizontalalignment='center')
    plt.xlabel('Throttle %', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    plt.subplot(2, 2, 1)
    ax1.scatter(throttle,current_vals,color='b')
    #fig1.canvas.draw()
    #ax1.set_xlim(left=max(0,x_vals[0]-30), right=x_vals[0]+60)
    
    x_vals[1] =i
    plt.ylabel('current(A)', fontweight='bold', horizontalalignment='center')
    plt.xlabel('Throttle %', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    plt.subplot(2, 2, 2)
    ax2.scatter(throttle,voltage_vals,color='b')
    #fig1.canvas.draw()
    #ax2.set_xlim(left=max(0,x_vals[1]-30), right=x_vals[1]+60)
    
    x_vals[2] =i
    plt.ylabel('Voltage(V)', fontweight='bold', horizontalalignment='center')
    plt.xlabel('Throttle %', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    plt.subplot(2, 2, 3)
    ax3.scatter(throttle,rpm_vals,color='b')
    #fig1.canvas.draw()
    #ax3.set_xlim(left=max(0,x_vals[2]-30), right=x_vals[2]+60)
    
    x_vals[3] =i
    plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    plt.xlabel('Throttle %', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    plt.subplot(2, 2, 4)
    ax4.scatter(throttle, thrust_vals,color='b')
    
    #ax4.set_xlim(left=max(0,x_vals[3]-30), right=x_vals[3]+60)
    ctr = ctr+1
    throttle_percet_ctr +=1
    
    fig1.canvas.draw()
    plt.pause(0.0001)



plt.show()
ser.close()
