#include <iostream>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "sophus/se3.hpp"

using namespace std;
using namespace Eigen;

int main(int argc, char** argv) {
    // Rotation matrix with 90 degree around Z axis
    //toRotationMatrix converts AngleAxis to Matrix using Rodrigues' rotation formula R=I+sinθ[u^]×​+(1−cosθ)[u^]×2​
    Matrix3d R = AngleAxisd(M_PI / 2, Vector3d(0, 0, 1)).toRotationMatrix();
    cout << "Rotation matrix R:\n" << R << endl;

    Quaterniond q(R);
    cout << "Quaternion from rotation matrix:\n" << q.coeffs().transpose() << endl;
    Sophus::SO3d SO3_R(R);
    Sophus::SO3d SO3_q(q);
    cout << "SO3 from rotation matrix:\n" << SO3_R.matrix() << endl;
    cout << "SO3 from quaternion:\n" << SO3_q.matrix() << endl;

    // Use log map to get the Lie algebra
    Vector3d so3 = SO3_R.log();
    cout << "Lie algebra so3:\n" << so3.transpose() << endl;
    // hat is from vector to skew-symmetric matrix
    cout << "so3 hat:\n" << Sophus::SO3d::hat(so3) << endl;
    // vee is from skew-symmetric matrix to vector
    cout << "so3 vee:\n" << Sophus::SO3d::vee(Sophus::SO3d::hat(so3)).transpose() << endl;

    cout << "------------------" << endl;
    Vector3d t(1, 0, 0);
    Sophus::SE3d SE3_Rt(R, t);
    Sophus::SE3d SE3_qt(q, t);
    cout << "SE3 from R,t:\n" << SE3_Rt.matrix() << endl;
    cout << "SE3 from q,t:\n" << SE3_qt.matrix() << endl;

    typedef Eigen::Matrix<double, 6, 1> Vector6d;
    Vector6d se3 = SE3_Rt.log();
    cout << "Lie algebra se3:\n" << se3.transpose() << endl;
    cout << "se3 hat:\n" << Sophus::SE3d::hat(se3) << endl;
    cout << "se3 vee:\n" << Sophus::SE3d::vee(Sophus::SE3d::hat(se3)).transpose() << endl;

    Vector6d update_se3;
    update_se3.setZero();
    update_se3(0, 0) = 1e-4;
    Sophus::SE3d SE3_updated = Sophus::SE3d::exp(update_se3) * SE3_Rt;
    cout << "Updated SE3:\n" << SE3_updated.matrix() << endl;

    return 0;
}