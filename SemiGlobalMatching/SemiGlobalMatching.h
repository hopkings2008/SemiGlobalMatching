/* -*-c++-*- SemiGlobalMatching - Copyright (C) 2020.
* Author	: Ethan Li <ethan.li.whu@gmail.com>
* Describe	: header of semi-global matching class
*/

#pragma once

#include "sgm_types.h"

/**
 * \brief SemiGlobalMatching类（General implementation of Semi-Global Matching）
 */
class SemiGlobalMatching
{
public:
	SemiGlobalMatching();
	~SemiGlobalMatching();

	/** \brief SGM参数结构体 */
	struct SGMOption {
		uint8	num_paths;			// 聚合路径数 4 and 8
		sint32  min_disparity;		// 最小视差
		sint32	max_disparity;		// 最大视差
		float	uniqueness_ratio;	// 唯一性约束阈值 （最小代价-次最小代价)/最小代价 > 阈值 为有效像素

		// P1,P2 
		// P2 = P2_init / (Ip-Iq)
		sint32  p1;				// 惩罚项参数P1
		sint32  p2_init;		// 惩罚项参数P2

		SGMOption(): num_paths(8), min_disparity(0), max_disparity(640), uniqueness_ratio(0.95), p1(10), p2_init(150)
		{
		}
	};
public:
	/**
	 * \brief 类的初始化，完成一些内存的预分配、参数的预设置等
	 * \param width		输入，核线像对影像宽
	 * \param height	输入，核线像对影像高
	 * \param option	输入，SemiGlobalMatching参数
	 */
	bool Initialize(const sint32& width, const sint32& height, const SGMOption& option);

	/**
	 * \brief 执行匹配
	 * \param img_left	输入，左影像数据指针 
	 * \param img_right	输入，右影像数据指针
	 * \param disp_left	输出，左影像视差图指针，预先分配和影像等尺寸的内存空间
	 */
	bool Match(const uint8* img_left, const uint8* img_right, float32* disp_left);

	/**
	 * \brief 重设
	 * \param width		输入，核线像对影像宽
	 * \param height	输入，核线像对影像高
	 * \param option	输入，SemiGlobalMatching参数
	 */
	bool Reset(const uint32& width, const uint32& height, const SGMOption& option);

private:

	/** \brief Census变换 */
	void CensusTransform() const;

	/** \brief 代价计算	 */
	void ComputeCost() const;

	/** \brief 代价聚合	 */
	void CostAggregation() const;

	/** \brief 视差计算	 */
	void ComputeDisparity() const;

	/** \brief 内存释放	 */
	void Release();
private:
	/** \brief SGM参数	 */
	SGMOption option_;

	/** \brief 影像宽	 */
	sint32 width_;

	/** \brief 影像高	 */
	sint32 height_;

	/** \brief 左影像数据	 */
	const uint8* img_left_;

	/** \brief 右影像数据	 */
	const uint8* img_right_;
	
	/** \brief 左影像census值	*/
	uint32* census_left_;
	
	/** \brief 右影像census值	*/
	uint32* census_right_;
	
	/** \brief 初始匹配代价	*/
	uint8* cost_init_;
	
	/** \brief 聚合匹配代价	*/
	uint16* cost_aggr_;

	// ↘ ↓ ↙   5  3  7
	// →    ←	 1    2
	// ↗ ↑ ↖   8  4  6
	/** \brief 聚合匹配代价-方向1	*/
	uint8* cost_aggr_1_;
	/** \brief 聚合匹配代价-方向2	*/
	uint8* cost_aggr_2_;
	/** \brief 聚合匹配代价-方向3	*/
	uint8* cost_aggr_3_;
	/** \brief 聚合匹配代价-方向4	*/
	uint8* cost_aggr_4_;
	/** \brief 聚合匹配代价-方向5	*/
	uint8* cost_aggr_5_;
	/** \brief 聚合匹配代价-方向6	*/
	uint8* cost_aggr_6_;
	/** \brief 聚合匹配代价-方向7	*/
	uint8* cost_aggr_7_;
	/** \brief 聚合匹配代价-方向8	*/
	uint8* cost_aggr_8_;

	/** \brief 左影像视差图	*/
	float32* disp_left_;

	/** \brief 是否初始化标志	*/
	bool is_initialized_;
};

