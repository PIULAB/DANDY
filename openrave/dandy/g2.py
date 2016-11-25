from __future__ import with_statement

import time
import openravepy
import redis
import json
if not __openravepy_build_doc__:
    from openravepy import *
    from numpy import *

def main(env,options):
    r_server = redis.Redis('localhost')

    env.Load('./g2.env.xml')
    robot = env.GetRobots()[0]
    robot.SetActiveManipulator(options.manipname)

    # generate the ik solver
    iktype = IkParameterization.Type.Translation3D if not options.withlocal else IkParameterization.Type.TranslationLocalGlobal6D
    ikmodel = databases.inversekinematics.InverseKinematicsModel(robot, iktype=iktype, freeindices=robot.GetActiveManipulator().GetArmIndices()[3:])
    if not ikmodel.load():
        ikmodel.autogenerate()

    previous_none = False
    while True:
        with env:
            # target=ikmodel.manip.GetTransform()[0:3,3]+(random.rand(3)-0.5)
            xy = json.loads(r_server.get('xy'))
            x, y, z = xy['x'], xy['y'], xy['z']
            target=array([x, y, z])
            if options.withlocal:
                localtarget = 0.5*(random.rand(3)-0.5)
                ikparam = IkParameterization([localtarget,target], IkParameterization.Type.TranslationLocalGlobal6D)
            else:
                localtarget = zeros(3)
                ikparam = IkParameterization(target, IkParameterization.Type.Translation3D)
            # solutions = ikmodel.manip.FindIKSolutions(ikparam,IkFilterOptions.CheckEnvCollisions)
            t0 = time.time()
            solution = ikmodel.manip.FindIKSolution(ikparam, IkFilterOptions.CheckEnvCollisions)

            if previous_none and solution is not None:
                print time.time() - t0
            previous_none = solution is None

            if solution is not None:
              r_server.set('q', json.dumps({'q0': solution[0], 'q1': solution[1], 'q2': solution[2], 'q3': solution[3]}))

              robot.SetDOFValues(solution,ikmodel.manip.GetArmIndices())
              T = ikmodel.manip.GetTransform()
              q = robot.GetDOFValues()
              env.UpdatePublishedBodies()

if __name__ == "__main__":
    class opt:
        manipname = 'arm'
        withlocal = False

    env = Environment()
    env.SetViewer('qtcoin')
    main(env, opt)
