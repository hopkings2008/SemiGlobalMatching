/* -*-c++-*- SemiGlobalMatching - Copyright (C) 2020.
* Author	: Ethan Li <ethan.li.whu@gmail.com>
* Describe	: main
*/

#include "stdafx.h"
#include "SemiGlobalMatching.h"

// opencv library
#include <opencv2/opencv.hpp>
#ifdef _DEBUG
#pragma comment(lib,"opencv_world310d.lib")
#else
#pragma comment(lib,"opencv_world310.lib")
#endif


/**
 * \brief 
 * \param argv 3
 * \param argc argc[1]:恣唹�饌珪� argc[2]: 嘔唹�饌珪� argc[3]: 篇餓夕揃抄
 * \return 
 */
int main(int argv,char** argc)
{
    if(argv < 3) {
        return 0;
    }

    //，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
    // 響函唹��
    std::string path_left = argc[1];
    std::string path_right = argc[2];
   
    cv::Mat img_left = cv::imread(path_left, cv::IMREAD_GRAYSCALE);
    cv::Mat img_right = cv::imread(path_right, cv::IMREAD_GRAYSCALE);

    if (img_left.data == nullptr || img_right.data == nullptr) {
        std::cout << "響函唹�駟О棕�" << std::endl;
        return -1;
    }
    if (img_left.rows != img_right.rows || img_left.cols != img_right.cols) {
        std::cout << "恣嘔唹�餝澳膕子志贈�" << std::endl;
        return -1;
    }


    //，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
    // SGM謄塘窃糞箭
    const sint32 width = static_cast<uint32>(img_left.cols);
    const sint32 height = static_cast<uint32>(img_right.rows);

    SemiGlobalMatching::SGMOption sgm_option;
    sgm_option.num_paths = 8;
    sgm_option.min_disparity = 0;
    sgm_option.max_disparity = 64;
    sgm_option.uniqueness_ratio = 1.0;
    sgm_option.p1 = 10;
    sgm_option.p2_init = 1500;

    SemiGlobalMatching sgm;


    //，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
    // 兜兵晒
    if(!sgm.Initialize(width, height, sgm_option)) {
        std::cout << "SGM兜兵晒払移��" << std::endl;
        return -2;
    }


    //，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
    // 謄塘
    auto disparity = new float32[width * height]();
    if(!sgm.Match(img_left.data,img_right.data,disparity)) {
        std::cout << "SGM謄塘払移��" << std::endl;
        return -2;
    }


    //，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
	// 匯崑來殊臥
    auto disparity_check = new float32[width * height]();
    cv::Mat img_left_reversal = cv::Mat(height, width, CV_8UC1);
    cv::Mat img_right_reversal = cv::Mat(height, width, CV_8UC1);
    // 鍬廬�駛�
	for (int n = 0; n < 2; n++) {
        auto data_s = (n == 0) ? img_left.data : img_right.data;
		auto data_d = (n == 0) ? img_left_reversal.data : img_right_reversal.data;
    	for (sint32 i = 0; i < height; i++) {
			auto data_row_s = data_s + i * width;
            auto data_row_d = data_d + i * width;
            for (sint32 j = 0; j < width; j++) {
                data_row_d[j] = data_row_s[width - 1 - j];
            }
        }
    }
    //cv::imshow("恣篇夕�┠�廬朔��", img_left_reversal);
    //cv::imshow("嘔篇夕�┠�廬朔��", img_right_reversal);
    // 恣嘔了崔札算恂謄塘
    //sgm.Match(img_right_reversal.data, img_left_reversal.data, disparity_check);
    // 匯崑來登僅
	for (sint32 i = 0; i < height; i++) {
        auto disp_row_lr = disparity + i * width;
        auto disp_row_rl = disparity_check + i * width;
        for (sint32 j = 0; j < width; j++) {
            float& disp_lr = disp_row_lr[j];
            sint32 col_r = lround(j - disp_lr);
            if (col_r >= 0 && col_r < width) {
                float& disp_rl = disp_row_rl[width - 1 - col_r];
                if (abs(disp_lr - disp_rl) > 1.0f) {
                    //disp_lr = Invalid_Float;
                    //disp_rl = Invalid_Float;
                }
            }else {
                //disp_lr = Invalid_Float;
            }
        }
    }
    
	//，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
    // �塋省啣醉�
    cv::Mat disp_mat = cv::Mat(height, width, CV_8UC1);
    cv::Mat disp_mat_r = cv::Mat(height, width, CV_8UC1);
    for (sint32 i = 0; i < height; i++) {
        for (sint32 j = 0; j < width; j++) {
            const float32 disp = disparity[i * width + j];
            if (disp == Invalid_Float) {
                disp_mat.data[i * width + j] = 0;
            }
            else {
                disp_mat.data[i * width + j] = 2.5 * static_cast<uchar>(disp);
            }
        }
    }
    for (sint32 i = 0; i < height; i++) {
        for (sint32 j = 0; j < width; j++) {
            const float32 disp = disparity_check[i * width + j];
            if (disp == Invalid_Float) {
                disp_mat_r.data[i * width + j] = 0;
            }
            else {
                disp_mat_r.data[i * width + j] = 2.5 * static_cast<uchar>(disp);
            }
        }
    }
    std::string disp_map_path = argc[3];
    cv::imwrite(disp_map_path, disp_mat);
    cv::imshow("篇餓夕-恣篇夕", disp_mat);
    //cv::imshow("篇餓夕-嘔篇夕", disp_mat_r);
    cv::waitKey(0);
    // 補竃留眉略泣堝(u,v,disparity��,喘泣堝箝誓罷周箝誓
    FILE* fp_disp_cloud = nullptr;
    fopen_s(&fp_disp_cloud, (disp_map_path+".cloud.txt").c_str(), "w");
    if (fp_disp_cloud) {
        for (sint32 i = 0; i < height; i++) {
            for (sint32 j = 0; j < width; j++) {
                const float32 disp = disparity[i * width + j];
                if (disp != Invalid_Float) {
                    fprintf_s(fp_disp_cloud, "%f %f %f %d %d %d\n", static_cast<float>(i), static_cast<float>(j), disp, img_left.data[i*width+j], img_left.data[i * width + j], img_left.data[i * width + j]);
                }
            }
        }
        fclose(fp_disp_cloud);
    }

    //，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，，//
    // 瞥慧坪贋
    delete[] disparity; disparity = nullptr;
    delete[] disparity_check; disparity_check = nullptr;

    system("pause");
	return 0;
}

