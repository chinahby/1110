#pragma once

namespace NMWave
{
	/** \brief 音频数据输出接口*/
	struct IWaveOut
	{
		/** \brief 打开输出*/
		virtual bool Open() = 0;

		/** \brief 关闭输出*/
		virtual bool Close() = 0;

		/** \brief 停止输出*/
		virtual bool Stop() = 0;

		/** \brief 开始输出*/
		virtual bool Start() = 0;
	};
}