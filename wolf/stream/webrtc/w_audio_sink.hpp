/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/media_stream_interface.h>
#include <api/scoped_refptr.h>

namespace wolf::stream::webRTC
{
  class w_audio_sink : public webrtc::AudioTrackSinkInterface
  {
  public:
    w_audio_sink(const rtc::scoped_refptr<webrtc::AudioTrackInterface> &p_track)
        : _track(p_track)
    {
      // RTC_LOG(LS_INFO) << __PRETTY_FUNCTION__ << " audiotrack:" <<
      // m_track->id();
      this->_track->AddSink(this);
    }
    virtual ~w_audio_sink()
    {
      // RTC_LOG(LS_INFO) << __PRETTY_FUNCTION__ << " audiotrack:" <<
      // m_track->id();
      this->_track->RemoveSink(this);
    }

    virtual void OnData(const void *p_audio_data, int p_bits_per_sample,
                        int p_sample_rate, size_t p_number_of_channels,
                        size_t p_number_of_frames)
    {
      // RTC_LOG(LS_VERBOSE) << __PRETTY_FUNCTION__ << "size:" << bits_per_sample
      //                     << " format:" << sample_rate << "/"
      //                     << number_of_channels << "/" << number_of_frames;
    }

  protected:
    rtc::scoped_refptr<webrtc::AudioTrackInterface> _track;
  };
}