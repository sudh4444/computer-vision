#include <opencv2/opencv.hpp>
#include <string>

using namespace std;

int main(int argc, char **argv){
    char *image_file = argv[1];
    cv::Mat input_image = cv::imread(image_file, cv::IMREAD_GRAYSCALE);
    if(input_image.empty()){
        printf("Could not open or find the image: %s\n", image_file);
        return 0;
    }

    // rad-tad model params
    double k1 = -0.28340811, k2 = 0.07395907,
           p1 = 0.00019359, p2 = 1.76187114e-05;
    
    double fx = 458.654, fy = 457.296;
    double cx = 367.215, cy = 248.375;

    int rows = input_image.rows;
    int cols = input_image.cols;
    // cv::Mat image_undistort = cv::Mat(rows, cols, CV_8UC1);
    cv::Mat image_undistort = cv::Mat::zeros(rows+100, cols+100, CV_8UC1);

    // compute the pixels in the undistored one
    for (int v=0; v < rows; v++){
        for (int u=0; u < cols; u++){

            double x = (u - cx) / fx;
            double y = (v - cy) / fy;
            double r = sqrt(x*x + y*y);
            double x_distorted = x * (1 +k1 * r*r + k2* r*r*r*r) + 2*p1*x*y + p2*(r*r + 2*x*x);
            double y_distorted = y * (1 +k1 * r*r + k2* r*r*r*r) + p1*(r*r + 2*y*y) + 2*p2*x*y;
            
            double u_distorted = fx * x_distorted + cx;
            double v_distorted = fy * y_distorted + cy;
            
            // check if the pixel is in the image borders
            if (u_distorted >=0 && v_distorted >=0 && u_distorted < cols && v_distorted < rows){
                image_undistort.at<uchar>(v, u) = input_image.at<uchar>((int) v_distorted, (int) u_distorted);
            }
            else{
                image_undistort.at<uchar>(v, u) = 0;
            }
        }
    }

    // show the undistorted image
    cv::imshow("input image", input_image);
    cv::imshow("undistorted image", image_undistort);
    cv::waitKey(0);

    return 0;

}