import matplotlib.pyplot as plt
import serial

ser = serial.Serial('COM32 ',9600)
fig1 = plt.figure(figsize=(7,3))

ax1 = fig1.add_subplot()

fig1.show()

a1=[]

current_vals = []
thrust_vals = []
rpm_vals = []

x_vals=[0]
i=0
l=200
ser.close()
ser.open()
ctr =0

for i in range(l):
    ser1 = ser.readline().decode('ascii')
    a1=[]
    ser_array = [float(val) for val in ser1.split()]  # Assuming values are space-separated
    a1.extend(ser_array)
    
    current_vals.append(a1[2])
    thrust_vals.append(a1[1])
    rpm_vals.append(a1[0])  
    print(current_vals[ctr])
    
    plt.xlabel('current(mA)', fontweight='bold', horizontalalignment='center')
    plt.ylabel('RPM', fontweight='bold', horizontalalignment='center')
    ax1.plot(current_vals[ctr],rpm_vals[ctr],color='b')
    fig1.canvas.draw()
    ax1.set_xlim(left=max(0,x_vals[0]-5), right=x_vals[0]+10)
    ctr = ctr+1
    
    plt.pause(0.0001)

plt.show()

