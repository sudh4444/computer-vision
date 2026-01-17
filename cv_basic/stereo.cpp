#include <opencv2/opencv.hpp>
#include <iostream>
#include <pangolin/pangolin.h>
#include <Eigen/Core>
#include <vector>
#include <unistd.h>

using namespace std;
using namespace Eigen;
using Vector6d = Eigen::Matrix<double, 6, 1>;

void showPointCloud(
    const vector<Vector6d, Eigen::aligned_allocator<Vector6d>> &pointCloud);

int main(int argc, char **argv){
    if(argc != 3){
        cout << "Usage: stereo <left_image> <right_image>" << endl;
        return -1;
    }

    double fx = 718.856, fy = 718.856;
    double cx = 607.1928, cy = 185.2157;
    double baseline = 0.573;

    cv::Mat left = cv::imread(argv[1], cv::IMREAD_COLOR);
    cv::Mat right = cv::imread(argv[2], cv::IMREAD_COLOR);
    cv::Mat leftG;
    cv::cvtColor(left, leftG, cv::COLOR_BGR2GRAY);
    cv::Mat rightG;
    cv::cvtColor(right, rightG, cv::COLOR_BGR2GRAY);
    
    if (left.empty() || right.empty()){
        cout << "Cannot read images!" << endl;
        return -1;
    }
    
    cv::imshow("left", left);
    cv::imshow("right", right);
    cv::waitKey(0);
    
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, 96, 9, 8*9*9, 32*9*9, 1, 63, 10, 100, 32);

    cv::Mat disparity_sgbm, disparity;
    sgbm->compute(leftG, rightG, disparity_sgbm);
    disparity_sgbm.convertTo(disparity, CV_32F, 1.0/16.0f);

    // compute the point cloud
    vector<Vector6d, Eigen::aligned_allocator<Vector6d>> pointCloud;

    for(int v=0; v<left.rows; v+=2){
        for(int u=0; u<left.cols; u+=2){
            if(disparity.at<float>(v,u) <= 10.0 || disparity.at<float>(v, u) >= 96.0)
                continue;

            Vector3d point(0, 0, 0); // first dimensions are xyz, then color
            // compute the depth from disparity
            double x = (u - cx) / fx;
            double y = (v - cy) / fy;
            double depth = fx * baseline / disparity.at<float>(v, u);
            point[0] = x * depth;
            point[1] = y * depth;
            point[2] = depth;
            
            cv::Vec3b color = left.at<cv::Vec3b>(v, u);
            Vector6d point_color;
            point_color << point[0], point[1], point[2],
                                   color[2] / 255.0, color[1] / 255.0, color[0] / 255.0;

            pointCloud.push_back(point_color);
        }
    }

    cv::imshow("disparity", disparity / 96.0f);
    cv::waitKey(0);

    // display point cloud
    showPointCloud(pointCloud);

    return 0;
}

void showPointCloud(
    const vector<Vector6d, Eigen::aligned_allocator<Vector6d>> &pointCloud){
    
    if (pointCloud.empty()){
        cerr << "Point cloud is empty!" << endl;
        return;
    }

    pangolin::CreateWindowAndBind("Point Cloud Viewer", 1024, 768);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
    );

    pangolin::View &d_cam = pangolin::CreateDisplay()
        .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f/768.0f)
        .SetHandler(new pangolin::Handler3D(s_cam));

    while (!pangolin::ShouldQuit()){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        glPointSize(2);
        glBegin(GL_POINTS);
        for (auto &p: pointCloud){
            glColor4f(p[3], p[4], p[5], 1.0);
            glVertex3d(p[0], p[1], p[2]);
        }
        glEnd();

        pangolin::FinishFrame();
        usleep(5000); // sleep 5 ms
    }
}