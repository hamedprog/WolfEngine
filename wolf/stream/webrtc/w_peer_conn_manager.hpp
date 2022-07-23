/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include "w_peer_conn_obs.hpp"
#include <api/peer_connection_interface.h>
#include <api/video_codecs/video_decoder_factory.h>
#include <future>
#include <modules/audio_device/include/audio_device.h>
#include <mutex>
#include <p2p/client/basic_port_allocator.h>
#include <regex>
#include <rtc_base/logging.h>
#include <rtc_base/strings/json.h>
#include <stream/http/w_http_server.hpp>
#include <string>
#include <thread>

namespace wolf::stream::webRTC
{
    class w_peer_conn_manager
    {
    public:
        w_peer_conn_manager(const std::list<std::string> &p_ice_server_list,
                            const Json::Value &p_config,
                            webrtc::AudioDeviceModule::AudioLayer p_audio_layer,
                            const std::string &p_publish_filter,
                            const std::string &p_webrtc_udp_port_range,
                            bool p_use_null_codec = true,
                            bool p_use_plan_b = true,
                            int p_max_pc = 0);

        virtual ~w_peer_conn_manager();

        bool get_is_initialized() const;
        const std::map<std::string, http::w_http_function> get_http_api() const;
        const Json::Value get_ice_candidate_list(const std::string &peerid);
        const Json::Value add_ice_candidate(
            const std::string &p_peer_id,
            const Json::Value &p_jmessage);
        // const Json::Value get_video_device_list();

        // const Json::Value get_audio_device_list();

        // const Json::Value get_media_list();

        // const Json::Value hang_up(const std::string &peerid);

        // const Json::Value call(const std::string &p_peerid,
        //                        const std::string &p_videourl,
        //                        const std::string &p_audiourl,
        //                        const std::string &p_options,
        //                        const Json::Value &p_message);

        // const Json::Value get_ice_servers(const std::string &p_client_ip);

        // const Json::Value get_peer_conn_list();

        // const Json::Value get_stream_list();

        // const Json::Value create_offer(const std::string &p_peer_id,
        //                                const std::string &p_video_url,
        //                                const std::string &p_audio_url,
        //                                const std::string &p_options);

        // const Json::Value set_answer(const std::string &p_peerid,
        //                              const Json::Value &p_message);

        // std::pair<std::map<std::string, std::string>, Json::Value> whip(const struct mg_request_info *p_req_info,
        //                                                                 const Json::Value &p_in);

    protected:
        // w_peer_conn_manager *create_peer_conn(const std::string &p_peerid);

        // bool add_streams(webrtc::PeerConnectionInterface *peer_connection,
        //                  const std::string &p_video_url,
        //                  const std::string &p_audio_url,
        //                  const std::string &p_options);

        // rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> create_video_source(
        //     const std::string &p_videourl,
        //     const std::map<std::string, std::string> &p_opts);

        // rtc::scoped_refptr<webrtc::AudioSourceInterface> create_audio_source(
        //     const std::string &p_audio_url,
        //     const std::map<std::string, std::string> &p_opts);

        // bool stream_still_used(const std::string &p_stream_label);

        // const std::list<std::string> get_video_capture_device_list();

        // rtc::scoped_refptr<webrtc::PeerConnectionInterface> get_peer_conn(const std::string &p_peer_id);

        // const std::string sanitize_label(const std::string &p_label);

        // void create_audio_module(webrtc::AudioDeviceModule::AudioLayer p_audio_layer);

        // std::unique_ptr<webrtc::SessionDescriptionInterface> get_answer(
        //     const std::string &p_peerid,
        //     webrtc::SessionDescriptionInterface *p_session_description,
        //     const std::string &p_videourl,
        //     const std::string &p_audiourl,
        //     const std::string &p_options);

        // std::string get_oldest_peer_conn();

    protected:
        std::unique_ptr<rtc::Thread> _signaling_thread;
        std::unique_ptr<rtc::Thread> _worker_thread;
        std::unique_ptr<rtc::Thread> _network_thread;
        typedef std::pair<rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>, rtc::scoped_refptr<webrtc::AudioSourceInterface>> AudioVideoPair;
        rtc::scoped_refptr<webrtc::AudioDecoderFactory> _audio_decoder_factory;
        std::unique_ptr<webrtc::TaskQueueFactory> _task_queue_factory;
        rtc::scoped_refptr<webrtc::AudioDeviceModule> _audio_device_module;
        std::unique_ptr<webrtc::VideoDecoderFactory> _video_decoder_factory;
        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peer_connection_factory;
        std::mutex _peer_map_mutex;
        std::map<std::string, w_peer_conn_obs *> _peer_conn_obs_map;
        std::map<std::string, AudioVideoPair> _stream_map;
        std::mutex _stream_map_mutex;
        std::list<std::string> _ice_server_list;
        const Json::Value _config;
        std::map<std::string, std::string> _video_audio_map;
        const std::regex _publish_filter;
        std::map<std::string, http::w_http_function> _funcs;
        std::string _webrtc_port_range;
        bool _use_null_codec;
        bool _use_plan_b;
        int _max_pc;
    };
}