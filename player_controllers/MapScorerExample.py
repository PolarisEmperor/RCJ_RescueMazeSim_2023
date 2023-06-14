from controller import Robot
import numpy as np
import struct

timeStep = 32            # Set the time step for the simulation

robot = Robot()

emitter = robot.getDevice("emitter")

while robot.step(timeStep) != -1:


    # '0': None/Unknown
    # '1': Walls
    # '2': Holes
    # '3': Swamps
    # '4': Checkpoints
    # '5': Starting tile
    # '6': Connection tile from 1 to 2
    # '7': Connection tile from 1 to 3
    # '8': Connection tile from 2 to 3
    # 'H': Harmed victim
    # 'S': Stable victim
    # 'U': Unharmed victim
    # 'F': Flammable Gas
    # 'P': Poison
    # 'C': Corrosive
    # 'O': Organic Peroxide


    ## Test map array for world1.wbt
    subMatrix = np.array([
    ['1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1'],
    ['1','5','0','5','0','6','0','6','0','7','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','5','0','5','0','6','0','6','0','7','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','1','1','1','1','1','1','1','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','1','1','1','1','1','1','1','1','1','1','1','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','1','1','1','1','1','1','1','1','1','1','1','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','1'],
    ['1','0','0','0','0','1','1','1','1','0','0','0','1','0','0','0','0','0','1','1','1','0','1','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','1','1','1','1','1','0','0','0','1','1','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','0','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','1','1','1','1','0','0','0','1','1','1','1','1','1','1','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','1','1','1','0','0','0','0','1','1','1','1','1','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1'],
    ['1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1']])
    """
    subMatrix = np.array([
        ['1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'],
        ['1', '5', '0', '5', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '3', '0', '3', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '5', '0', '5', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3', '0', '3', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'S'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '6', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['F', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '6', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '7', '0', '7', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '7', '0', '7', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', 'U', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', 'H', '1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['P', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '8', '0', '8', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '8', '0', '8', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '4', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '4', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1']
    ])
    """

    """
    ## Test map array for world2.wbt
    subMatrix = np.array([
        ['1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'C', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'],
        ['1', '4', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '0', '0', '0', '0', '3', '0', '3', '1', '2', '0', '2', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '4', '0', '4', '0', '0', '0', '0', 'C', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '0', '0', '0', '0', '3', '0', '3', '1', '2', '0', '2', '1'],
        ['1', '1', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '6', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', 'O', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', 'U', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '6', '0', '6', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '8', '0', '8', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '4', '1', '0', '0', '0', '0', '0', '0', '0', '1', '5', '0', '5', '1'],
        ['1', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '1', '0', '0', '0', '1', '8', '0', '8', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '4', 'H', '0', '0', '0', '0', '0', '0', '0', '1', '5', '0', '5', '1'],
        ['1', '1', '1', 'S', '1', '1', '1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '1', '4', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '4', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1', '1', '1', '0', '0', '0', '1', 'U', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1'],
        ['1', '4', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3', '0', '3', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '4', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '3', '0', '3', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '7', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '1', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '7', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['0', '1', '1', '1', '1', 'F', '1', '1', '1', '1', '1', '1', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', 'S', '1', '1', '1', '1', '1', '1', '1']
    ])
    """

    """
    ## Test map aray for world3.wbt
    subMatrix = np.array([
        ['1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '5', '0', '5', '0', '0', '0', '0', '1', '4', '0', '4', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', 'FH', '0', '0', '0', '0', '5', '0', '5', '0', '0', '0', '0', '1', '4', '0', '4', '0', '0', '0', '0', 'F'],
        ['1', '1', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '1', '1', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '4', '0', '4', '1', '2', '0', '2', '0', '3', '0', '3', '0', '0', '0', '0', '1', '0', '1', '0', '0', '3', '0', '3', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '4', '0', '4', '1', '2', '0', '2', '0', '3', '0', '3', '0', '0', '0', '0', '1', '0', '1', '0', '0', '3', '0', '3', '1'],
        ['1', '1', '1', '1', '1', '0', '0', '0', '1', 'U', '1', '1', '1', '0', '0', '0', '1', 'SO', '1', '1', '1', '1', '0', '0', '0', '0', '0', '1', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '4', '0', '4', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '4', '0', '4', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0'],
        ['1', '1', '1', '0', '0', '0', '1', '1', '1', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0', '1', '1'],
        ['0', '0', 'S', '0', '0', '0', '0', '0', '1', '6', '0', '6', '1', '7', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'U'],
        ['1', '1', '1', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '6', '0', '6', '1', '7', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '2', '0', '2', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0']
    ])
    """

    """
    ## Test map array for world4.wbt
    subMatrix = np.array([
        ['1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0', '0', '0', '0'],
        ['1', '5', '0', '5', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0'],
        ['1', '5', '0', '5', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0'],
        ['1', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '1'],
        ['1', '0', '0', '0', '1', '7', '0', '7', '1', '0', '0', '0', '0', '6', '0', '6', '1'],
        ['1', '0', '0', '0', '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '1', '7', '0', '7', '1', '0', '0', '0', '0', '6', '0', '6', '1'],
        ['1', 'U', '1', 'F', '1', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', 'H', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '0', 'O', '0', '0', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0', '0', '1'],
        ['1', '0', '0', '0', '0', '0', '0', '0', '0', '8', '0', '8', '0', '0', '0', '0', '1'],
        ['1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1'],
        ['0', '0', '1', '0', '0', '0', '0', '0', '0', '8', '0', '8', '0', '0', '0', '0', '1'],
        ['0', '0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1']
    ])
    """

    # Get shape
    s = subMatrix.shape
    # Get shape as bytes
    s_bytes = struct.pack('2i',*s)

    # Flattening the matrix and join with ','
    flatMap = ','.join(subMatrix.flatten())
    # Encode
    sub_bytes = flatMap.encode('utf-8')

    # Add togeather, shape + map
    a_bytes = s_bytes + sub_bytes

    # Send map data
    emitter.send(a_bytes)
    # Send map evaluate request
    map_evaluate_request = struct.pack('c', b'M')
    emitter.send(map_evaluate_request)

    #########
    #Exit message
    exit_mes = struct.pack('c', b'E')
    emitter.send(exit_mes)

    break