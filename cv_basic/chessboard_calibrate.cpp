#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <pangolin/pangolin.h>
#include <Eigen/Core>
#include <unistd.h>

using namespace std;
using namespace Eigen;
namespace fs = std::filesystem;

double boardSize = 30, baseline = 500.0; // in mm

void read_images(const string &path, vector<string> &files){
    for (const auto &entry : fs::directory_iterator(path)){
        cout << "Found file: " << entry.path() << endl;
        if(!fs::is_regular_file(entry.path()))
            continue;
        
        if (entry.path().extension() == ".png"){
            files.push_back(entry.path().string());
        }
    }
}

void calibrate_chessboard(const vector<string> &image_files, cv::Mat &cameraMatrix, cv::Mat &distCoeffs){
    // This function is a placeholder for the actual chessboard calibration logic.
    // You would typically use OpenCV functions like findChessboardCorners,
    // cornerSubPix, and calibrateCamera here.
    cout << "Calibrating using " << image_files.size() << " images." << endl;
    // Calibration code goes here...

    vector<vector<cv::Point2f>> imagePointsVector;
    cv::Size patternSize(11, 7); // Assuming a 11x7 chessboard pattern
    
    //create object points for the chessboarrd corners
    vector<cv::Point3f> objectPoints;
    for(int i = 0; i < patternSize.height; i++){
        for(int j = 0; j < patternSize.width; j++){
            objectPoints.push_back(cv::Point3f(j*boardSize, i*boardSize, 0));
        }
    }

    cv::Mat input_img;

    for (const auto &file: image_files){
        input_img = cv::Mat();
        input_img = cv::imread(file, cv::IMREAD_COLOR);
        if(input_img.empty()){
            cout << "Cannot read image: " << file << endl;
            continue;
        }
        // Process each image for calibration
        vector<cv::Point2f> imgPoints;

        cv::findChessboardCorners(input_img, patternSize, imgPoints);
        if(imgPoints.empty()){
            cout << "No corners found in image: " << file << endl;
            continue;
        }

        cv::Mat grayscale;
        cv::cvtColor(input_img, grayscale, cv::COLOR_BGR2GRAY);

        cv::cornerSubPix(grayscale, imgPoints, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));

        imagePointsVector.push_back(imgPoints);

        // cv::drawChessboardCorners(input_img, patternSize, imgPoints, true);
        // cv::imshow("Chessboard Corners", input_img);
        // cv::waitKey(100);
    }

    cout << "Calibrating camera for : " << imagePointsVector.size() << " valid images." << endl;
    // After collecting all objectPoints and imagePoints, perform calibration
    vector<vector<cv::Point3f>> objectPointsVec(imagePointsVector.size(), objectPoints);
    cv::calibrateCamera(objectPointsVec, imagePointsVector, cv::Size(input_img.cols, input_img.rows), cameraMatrix, distCoeffs, cv::noArray(), cv::noArray());
}

void undistort_image(const cv::Mat &input_img, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, cv::Mat &undistort_img){
    cout << "Camera Matrix: " << endl << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << endl << distCoeffs << endl;

    double fx = cameraMatrix.at<double>(0,0);
    double fy = cameraMatrix.at<double>(1,1);
    double cx = cameraMatrix.at<double>(0,2);
    double cy = cameraMatrix.at<double>(1,2);
    double k1 = distCoeffs.at<double>(0), k2 = distCoeffs.at<double>(1), p1 = distCoeffs.at<double>(2), p2 = distCoeffs.at<double>(3);
    int rows = input_img.rows;
    int cols = input_img.cols;
    undistort_img = cv::Mat::zeros(rows, cols, CV_8UC3);
    for(int v = 0; v < rows; v++){
        for(int u =0; u < cols; u++){
            double x_i = (u - cx) / fx;
            double y_i = (v - cy) / fy;
            double r = sqrt(x_i * x_i + y_i * y_i);

            // radial + tangential distortion
            double x_d = x_i * (1 + k1*r*r + k2*r*r*r*r) + 2*p1*x_i*y_i + p2*(r*r + 2*x_i*x_i);
            double y_d = y_i * (1 + k1*r*r + k2*r*r*r*r) + p1*(r*r + 2*y_i*y_i) + 2*p2*x_i*y_i;

            double u_d = fx * x_d + cx;
            double v_d = fy * y_d + cy;

            if(u_d >=0 && v_d >=0 && u_d < cols && v_d < rows){
                undistort_img.at<cv::Vec3b>(v, u) = input_img.at<cv::Vec3b>((int) v_d, (int) u_d);
            }
        }
    }
}

