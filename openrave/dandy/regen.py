from openravepy import *

env = Environment()
env.Load('./dandy.robot.xml')

robot = env.GetRobots()[0]
manip = robot.SetActiveManipulator(robot.GetManipulators()[0])
ikmodel = databases.inversekinematics.InverseKinematicsModel(robot, iktype=IkParameterization.Type.Transform6D)

ikmodel.autogenerate()

