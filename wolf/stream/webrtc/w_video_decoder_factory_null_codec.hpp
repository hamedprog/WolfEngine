/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/video_codecs/video_decoder_factory.h>
#include <api/video_codecs/sdp_video_format.h>

namespace wolf::stream::webRTC
{
   class w_video_decoder_factory_null_codec : public webrtc::VideoDecoderFactory
   {
   public:
      std::unique_ptr<webrtc::VideoDecoder> CreateVideoDecoder(
          const webrtc::SdpVideoFormat &p_format) override
      {
         return nullptr;
      }

      std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override
      {
         return std::vector<webrtc::SdpVideoFormat>();
      }
   };
} // namespace wolf::stream::webRTC