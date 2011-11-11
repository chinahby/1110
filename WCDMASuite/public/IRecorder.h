#pragma once
#include ".\IWaveOut.h"
#include ".\IWaveIn.h"

namespace NMWave
{
	struct IRecorder: public IWaveOut, public IWaveInListener
	{
		/** \brief 释放本队象*/
		virtual void Release() = 0;
	};
};