"""Shows how to get all 6D IK solutions.
"""
from openravepy import *
import numpy, time, random, redis, json

r_server = redis.Redis('localhost')
env = Environment()
env.SetViewer('qtcoin')

# env.Load('data/pr2test1.env.xml')
# env.Load('robots/mitsubishi-pa10.zae')
# env.Load('robots/barrettwam.robot.xml')
# env.Load('robots/schunk-lwa3.zae')
# env.Load('/home/enry/src/openrave/src/data/katanatable.env.xml')
env.Load('./dandy.robot.xml')

robot = env.GetRobots()[0]

manip = robot.SetActiveManipulator(robot.GetManipulators()[0])
ikmodel = databases.inversekinematics.InverseKinematicsModel(robot, iktype=IkParameterization.Type.Transform6D)
# ikmodel = databases.inversekinematics.InverseKinematicsModel(robot, iktype=IkParameterization.Type.TranslationDirection5D)
if not ikmodel.load():
    ikmodel.autogenerate()
print 'ready'

with env:
    while True:
        xy = json.loads(r_server.get('xy'))
        x = xy['x']
        y = xy['y']
        z = xy['z']
        Tgoal = numpy.array([[0,-1,0,x],[-1,0,0,y],[0,0,-1,z],[0,0,0,1]])
        sol = manip.FindIKSolution(Tgoal, 2) # get collision-free solution
        with robot:  # save robot state
            if sol is not None:
                robot.SetDOFValues(sol,manip.GetArmIndices()) # set the current solution
                Tee = manip.GetEndEffectorTransform()
                env.UpdatePublishedBodies() # allow viewer to update new robot

        time.sleep(0.05)

    time.sleep(5)
