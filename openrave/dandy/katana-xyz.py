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
"""Shows how to use translational inverse kinematics for an arm with few joints.

.. examplepre-block:: tutorial_iktranslation
  :image-width: 640

Description
-----------

There are two different types of translation 3D IKs:

* **Translation3D** - Align the origin of the manipulation coordinate system with the desired world position

* **TranslationLocalGlobal6D** - Align a dynamically chosen position with respect ot the manipulation coordinate system with the desired world position. To see this in action, execute the example with:

.. code-block:: bash

  openrave.py --example tutorial_iktranslation --withlocal

.. examplepost-block:: tutorial_iktranslation
"""
from __future__ import with_statement # for python 2.5
__author__ = 'Rosen Diankov'

import time
import openravepy
import redis
import json
if not __openravepy_build_doc__:
    from openravepy import *
    from numpy import *

def main(env,options):
    "Main example code."
    r_server = redis.Redis('localhost')

    env.Load('./katana-5d.env.xml')
    robot = env.GetRobots()[0]
    robot.SetActiveManipulator(options.manipname)

    # generate the ik solver
    iktype = IkParameterization.Type.Translation3D if not options.withlocal else IkParameterization.Type.TranslationLocalGlobal6D
    ikmodel = databases.inversekinematics.InverseKinematicsModel(robot, iktype=iktype,freeindices=robot.GetActiveManipulator().GetArmIndices()[3:])
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
                ikparam = IkParameterization([localtarget,target],IkParameterization.Type.TranslationLocalGlobal6D)
            else:
                localtarget = zeros(3)
                ikparam = IkParameterization(target,IkParameterization.Type.Translation3D)
            # solutions = ikmodel.manip.FindIKSolutions(ikparam,IkFilterOptions.CheckEnvCollisions)
            t0 = time.time()
            solution = ikmodel.manip.FindIKSolution(ikparam, IkFilterOptions.IgnoreSelfCollisions)
            if previous_none and solution is not None:
                print time.time() - t0
            previous_none = solution is None

            if solution is not None:
                robot.SetDOFValues(solution,ikmodel.manip.GetArmIndices())
                T = ikmodel.manip.GetTransform()
                env.UpdatePublishedBodies()

from optparse import OptionParser
from openravepy.misc import OpenRAVEGlobalArguments

if __name__ == "__main__":
    class opt:
        manipname = 'arm'
        withlocal = False

    env = Environment()
    env.SetViewer('qtcoin')
    main(env, opt)
