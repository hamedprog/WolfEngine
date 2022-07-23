/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include "w_audio_sink.hpp"
#include "w_data_channel_obs.hpp"
#include "w_peer_conn_stats_collector_callback.hpp"
#include "w_video_sink.hpp"
#include <api/peer_connection_interface.h>
#include <chrono>
#include <json/json.h>

namespace wolf::stream::webRTC
{
    class w_peer_conn_manager;
    class w_peer_conn_obs : public webrtc::PeerConnectionObserver
    {
    public:
        w_peer_conn_obs(w_peer_conn_manager *p_peer_conn_manager,
                        const std::string &p_peerid,
                        const webrtc::PeerConnectionInterface::RTCConfiguration &p_config);

        virtual ~w_peer_conn_obs();

        Json::Value get_ice_candidate_list();

        Json::Value get_stats();

        rtc::scoped_refptr<webrtc::PeerConnectionInterface> get_peer_conn();

        // PeerConnectionObserver interface
        virtual void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

        virtual void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

        virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> p_channel);

        virtual void OnRenegotiationNeeded();

        virtual void OnIceCandidate(const webrtc::IceCandidateInterface *candidate);

        virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state);

        virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState p_state);

        virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState);

        uint64_t get_creation_time() const;

        std::string get_peer_id() const;

    private:
        w_peer_conn_manager *_peer_conn_manager;
        const std::string _peer_id;
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> _pc;
        w_data_channel_obs *_local_channel;
        w_data_channel_obs *_remote_channel;
        Json::Value _ice_candidate_list;
        rtc::scoped_refptr<w_peer_conn_stats_collector_callback> _stats_callback;
        std::unique_ptr<w_video_sink> _video_sink;
        std::unique_ptr<w_audio_sink> _audio_sink;
        bool _deleting;
        uint64_t _creation_time;
    };
}