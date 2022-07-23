/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/video/video_frame.h>
#include <media/base/video_broadcaster.h>
#include <modules/video_capture/video_capture_factory.h>

namespace wolf::stream::webRTC
{
  class w_video_source : public rtc::VideoSourceInterface<webrtc::VideoFrame>
  {
  public:
    void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
                         const rtc::VideoSinkWants &wants) override
    {
      this->broadcaster.AddOrUpdateSink(sink, wants);
    }

    void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame> *sink) override
    {
      this->broadcaster.RemoveSink(sink);
    }

  protected:
    rtc::VideoBroadcaster broadcaster;
  };
} // namespace wolf::stream::webRTC
