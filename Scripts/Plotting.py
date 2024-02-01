import matplotlib.pyplot as plt
import serial
import csv
#import pdb

ser = serial.Serial('COM5',9600)
fig1 = plt.figure(figsize=(7,3))
fig2 = plt.figure(figsize=(7,3))
fig3 = plt.figure(figsize=(7,3))
fig4 = plt.figure(figsize=(7,3))

ax1 = fig1.add_subplot()
ax2 = fig2.add_subplot()
ax3 = fig3.add_subplot()
ax4 = fig4.add_subplot()

fig1.show()
fig2.show()
fig3.show()
fig4.show()

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
    
    x_vals[0] =i
    plt.ylabel('current(mA)', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    ax1.plot(current_vals,color='b')
    fig1.canvas.draw()
    ax1.set_xlim(left=max(0,x_vals[0]-30), right=x_vals[0]+60)
    
    x_vals[1] =i
    plt.ylabel('Voltage(V)', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    ax2.plot(voltage_vals,color='b')
    fig2.canvas.draw()
    ax2.set_xlim(left=max(0,x_vals[1]-30), right=x_vals[1]+60)
    
    x_vals[2] =i
    plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    ax3.plot(rpm_vals,color='b')
    fig3.canvas.draw()
    ax3.set_xlim(left=max(0,x_vals[2]-30), right=x_vals[2]+60)
    
    x_vals[3] =i
    plt.ylabel('Thrust', fontweight='bold', horizontalalignment='center')
    #plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    ax4.plot(thrust_vals,color='b')
    fig4.canvas.draw()
    ax4.set_xlim(left=max(0,x_vals[3]-30), right=x_vals[3]+60)
    ctr = ctr+1
    plt.pause(0.0001)



plt.show()
ser.close()
