#include<iostream>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <cmath>

cv::Mat generateImage(std::string imgPath, int rows, int cols, bool userInput){
    if(userInput){
        cv::Mat img = cv::imread(imgPath, CV_8UC1);
        return img;
    }
    else{
        cv::Mat img = cv::Mat::zeros(cv::Size(rows, cols), CV_8UC1);
        int step = 256 / (rows * cols);
        uchar pixIntensity = step;
        for(int row = 0 ; row < rows ; row++){
            for (int col = 0 ; col < cols ; col++){
                img.at<uchar>(row, col) = pixIntensity;
                pixIntensity += step;
            }
        }
        return img;
    }
}

void pasteImageToDst(const cv::Mat &in_img, cv::Mat &out_img){
    int in_rows = in_img.rows;
    int in_cols = in_img.cols;
    int row_offset = (out_img.rows - in_rows) / 2;
    int col_offset = (out_img.cols - in_cols) / 2;
    for(int row = 0; row < in_rows ; row++){
        int dst_row = row + row_offset;
        for(int col = 0; col < in_cols ; col++){
            int dst_col = col + col_offset;
            out_img.at<uchar>(dst_row, dst_col) = in_img.at<uchar>(row, col);
        }
    }
}

cv::Mat removeOffset(cv::Mat& in_img, cv::Mat& out_img, double angle){
    int img_center_row = out_img.rows / 2;
    int img_center_col = out_img.cols / 2;
    int row_offset = ( out_img.rows - in_img.rows) / 2;
    int col_offset = ( out_img.cols - in_img.cols) / 2;

    std::vector<int> x_corners;
    std::vector<int> y_corners;

    x_corners.push_back(col_offset);
    x_corners.push_back(col_offset + in_img.cols);
    y_corners.push_back(row_offset);
    y_corners.push_back(row_offset + in_img.rows);
    double cosine_val = cos(angle);
    double sine_val = sin(angle);

    int x_min = out_img.cols, x_max = 0 , y_min = out_img.rows, y_max = 0;

    for(int x = 0 ; x < x_corners.size() ; x++){
        for(int y = 0 ; y < y_corners.size() ; y++){
            double x_trans = x_corners[x] - img_center_col;
            double y_trans = y_corners[y] - img_center_row;
            int x_rotate = (cosine_val * x_trans) - (sine_val * y_trans);
            int y_rotate = (sine_val * x_trans) + (cosine_val * y_trans);           
            x_rotate = x_rotate + img_center_col;
            y_rotate = y_rotate + img_center_row;
            if(x_rotate >= 0 && x_rotate < out_img.cols && y_rotate >= 0 && y_rotate < out_img.rows){
                x_min = std::min(x_min, x_rotate);
                x_max = std::max(x_max, x_rotate);
                y_min = std::min(y_min, y_rotate);
                y_max = std::max(y_max, y_rotate);
            }
        }
    }

    cv::Mat final_img = cv::Mat::zeros(cv::Size(x_max - x_min, y_max - y_min), out_img.type());

    for(int row = 0 ; row < final_img.rows ; row++){
        int out_map_row = y_min + row;
        for(int col = 0 ; col < final_img.cols ; col++){
            int out_map_col = x_min + col;
            final_img.at<uchar>(row, col) = out_img.at<uchar>(out_map_row, out_map_col);
        }
    }
    return final_img;
}

cv::Mat rotate(cv::Mat& in_img, double angle ){
    angle = angle * 3.14159/180;    //convert angle in degree to radians
    int rotated_dim = sqrt(pow(in_img.rows, 2) + pow(in_img.cols, 2));
    int rotated_dim_w = rotated_dim, rotated_dim_h = rotated_dim;
    cv::Mat temp_img = cv::Mat::zeros(cv::Size(rotated_dim, rotated_dim), in_img.type());
    cv::Mat final_img = cv::Mat::zeros(cv::Size(rotated_dim, rotated_dim), in_img.type());
    // cv::bitwise_not(temp_img, temp_img);
    pasteImageToDst(in_img, temp_img);

    int img_center_row = temp_img.rows / 2;
    int img_center_col = temp_img.cols / 2;

    double cosine_val = cos(angle);
    double sine_val = sin(angle);

    for(int row = 0 ; row < temp_img.rows ; row++){
        for(int col = 0 ; col < temp_img.cols ; col++){
            double x_trans = col - img_center_col;
            double y_trans = row - img_center_row;
            int x_rotate = (cosine_val * x_trans) - (sine_val * y_trans);
            int y_rotate = (sine_val * x_trans) + (cosine_val * y_trans);           
            x_rotate = x_rotate + img_center_col;
            y_rotate = y_rotate + img_center_row;
            if(x_rotate >= 0 && x_rotate < temp_img.cols && y_rotate >= 0 && y_rotate < temp_img.rows){
                final_img.at<uchar>(y_rotate, x_rotate) = temp_img.at<uchar>(row, col);
            }
        }
    }
    final_img = removeOffset(in_img, final_img, angle);
    return final_img;
}

int main(){
    std::string imgPath = "input_img.tif";
    int rows = 5 , cols = 5;
    cv::Mat img = generateImage(imgPath, rows, cols, true);
    cv::imwrite("raw.tif", img);
    cv::Mat img30 = rotate(img, 30);
    cv::imwrite("img30.tif", img30);
    cv::Mat img45 = rotate(img, 45);
    cv::imwrite("img45.tif", img45);
    cv::Mat img90 = rotate(img, 90);
    cv::imwrite("img90.tif", img90);


}