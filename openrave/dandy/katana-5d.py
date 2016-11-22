#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2009-2011 Rosen Diankov (rosen.diankov@gmail.com)
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Shows how to use 5D translation+direction inverse kinematics for an arm with >= 5 joints.

.. examplepre-block:: tutorial_ik5d

.. examplepost-block:: tutorial_ik5d
"""
from __future__ import with_statement # for python 2.5
__author__ = 'Rosen Diankov'

import time, redis, json
import openravepy
if not __openravepy_build_doc__:
    from openravepy import *
    from numpy import *

def main(env,options):
    "Main example code."
    r_server = redis.Redis('localhost')
    directions = {}

    env.Load(options.scene)
    robot = env.GetRobots()[0]
    robot.SetActiveManipulator(options.manipname)

    # generate the ik solver
    ikmodel = databases.inversekinematics.InverseKinematicsModel(robot, iktype=IkParameterization.Type.TranslationDirection5D)
    if not ikmodel.load():
        ikmodel.autogenerate()

    while True:
        xy = json.loads(r_server.get('xy'))
        x = xy['x']
        y = xy['y']
        z = xy['z']

        with env:
            while True:
                # target = ikmodel.manip.GetTransform()[0:3, 3] + (random.rand(3) - 0.5)
                target = array([x, y, z])

                # direction = array([x, y, z])
                # direction /= linalg.norm(direction)
                # print Ray(target,direction)

                key = r_server.get('xy')
                if False:  #directions.has_key(key):
                    direction = directions[key]
                else:
                    direction = random.rand(3)-0.5
                    direction[0] = 0
                    # direction[1] = -y
                    # direction[2] = -z
                    direction /= linalg.norm(direction)

                solutions = ikmodel.manip.FindIKSolutions(IkParameterization(Ray(target,direction),IkParameterization.Type.TranslationDirection5D),IkFilterOptions.CheckEnvCollisions)
                if solutions is None or len(solutions) == 0:
                    # print 0, target, direction
                    break

                print len(solutions), target, direction
                directions[key] = direction
                for i in range(1):
                    with env:
                        robot.SetDOFValues(solutions[i],ikmodel.manip.GetArmIndices())
                        env.UpdatePublishedBodies()
                    time.sleep(0.05)
                break

from optparse import OptionParser
from openravepy.misc import OpenRAVEGlobalArguments

@openravepy.with_destroy
def run(args=None):
    """Command-line execution of the example.

    :param args: arguments for script to parse, if not specified will use sys.argv
    """
    parser = OptionParser(description='Shows how to use the 5DOF IK solution for arms with >= 5 joints.')
    OpenRAVEGlobalArguments.addOptions(parser)
    parser.add_option('--scene',action="store",type='string',dest='scene',default='data/katanatable.env.xml',
                      help='Scene file to load (default=%default)')
    parser.add_option('--manipname',action="store",type='string',dest='manipname',default='arm',
                      help='name of manipulator to use (default=%default)')
    (options, leftargs) = parser.parse_args(args=args)
    OpenRAVEGlobalArguments.parseAndCreateThreadedUser(options,main,defaultviewer=True)

if __name__ == "__main__":
    run()
