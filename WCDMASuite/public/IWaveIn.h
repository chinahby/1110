#pragma once

namespace NMWave
{
	struct IWaveIn;

	/** \brief Wave数据输入监听器接口。
	*实现此接口来处理从IWaveIn输入的Wave数据.
	*/
	struct IWaveInListener
	{
		/** \brief 有Wave数据输入。
		*@param sender 输入源
		*@param data Wave数据头地址
		*@param dataSize Wave数据长度(字节数)
		*/
		virtual void OnDataIn(IWaveIn* sender, const BYTE* data, size_t dataSize) = 0;
	};

	/** \brief 音频数据数据输入源接口。
	*定义了一个音频数据输入源的基本操作
	*/
	struct IWaveIn
	{
		/** \brief 打开输入源*/
		virtual bool Open() = 0;
		/** \brief 输入源是否已经打开*/
		virtual bool IsOpen() = 0;

		/** \brief 关闭输入源*/
		virtual bool Close() = 0;
		/** \brief 开始输入*/
		virtual bool Start() = 0;
		/** \brief 停止输入*/
		virtual bool Stop() = 0;
		/** \brief 添加一个监听器*/
		virtual void AddListener(IWaveInListener& listener) = 0;
		/** \brief 删除一个监听器*/
		virtual void RemoveListener(IWaveInListener& listener) = 0;
	};
}