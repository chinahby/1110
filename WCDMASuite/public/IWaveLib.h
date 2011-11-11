#pragma once 

#include ".\IWaveIn.h"
#include ".\IWaveOut.h"
#include ".\IWaveMixer.h"
#include ".\IRecorder.h"
#include <string>

#ifdef WAVELIB_EXPORTS
#define WAVELIB_API __declspec(dllexport)
#else
#define WAVELIB_API __declspec(dllimport)
#endif

namespace NMWave
{
	/** \brief 音频处理库接口。*/
	struct IWaveLib
	{
		/** \brief 得到一个PC声卡的音频输入接口*/
		virtual IWaveIn& GetAudioInput() = 0;

		/** \brief 得到一个串口的音频输入接口*/
		virtual IWaveIn& GetSerialInput() = 0;

		/** \brief 得到一个PC声卡的音频输出接口*/
		virtual IWaveOut& GetAudioOutPut() = 0;

		/** \brief 得到一个串口的音频输出接口*/
		virtual IWaveOut& GetSerialOutput() = 0;

		/** \brief 创建一个混音器接口
		*@param hwndCallback 接收混音器消息的窗口。
		*混音器会在音量改变或静音设置改变时向接收窗口发送
		*MM_MIXM_LINE_CHANGE 和 MM_MIXM_CONTROL_CHANGE 消息.
		*@return 新创建的对象,需要使用IWaveMixer::Release来释放
		*/
		virtual IWaveMixer* CreateMixer(HWND hwndCallback) = 0;

		/** \brief 创建一个文件型录音器，将音频数据录制成wave文件。
		*@param fileName 文件名
		*@return 新创建的对象,需要使用IRecorder::Release来释放
		*/
		virtual IRecorder* CreateRecorder(const std::string& fileName) = 0;

		/** \brief 切换输入输出设备，调试用*/
		virtual void SwitchDevice() = 0;
	};

	/** \brief 创建一个IWaveLib接口的实例
	*@param portName 传送音频数据的串口名，如 COM3
	*@param baud 传送音频数据的串口波特率
	*/
	WAVELIB_API	IWaveLib* CreateWaveLib(const std::string& portName, DWORD baud);

	/** \brief 释放一个由CreateWaveLib创建的IWaveLib接口的实例	*/
	WAVELIB_API void ReleaseWaveLib(IWaveLib* pWaveLib);
}
