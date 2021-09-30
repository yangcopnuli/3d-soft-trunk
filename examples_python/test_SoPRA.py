from softtrunk_pybind_module import CurvatureCalculator, SoftTrunkParameters
from mobilerack_pybind_module import ValveController, QualisysClient

import math
import time
import numpy as np
import matplotlib.pyplot as plt

from pydrake.math import RigidTransform, RollPitchYaw, RotationMatrix
from pydrake.common.eigen_geometry import Quaternion
from collections import defaultdict

np.set_printoptions(precision=4)
plt.rcParams["savefig.facecolor"] = "0.8"


def xform_rot_dist(x1, x2):
    """[summary]

    Args:
        x1 ([type]): [description]
        x2 ([type]): [description]

    Returns:
        [type]: [description]
    """
    return quat_dist(get_quat(x1), get_quat(x2))


def xform_dist(x1, x2):
    """[summary]

    Args:
        x1 ([type]): [description]
        x2 ([type]): [description]

    Returns:
        [type]: [description]
    """
    return pos_dist(x1.translation(), x2.translation()) + quat_dist(
        get_quat(x1), get_quat(x2)
    )


def quat_dist(q1, q2):
    """[summary]

    Args:
        q1 ([type]): [description]
        q2 ([type]): [description]

    Returns:
        [type]: [description]
    """
    q1_dot_q2 = np.dot(q1.wxyz(), q2.wxyz())
    return np.sqrt(np.power(1 - q1_dot_q2 * q1_dot_q2, 2))


def pos_dist(p1, p2):
    """[summary]

    Args:
        p1 ([type]): [description]
        p2 ([type]): [description]

    Returns:
        [type]: [description]
    """
    return np.linalg.norm(p1 - p2)


def get_rpy(xform):
    """[summary]

    Args:
        xform ([type]): [description]

    Returns:
        [type]: [description]
    """
    return RollPitchYaw(xform.rotation()).vector()


def get_quat(xform):
    """[summary]

    Args:
        xform ([type]): [description]

    Returns:
        [type]: [description]
    """
    return xform.rotation().ToQuaternion()


def xform_to_xyzrpy(xform):
    """[summary]

    Args:
        xform ([type]): [description]

    Returns:
        [type]: [description]
    """
    xyzrpy = np.zeros(6)
    xyzrpy[:3] = xform.translation()
    xyzrpy[:3] *= 1000  # m to mm
    xyzrpy[3:] = get_rpy(xform)
    xyzrpy[3:] *= 180 / np.pi  # rad to deg
    return xyzrpy


def rgb_(value):
    """Converts a value in range [0, 1] to rgb color in [0, 1]^3

    Args:
        value ([float]): [0, 1]

    Returns:
        [numpy.array[float]]: [0, 1]^3
    """
    minimum, maximum = 0.0, 1.0
    ratio = 2 * (value - minimum) / (maximum - minimum)
    b = 1 - ratio
    b = np.clip(b, 0, 1)

    r = ratio - 1
    r = np.clip(r, 0, 1)

    g = 1.0 - b - r
    return np.stack([r, g, b]).transpose()


def sleep(seconds):
    """[summary]

    Args:
        seconds ([type]): [description]
    """
    for ii in range(math.floor(seconds)):
        time.sleep(1)
        print(".", end="", flush=True)
    time.sleep(seconds % 1)
    print("-")

num_segments = 3
eps = 1e-10

st_params = SoftTrunkParameters()
st_params.finalize()
cc = CurvatureCalculator(st_params, CurvatureCalculator.SensorType.qualisys, "")

# global const
X_BI_init_nominal = RigidTransform(np.array([0, 0, 0.118]))
X_IT_init_nominal = RigidTransform(np.array([0, 0, 0.118]))

# global variable?
X_BI_init_meas = RigidTransform()
X_IT_init_meas = RigidTransform()


def set_init_params(H_WB, H_WI, H_WT):
    """[summary]

    Args:
        H_WB ([type]): [description]
        H_WI ([type]): [description]
        H_WT ([type]): [description]
    """
    global X_BI_init_meas
    global X_IT_init_meas
    global X_BI_init_diff
    global X_BI_init_nominal
    global X_IT_init_diff
    global X_IT_init_nominal

    X_WB = RigidTransform(H_WB)
    X_WI = RigidTransform(H_WI)
    X_WT = RigidTransform(H_WT)
    X_BI_init_meas = X_WB.inverse() @ X_WI
    X_IT_init_meas = X_WI.inverse() @ X_WT
    X_BI_init_diff = X_BI_init_nominal.inverse() @ X_BI_init_meas
    X_IT_init_diff = X_IT_init_nominal.inverse() @ X_IT_init_meas
    print(f"X_BI_init_meas = {xform_to_xyzrpy(X_BI_init_meas)}")
    print(f"X_IT_init_meas = {xform_to_xyzrpy(X_IT_init_meas)}")
    print(f"X_BI_init_diff = {xform_to_xyzrpy(X_BI_init_diff)}")
    print(f"X_IT_init_diff = {xform_to_xyzrpy(X_IT_init_diff)}")


