#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <cmath>

using namespace std;

class node {
public:
    int src;
    int dest;
    int cost;

    node(int s, int d, int c) : src(s), dest(d), cost(c) {}

    bool operator <(const node& n) {
        return (cost < n.cost);
    }
};

void print_(const vector<node> &img_graph) {
    for(int i=0; i<img_graph.size(); ++i) {
        cout << "src: {" << img_graph[i].src << "} dest: {" << img_graph[i].dest << "} cost: {" << img_graph[i].cost << "}" << endl;
    }
}

void prepare_graph(const cv::Mat &img) {
    vector<node> img_graph;

    int width = img.cols;
    int height = img.rows;

    for(int i=0; i<width; ++i) {
        for(int j=0; j<height; ++j) {
            for(int ii=i-1; ii<=i+1; ++ii) {
                for(int jj=j-1; jj<=j+1; ++jj) {
                    if((ii >= 0) && (ii < width) && (jj >= 0) && (jj < height)) {
                        if(ii == i && jj == j)
                            continue;
                        else {
                            cv::Scalar pix1 = img.at<uchar>(j, i);
                            cv::Scalar pix2 = img.at<uchar>(jj, ii);
                            img_graph.push_back(node(j*width+i, jj*width+ii, abs((pix1[0] - pix2[0]))));
                        }
                    }
                }
            }
        }
    }
    print_(img_graph);
}

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;
//    cv::Mat img = cv::imread("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/baseball.jpg");
//    if(!img.data) {
//        cerr << "No data in the current image" << endl;
//        cerr << "Are you sure that the path is correct" << endl;
//    }

//    cv::namedWindow("image", 1);
//    cv::imshow("image", img);
//    cv::waitKey(0);

//    cout << "width {" << img.rows << "} height {" << img.cols << "}" << endl;

//    // split the image in three channels
//    cv::Mat bgr[3];
//    cv::split(img, bgr);

//    // write the separate images just for visualization.
//    cv::imwrite("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/blue.jpg", bgr[0]);
//    cv::imwrite("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/green.jpg", bgr[1]);
//    cv::imwrite("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/red.jpg", bgr[2]);

    // testing the programs sanitation

    uchar dummy_data[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 190, 200, 4, 4, 230, 245};
    cv::Mat dummy = cv::Mat(4,4, CV_8UC1, dummy_data);
    cout <<  dummy.channels() << endl;
    cout << "width {" << dummy.rows << "} height {" << dummy.cols << "}" << endl;
    cv::Scalar pix = dummy.at<uchar>(0,1);
    cout << "Current pixel is " << pix[0] << endl;
    prepare_graph(dummy);
    return 0;
}
