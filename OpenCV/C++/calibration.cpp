#include "opencv2/features2d/features2d.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <iostream> 

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
        
    int numBoards = 41;
    int board_w = 8;
    int board_h = 6;

    Size board_sz = Size(board_w, board_h);
    int board_n = board_w*board_h;

    vector<vector<Point3f> > object_points;
    vector<vector<Point2f> > image_points;
    vector<Point2f> corners;

    vector<Point3f> obj;
    for (int j=0; j<board_n; j++)
    {
        obj.push_back(Point3f(j/board_w, j%board_w, 0.0f));
    }

    Mat img, gray;
    VideoCapture cap = VideoCapture(0);

    cout << " VideoCapture Open " << endl;

    int success = 0;
    int k = 0;
    bool found = false;
    while (success < numBoards)
    {
        cap >> img;
        cvtColor(img, gray, CV_BGR2GRAY);
        found = findChessboardCorners(gray, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if (found)
        {
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(gray, board_sz, corners, found);
        }

        imshow("image", img);
        imshow("corners", gray);

        k = waitKey(1);

        cout << " k = "  << k << endl;        
        if (found)
        {
            k = waitKey(0);
        }
        if (k == 27)    // if press ESC program will stop
        {
            break;
        }
        if (k == ' ' && found !=0)  //// if press space bar program got success
        {
            image_points.push_back(corners);
            object_points.push_back(obj);
            cout << " Corners stored " << endl;
            success++;

            cout << " success = "  << success << endl;        
            cout << " numBoards = "  << numBoards << endl;        

            if (success >= numBoards)
            {
                break;
            }
        }

    }
    destroyAllWindows();
    cout << " Starting calibration " << endl;//   printf ("Corners stored\n");
    
    Mat intrinsic = Mat(3, 3, CV_32FC1);
    Mat distcoeffs;
    vector<Mat> rvecs, tvecs;

    intrinsic.at<float>(0, 0) = 1;
    intrinsic.at<float>(1, 1) = 1;
    
    calibrateCamera(object_points, image_points, img.size(), intrinsic, distcoeffs, rvecs, tvecs);

    FileStorage fs1("mycalib.yml", FileStorage::WRITE);
    fs1 << "CM1" << intrinsic;
    fs1 << "D1" << distcoeffs;

    cout << " calibration done " << endl;


    Mat imgU;
    while(1)
    {
        cap >> img;
        undistort(img, imgU, intrinsic, distcoeffs);

        imshow("image", img);
        imshow("undistort", imgU);

        k = waitKey(5);
        if (k == 27)   // if press ESC program will stop.
        {
            break;
        } 
    }

    
    cap.release();
    
    return(0);
}
