from softtrunk_pybind_module import CurvatureCalculator
from time import sleep

cc = CurvatureCalculator(CurvatureCalculator.SensorType.qualisys)

for i in range(10):
    q, dq, ddq = cc.get_curvature()
    print(f"q: {q}")
    sleep(0.5)