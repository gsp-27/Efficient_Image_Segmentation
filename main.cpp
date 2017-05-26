#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <cmath>
#include <map>
#include <random>
#include <chrono>
#include "unionfind.h"

using namespace std;

// node of the graph, need to bypass this in the next space optimised iteration.
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

// print the image_graph.
void print_(const vector<node> &img_graph) {
    for(int i=0; i<img_graph.size(); ++i) {
        cout << "src: {" << img_graph[i].src << "} dest: {" << img_graph[i].dest << "} cost: {" << img_graph[i].cost << "}" << endl;
    }
}

// draw the segmented image
void draw_segmented_image(const map<int, vector<int> > &correspondence, const int width, const int height) {
    cv::Mat output = cv::Mat::zeros(width, height, CV_8UC3);
    for(auto it = correspondence.begin(); it != correspondence.end(); ++it) {
        // random number generator for the color.
        mt19937_64 rng;
        uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
        seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
        rng.seed(ss);
        uniform_int_distribution<int> unif(0, 255);
        // throw the 255 sided die
        int th = unif(rng);

        cv::Vec3b col((*it).first*th % 255, (*it).first*th*3%255, (*it).first*th*5%255);
        vector<int> v = (*it).second;
        // now iterate through the vector of children of this parent, and color them the same including the parent.
        for(auto it=0; it<v.size(); ++it) {
            int p = v[it];
            int j = p / height;
            int i = p - (height*j);

            // fill the point with light, row by j and col by i
            output.at<cv::Vec3b>(j, i) = col;
        }
    }
    cv::namedWindow("SegmentedImage", 1);
    cv::imshow("SegmentedImage", output);
    cv::waitKey(0);
}

// given the image graph find the connected components using the predicate of the paper.
void find_connected_components(const vector<node> img_g[], const int width, const int height) {
    int num_vertices = width * height;
    vector<UnionFind> uf(3, num_vertices); // for the three channels
    map<int, vector<int> > correspondence;

    // calculate the connected components for each channel.
    for(int ch=0; ch<3; ++ch) {
        for(int i=0; i<img_g[ch].size(); ++i) {
            int s = img_g[ch][i].src, d = img_g[ch][i].dest, c = img_g[ch][i].cost;
            bool merged = uf[ch].unionByRank(s, d, c);
        }
    }

    // do the parent merging for each of the channels.
    for(int ch=0; ch<3; ++ch) {
        for(int i=0; i<num_vertices; ++i) {
            // also calculate the number of distinct parents in the set.
            uf[ch].parent[i] = uf[ch].find(i);
        }
    }

    // develop the correspondence, two are in the same component if all the three have the same parent.
    for(int i=0; i<num_vertices; ++i) {
        if(uf[0].parent[i] == uf[1].parent[i]) {
            if(uf[0].parent[i] == uf[2].parent[i]) {
                correspondence[uf[0].parent[i]].push_back(i);
            }
            else {
                correspondence[uf[2].parent[i]].push_back(i);
            }
        }
        else {
            correspondence[uf[1].parent[i]].push_back(i);
        }
    }

    // prepare the final connected component by seeing if every pixel has the same element in every channel.

    // draw the results
    draw_segmented_image(correspondence, width, height);
}

// given the image make the grid graph, need to separate out this logic and give two choices to the user.
// 1. grid graph
// 2. nearest neighbor graph
void prepare_graph(const vector<cv::Mat> &img) {
    vector<node> img_graph[img.size()];
    int width = img[0].cols; // 120
    int height = img[0].rows; // 120


    for(int image=0; image<img.size(); ++image) {

        for(int i=0; i<width; ++i) {
            for(int j=0; j<height; ++j) {
                for(int ii=i-1; ii<=i+1; ++ii) {
                    for(int jj=j-1; jj<=j+1; ++jj) {
                        if((ii >= 0) && (ii < width) && (jj >= 0) && (jj < height)) {
                            if(ii == i && jj == j)
                                continue;
                            else {
                                cv::Scalar pix1 = img[image].at<uchar>(j, i);
                                cv::Scalar pix2 = img[image].at<uchar>(jj, ii);
                                img_graph[image].push_back(node(j*width+i, jj*width+ii, abs((pix1[0] - pix2[0]))));
                            }
                        }
                    }
                }
            }
        }
        sort(img_graph[image].begin(), img_graph[image].end());
    }
//    print_(img_graph);
    // at this point i have the graph representation of the image, only thing now remains is finding the connected component.
    // sort the image graph.
    find_connected_components(img_graph, width, height);
}

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;
    cv::Mat img = cv::imread("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/eiffel.jpg");
    if(!img.data) {
        cerr << "No data in the current image" << endl;
        cerr << "Are you sure that the path is correct" << endl;
    }

    cv::namedWindow("image", 1);
    cv::imshow("image", img);
    cv::waitKey(0);

    cout << "width {" << img.rows << "} height {" << img.cols << "}" << endl;

    // resize the image
    cv::resize(img, img, cv::Size(128, 128), cv::INTER_CUBIC);

    // apply the Gaussian Blur
    cv::GaussianBlur(img, img, cv::Size(3,3), 2, 2);
    cv::namedWindow("SmoothedImage", 1);
    cv::imshow("SmoothedImage", img);
    cv::waitKey(0);

    // split the image in three channels
    cv::Mat bgr[3];
    cv::split(img, bgr);

    // write the separate images just for visualization.
    cv::imwrite("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/blue.jpg", bgr[0]);
    cv::imwrite("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/green.jpg", bgr[1]);
    cv::imwrite("/home/gaurav/Documents/limbo/Efficient_Image_Segmentation/red.jpg", bgr[2]);

    // testing the programs sanitation

//    uchar dummy_data[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 190, 200, 4, 4, 230, 245};
//    cv::Mat dummy = cv::Mat(4,4, CV_8UC1, dummy_data);
//    cout <<  dummy.channels() << endl;
//    cout << "width {" << dummy.rows << "} height {" << dummy.cols << "}" << endl;

    // now i will create a dummy image and try to segment it out.
//    cv::Mat img_ = cv::Mat::zeros(120, 120, CV_8UC1);
//    cout << "width {" << img_.rows << "} height {" << img_.cols << "}" << endl;
//    rectangle(img_, cv::Point(60,60), cv::Point(120,120), cv::Scalar(255,255,255), cv::FILLED);
//    cv::namedWindow("origImage", 1);
//    cv::imshow("origImage", img_);
//    cv::waitKey(0);

    vector<cv::Mat> img_{bgr[0], bgr[1], bgr[2]};
    prepare_graph(img_);
    return 0;
}