def calc_rectified_poses(H_WB, H_WI, H_WT):
    """[summary]

    Args:
        H_WB ([type]): [description]
        H_WI ([type]): [description]
        H_WT ([type]): [description]

    Returns:
        [type]: [description]
    """
    global X_BI_init_diff
    global X_IT_init_diff
    X_WB = RigidTransform(H_WB)
    X_WI = RigidTransform(H_WI)
    X_WT = RigidTransform(H_WT)
    X_BI_meas = X_WB.inverse() @ X_WI
    # X_BI_meas = X_BI_init_diff.inverse() @ X_BI_meas
    X_IT_meas = X_WI.inverse() @ X_WT
    # X_IT_meas = X_IT_init_diff.inverse() @ X_IT_meas
    return X_BI_meas, X_IT_meas


def poses_to_angles_array(X_BI, X_IT):
    """[summary]

    Args:
        X_BI ([type]): [description]
        X_IT ([type]): [description]

    Returns:
        [type]: [description]
    """
    xyzrpy_BI = xform_to_xyzrpy(X_BI)
    xyzrpy_IT = xform_to_xyzrpy(X_IT)

    # invert roll?
    xyzrpy_BI[3] *= -1
    xyzrpy_IT[3] *= -1

    # print(f"X_BI = {xyzrpy_BI}\n\tX_IT = {xyzrpy_IT}")
    return np.hstack([xyzrpy_BI[-3:], xyzrpy_IT[-3:]])


def get_pressures_array(idx=None, pressure=None):
    """[summary]

    Args:
        idx ([type], optional): [description]. Defaults to None.
        pressure ([type], optional): [description]. Defaults to eps.
    """
    pressures_array = np.ones(6) * eps
    if idx is not None:
        pressures_array[idx] = pressure
    return pressures_array


def write_data_to_file(
    angles, pressures, filepath="../../sopra-fem/sensor_test/Temp/AngleData.txt"
):
    """Save angle and pressure data to txt file

    Args:
        angles (4x1 float np.array): 2* 2 angles for each segment
        pressures (6x1 float np.array): 6 chambers
        filepath (str, optional): Defaults to "../../sopra-fem/sensor_test/Temp/AngleData.txt"
    """
    # eps = 1e-10
    save_data_array = np.hstack([angles, pressures])
    np.savetxt(filepath, save_data_array)


valves = [2, 6, 4, 0, 5, 3]
# valves = list(range(7))
# # valve 1 is the gripper. skip
# valves.remove(1)

max_pressure = 500
pressure = 300
num_segments = 3
eps = 1e-10

vc = ValveController("192.168.0.100", valves, max_pressure)

st_params = SoftTrunkParameters()
st_params.finalize()
cc = CurvatureCalculator(st_params, CurvatureCalculator.SensorType.qualisys, "")

# global const
X_BI_init_nominal = RigidTransform(np.array([0, 0, 0.118]))
X_IT_init_nominal = RigidTransform(np.array([0, 0, 0.118]))

# global variable?
X_BI_init_meas = RigidTransform()
X_IT_init_meas = RigidTransform()


def set_init_params(H_WB, H_WI, H_WT):
    """[summary]

    Args:
        H_WB ([type]): [description]
        H_WI ([type]): [description]
        H_WT ([type]): [description]
    """
    global X_BI_init_meas
    global X_IT_init_meas
    global X_BI_init_diff
    global X_BI_init_nominal
    global X_IT_init_diff
    global X_IT_init_nominal

    X_WB = RigidTransform(H_WB)
    X_WI = RigidTransform(H_WI)
    X_WT = RigidTransform(H_WT)
    X_BI_init_meas = X_WB.inverse() @ X_WI
    X_IT_init_meas = X_WI.inverse() @ X_WT
    X_BI_init_diff = X_BI_init_nominal.inverse() @ X_BI_init_meas
    X_IT_init_diff = X_IT_init_nominal.inverse() @ X_IT_init_meas
    print(f"X_BI_init_meas = {xform_to_xyzrpy(X_BI_init_meas)}")
    print(f"X_IT_init_meas = {xform_to_xyzrpy(X_IT_init_meas)}")
    print(f"X_BI_init_diff = {xform_to_xyzrpy(X_BI_init_diff)}")
    print(f"X_IT_init_diff = {xform_to_xyzrpy(X_IT_init_diff)}")


