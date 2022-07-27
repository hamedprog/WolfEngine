/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <string>
#include <memory>
#include <stream/webrtc/interceptor/w_ice_server.hpp>

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/video_codecs/video_decoder_factory.h>
#include <api/video_codecs/video_encoder_factory.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
    class w_peer_conn_manager_helpers
    {
    public:
        // character to remove from url to make webrtc label
        static constexpr bool ignore_in_label(char c)
        {
            return c == ' ' || c == ':' || c == '.' || c == '/' || c == '&';
        }

        static std::string get_server_ip_from_client_ip(int p_client_ip);

        static w_ice_server get_ice_server_from_url(
            const std::string& p_url,
            const std::string& p_client_ip = "");

        static std::unique_ptr<webrtc::VideoEncoderFactory> create_encoder_factory(bool p_use_null_codec);
        static std::unique_ptr<webrtc::VideoDecoderFactory> create_decoder_factory(bool p_use_null_codec);
    };
} // namespace wolf::stream::webRTC