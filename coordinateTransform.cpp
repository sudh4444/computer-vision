#include <iostream>
#include <vector>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Geometry> // For transformations
using namespace std;
using namespace Eigen;

int main(int argc, char **argv){
    Quaterniond q1(0.35, 0.2, 0.3, 0.1), q2(-0.5, 0.4, -0.1, 0.2);
    q1.normalize();
    q2.normalize();
    Vector3d t1(0.3, 0.1, 0.1), t2(-0.1, 0.5, 0.3);
    Vector3d p1(0.5, 0, 0.2);

    Isometry3d T1m(q1), T2m(q2);
    cout << "Transform matrix T1: \n" << T1m.matrix() << endl;
    cout << "Transform matrix T2: \n" << T2m.matrix() << endl;
    T1m.pretranslate(t1);
    T2m.pretranslate(t2);

    Vector3d p2 = T2m * T1m.inverse()* p1;
    cout << endl << p2.transpose() << endl;
    return 0;
}