def calc_rectified_poses(idx, H_WB, H_WI, H_WT):
    """[summary]

    Args:
        idx ([type]): [description]
        H_WB ([type]): [description]
        H_WI ([type]): [description]
        H_WT ([type]): [description]

    Returns:
        [type]: [description]
    """
    global X_BI_init_diff
    global X_IT_init_diff
    X_WB = RigidTransform(H_WB)
    X_WI = RigidTransform(H_WI)
    X_WT = RigidTransform(H_WT)
    X_BI_meas = X_WB.inverse() @ X_WI
    # X_BI_meas = X_BI_init_diff.inverse() @ X_BI_meas
    X_IT_meas = X_WI.inverse() @ X_WT
    # X_IT_meas = X_IT_init_diff.inverse() @ X_IT_meas
    return X_BI_meas, X_IT_meas


def poses_to_angles_array(X_BI, X_IT):
    """[summary]

    Args:
        X_BI ([type]): [description]
        X_IT ([type]): [description]

    Returns:
        [type]: [description]
    """
    xyzrpy_BI = xform_to_xyzrpy(X_BI)
    xyzrpy_IT = xform_to_xyzrpy(X_IT)
    print(f"X_BI = {xyzrpy_BI}\n\tX_IT = {xyzrpy_IT}")
    return np.hstack([xyzrpy_BI[-3:][::-1], xyzrpy_IT[-3:][::-1]])


def get_pressures_array(idx=None, pressure=None):
    """[summary]

    Args:
        idx ([type], optional): [description]. Defaults to None.
        pressure ([type], optional): [description]. Defaults to eps.
    """
    pressures_array = np.ones(6) * eps
    if idx is not None:
        pressures_array[idx] = pressure
    return pressures_array


def write_data_to_file(
    angles, pressures, filepath="../../sopra-fem/sensor_test/Temp/AngleData.txt"
):
    """Save angle and pressure data to txt file

    Args:
        angles (4x1 float np.array): 2* 2 angles for each segment
        pressures (6x1 float np.array): 6 chambers
        filepath (str, optional): Defaults to "../../sopra-fem/sensor_test/Temp/AngleData.txt"
    """
    # eps = 1e-10
    save_data_array = np.hstack([angles, pressures])
    np.savetxt(filepath, save_data_array)


sleep(1)  # sleep until data is received from Qualisys (temporary hack solution)

q, dq, ddq = cc.get_curvature()
H_WB = RigidTransform(cc.get_frame(0))
H_WI = RigidTransform(cc.get_frame(1))
H_WT = RigidTransform(cc.get_frame(num_segments - 1))
timestamp = cc.get_timestamp()
set_init_params(H_WB, H_WI, H_WT)

pressures_array = get_pressures_array()  # default
angles_array = np.ones(4) * eps
write_data_to_file(angles_array, pressures_array)

for ii in range(len(valves[:3])):
    print(f"\nindex:{ii}\tvalve id:{valves[ii]}\tpressure:{pressure}")
    # set pressure
    vc.setSinglePressure(ii, pressure)

    # wait to settle
    sleep(10)

    q, dq, ddq = cc.get_curvature()
    H_WB = RigidTransform(cc.get_frame(0))
    H_WI = RigidTransform(cc.get_frame(1))
    H_WT = RigidTransform(cc.get_frame(num_segments - 1))
    timestamp = cc.get_timestamp()
    X_BI, X_IT = calc_rectified_poses(ii, H_WB, H_WI, H_WT)

    pressures_array = get_pressures_array(ii, pressure)
    angles_array = poses_to_angles_array(X_BI, X_IT)
    write_data_to_file(angles_array, pressures_array)

    # sleep(5)
    input()

    # reset to zero
    vc.setSinglePressure(ii, 0)
    sleep(1)

print("All valves closed")
q, dq, ddq = cc.get_curvature()
H_WB = RigidTransform(cc.get_frame(0))
H_WI = RigidTransform(cc.get_frame(1))
H_WT = RigidTransform(cc.get_frame(num_segments - 1))
timestamp = cc.get_timestamp()
X_BI, X_IT = calc_rectified_poses(ii, H_WB, H_WI, H_WT)

pressures_array = get_pressures_array(ii, pressure)
angles_array = poses_to_angles_array(X_BI, X_IT)
write_data_to_file(angles_array, pressures_array)