void showPointCloud(
    const vector<Vector4d, Eigen::aligned_allocator<Vector4d>> &pointCloud){
    
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
            glColor4f(p[3], p[3], p[3], 1.0);
            glVertex3d(p[0], p[1], p[2]);
        }
        glEnd();

        pangolin::FinishFrame();
        usleep(5000); // sleep 5 ms
    }
}

void create3dpointcloud(cv::Mat &imageL, cv::Mat &imageR, cv::Mat &cameraMatrix, cv::Mat &distCoeffs){

    cout << "Creating 3D point cloud from stereo images." << endl;
    
    cv::Mat grayL, grayR;
    cv::cvtColor(imageL, grayL, cv::COLOR_BGR2GRAY);
    cv::cvtColor(imageR, grayR, cv::COLOR_BGR2GRAY);
    
    double fx = cameraMatrix.at<double>(0,0);
    double fy = cameraMatrix.at<double>(1,1);
    double cx = cameraMatrix.at<double>(0,2);
    double cy = cameraMatrix.at<double>(1,2);

    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, 96, 9, 8*9*9, 32*9*9, 1, 63, 10, 100, 32);
    cv::Mat disparity_sgbm, disparity;
    sgbm->compute(grayL, grayR, disparity_sgbm);
    disparity_sgbm.convertTo(disparity, CV_32F, 1.0/16.0f);

    // compute the point cloud
    vector<Vector4d, Eigen::aligned_allocator<Vector4d>> pointCloud;
    for(int v = 0; v < grayL.rows; v+=2){
        for(int u = 0; u < grayL.cols; u+=2){
            if(disparity.at<float>(v,u) <=10.0 || disparity.at<float>(v,u) >= 96.0)
                continue;

            Vector4d point(0, 0, 0, grayL.at<uchar>(v, u) / 255.0);
            // compute the depth from disparity
            double x = (u - cx) / fx;
            double y = (v - cy) / fy;
            double depth = fx * baseline / disparity.at<float>(v, u);
            point[0] = x * depth;
            point[1] = y * depth;
            point[2] = depth;

            pointCloud.push_back(point);
        }
    }

    cv::imshow("disparity", disparity / 96.0f);
    cv::waitKey(0);

    // display point cloud
    showPointCloud(pointCloud);

}

int main(int argc, char **argv){

    if(argc != 3){
        cout << "Need left and right path with .png as argument." << endl;
        return -1;
    }

    cout << "Reading images from folder: " << argv[1] << endl;

    // read images from the folder
    vector<string> image_filesL, image_filesR;
    read_images(argv[1], image_filesL);
    read_images(argv[2], image_filesR);
    if(image_filesL.empty() || image_filesR.empty()){
        cout << "No images found in " << argv[1] << "," << argv[2] << endl;
        return -1;
    }

    cout << "Found : " << image_filesL.size() << " images in " << argv[1] << endl;
    cout << "Found : " << image_filesR.size() << " images in " << argv[2] << endl;

    cv::Mat cameraMatrixL, distCoeffsL;
    cv::Mat cameraMatrixR, distCoeffsR;
    
    calibrate_chessboard(image_filesL, cameraMatrixL, distCoeffsL);
    calibrate_chessboard(image_filesR, cameraMatrixR, distCoeffsR);

    cout << "Reading image: " << image_filesL[1] << endl;
    cv::Mat input_imgL = cv::imread(image_filesL[1], cv::IMREAD_COLOR);
    if(input_imgL.empty()){
        cout << "Cannot read left image!" << endl;
        return -1;
    }
    cout << "Reading image: " << image_filesR[1] << endl;
    cv::Mat input_imgR = cv::imread(image_filesR[1], cv::IMREAD_COLOR);
    if(input_imgR.empty()){
        cout << "Cannot read right image!" << endl;
        return -1;
    }

    cv::Mat undistort_imgL, undistort_imgR;
    undistort_image(input_imgL, cameraMatrixL, distCoeffsL, undistort_imgL);
    undistort_image(input_imgR, cameraMatrixR, distCoeffsR, undistort_imgR);

    cv::imshow("input image L", input_imgL);
    cv::imshow("input image R", input_imgR);
    cv::imshow("undistorted image L", undistort_imgL);
    cv::imshow("undistorted image R", undistort_imgR);
    cv::waitKey(0);

    create3dpointcloud(undistort_imgL, undistort_imgR, cameraMatrixL, distCoeffsL);



    return 0;

}