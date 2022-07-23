/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/data_channel_interface.h>
#include <api/scoped_refptr.h>

namespace wolf::stream::webRTC
{
	class w_data_channel_obs : public webrtc::DataChannelObserver
	{
	public:
		w_data_channel_obs(rtc::scoped_refptr<webrtc::DataChannelInterface>
							   p_data_channel) : _data_channel(p_data_channel)
		{
			this->_data_channel->RegisterObserver(this);
		}

		virtual ~w_data_channel_obs()
		{
			this->_data_channel->UnregisterObserver();
		}

		// DataChannelObserver interface
		virtual void OnStateChange()
		{
			// RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << "
			// 	channel : "
			// 				  << this->_data_channel->label()
			// 				  << " state:"
			// 				  << webrtc::DataChannelInterface::DataStateString(this->_data_channel->state());
			std::string msg(this->_data_channel->label() + " " +
							webrtc::DataChannelInterface::DataStateString(this->_data_channel->state()));
			webrtc::DataBuffer buffer(msg);
			this->_data_channel->Send(buffer);
		}
		virtual void OnMessage(const webrtc::DataBuffer &p_buffer)
		{
			std::string msg((const char *)p_buffer.data.data(), p_buffer.data.size());
			// RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " channel:"
			// 				  << m_dataChannel->label() << " msg:" << msg;
		}

	protected:
		rtc::scoped_refptr<webrtc::DataChannelInterface> _data_channel;
	};
}