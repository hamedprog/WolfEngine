/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/media_stream_interface.h>
#include <api/scoped_refptr.h>
#include <api/video/video_frame.h>

namespace wolf::stream::webRTC
{
  class w_video_sink : public rtc::VideoSinkInterface<webrtc::VideoFrame>
  {
  public:
    w_video_sink(const rtc::scoped_refptr<webrtc::VideoTrackInterface> &p_track)
        : _track(p_track)
    {
      // RTC_LOG(LS_INFO) << __PRETTY_FUNCTION__ << " videotrack:" <<
      // m_track->id();
      this->_track->AddOrUpdateSink(this, rtc::VideoSinkWants());
    }

    virtual ~w_video_sink()
    {
      // RTC_LOG(LS_INFO) << __PRETTY_FUNCTION__ << " videotrack:" <<
      // m_track->id();
      this->_track->RemoveSink(this);
    }

    // VideoSinkInterface implementation
    virtual void OnFrame(const webrtc::VideoFrame &p_video_frame)
    {
      rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(
          p_video_frame.video_frame_buffer()->ToI420());
      // RTC_LOG(LS_VERBOSE) << __PRETTY_FUNCTION__ << " frame:" <<
      // buffer->width()
      //                     << "x" << buffer->height();
    }

  protected:
    rtc::scoped_refptr<webrtc::VideoTrackInterface> _track;
  };
}