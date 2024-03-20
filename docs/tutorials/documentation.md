# Code Documentation
In order to pass down the knowledge aquired to the next generations of engineers and to make the code developed easier to understand, it is fundamental that both the code, system design and architecture and decisions are all documented.
- For code documentation, we will be using **Doxygen**.

## Doxygen C++
C++ code example:
```c++
/**
 * @brief Class for Kalman Filter
 * Used for SLAM
 * 
*/
class KalmanFilter {

 public:
  /**
   * @brief Base constructor
   * 
   * @param MotionModel motion_model motion model to be used
   * @param ObservationModel observation_model validation model to be used
   * 
  */ 
  KalmanFilter(const MotionModel& motion_model, const ObservationModel& observation_model);
}
```

Doxygen does not need to be installed in your computers. It will run automatically when pushed to the main and the documentation is generated and published [here](https://fs-feup.github.io/el-sw/).