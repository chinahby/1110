#pragma once

namespace NMWave
{
	struct IWaveMixer
	{
		/**\brief 检测是否麦克风设置为静音
		*@param bMute [out]当前麦克风静音设置
		*@return 查询操作是否成功
		*/
		virtual bool IsMute(bool& bMute) = 0;

		/**\brief 设置麦克风是否为静音
		*@param bMute 新麦克风静音设置
		*@return 操作是否成功
		*/
		virtual bool SetMute(bool bMute) = 0;
		/**\brief 查询扬声器音量
		*@param vol[out] 当前音量
		*@return 操作是否成功
		*/
		virtual bool GetVolume(DWORD& vol) = 0;
		/**\brief 设置扬声器音量
		*@param vol 新音量
		*@return 操作是否成功
		*/
		virtual bool SetVolume(DWORD vol) = 0;

		/**\brief 查询扬声器音量范围
		*@param minVol[out] 最小音量
		*@param maxVol[out] 最大音量
		*@return 操作是否成功
		*/
		virtual bool GetVolumeRange(DWORD& minVol, DWORD& maxVol) = 0;

		/** \brief 释放本对象 */
		virtual void Release() = 0;
	};
}