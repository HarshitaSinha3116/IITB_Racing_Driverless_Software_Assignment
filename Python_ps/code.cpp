import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

cones = pd.read_csv('cones.csv')

n = len(cones)

xB = cones['x'][:n].to_numpy()
yB = cones['y'][:n].to_numpy()
xY = cones['x'][n:2*n].to_numpy()
yY = cones['y'][n:2*n].to_numpy()

theta_B = np.arctan2(yB - yB.mean(), xB - xB.mean())
theta_Y = np.arctan2(yY - yY.mean(), xY - xY.mean())

sort_B = np.argsort(theta_B)
sort_Y = np.argsort(theta_Y)

xB_sorted = xB[sort_B]
yB_sorted = yB[sort_B]
xY_sorted = xY[sort_Y]
yY_sorted = yY[sort_Y]

Cx = (xB_sorted + xY_sorted) / 2
Cy = (yB_sorted + yY_sorted) / 2

fig, ax = plt.subplots()
ax.plot(Cx, Cy, 'r.-')
ax.set_aspect('equal')

particle, = ax.plot([], [], 'bo')

def update(frame):
    particle.set_data(Cx[frame], Cy[frame])
    return particle,

ani = FuncAnimation(fig, update, frames=len(Cx), interval=50)
plt.show()
