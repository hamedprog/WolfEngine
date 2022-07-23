#include "w_peer_conn_obs.hpp"
#include <thread>
#include <wolf.hpp>

using w_peer_conn_obs = wolf::stream::webRTC::w_peer_conn_obs;

w_peer_conn_obs::w_peer_conn_obs(
    w_peer_conn_manager *p_peer_conn_manager,
    const std::string &p_peerid,
    const webrtc::PeerConnectionInterface::RTCConfiguration &p_config)
    : _peer_conn_manager(p_peer_conn_manager),
      _peer_id(p_peerid),
      _local_channel(nullptr),
      _remote_channel(nullptr),
      _ice_candidate_list(Json::arrayValue),
      _deleting(false),
      _creation_time(rtc::TimeMicros())
{
    // RTC_LOG(LS_INFO) << __FUNCTION__ << "CreatePeerConnection peerid:" << peerid;
    webrtc::PeerConnectionDependencies dependencies(this);

    // webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::PeerConnectionInterface>> result =
    //     this->_peer_conn_manager->_peer_connection_factory->CreatePeerConnectionOrError(p_config, std::move(dependencies));
    // if (result.ok())
    // {
    //     this->_pc = result.MoveValue();

    //     webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::DataChannelInterface>>
    //         error_or_channel = this->_pc->CreateDataChannelOrError("ServerDataChannel", NULL);
    //     if (error_or_channel.ok())
    //     {
    //         this->_local_channel = new w_data_channel_obs(error_or_channel.MoveValue());
    //     }
    //     else
    //     {
    //         // RTC_LOG(LS_ERROR)
    //         //     << __FUNCTION__ << "CreateDataChannel peerid:" << peerid << " error:" << errorOrChannel.error().message();
    //     }
    // }
    // else
    // {
    //     // RTC_LOG(LS_ERROR) << __FUNCTION__ << "CreatePeerConnection peerid:" << peerid << " error:" << result.error().message();
    // }

    this->_stats_callback = new rtc::RefCountedObject<w_peer_conn_stats_collector_callback>();
    // RTC_LOG(LS_INFO) << __FUNCTION__ << "CreatePeerConnection peerid:" << peerid;
};

w_peer_conn_obs::~w_peer_conn_obs()
{
    // RTC_LOG(LS_INFO) << __PRETTY_FUNCTION__;
    W_SAFE_DELETE(this->_local_channel);
    W_SAFE_DELETE(this->_remote_channel);
    if (this->_pc.get())
    {
        this->_deleting = true;
        this->_pc->Close();
    }
}

Json::Value w_peer_conn_obs::get_ice_candidate_list()
{
    return this->_ice_candidate_list;
}

Json::Value w_peer_conn_obs::get_stats()
{
    this->_stats_callback->clear_report();
    // this->_pc->get_stats(this->_stats_callback.get());
    int count = 10;
    while ((this->_stats_callback->get_report().empty()) && (--count > 0))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return Json::Value(this->_stats_callback->get_report());
};

rtc::scoped_refptr<webrtc::PeerConnectionInterface> w_peer_conn_obs::get_peer_conn()
{
    return this->_pc;
};

void w_peer_conn_obs::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " nb
    //     video tracks : " << stream->GetVideoTracks().size();
    //                    webrtc::VideoTrackVector videoTracks = stream->GetVideoTracks();

    // if (this->_video_tracks.size() > 0)
    // {
    //     this->_video_sink.reset(new w_video_sink(this->_video_tracks.at(0)));
    // }

    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " nb
    //     audio tracks : " << stream->GetAudioTracks().size();
    //                    webrtc::AudioTrackVector audioTracks = stream->GetAudioTracks();

    // if (this->_audio_tracks.size() > 0)
    // {
    //     this->_audio_sink.reset(new w_audio_sink(this->_audio_tracks.at(0)));
    // }
}

void w_peer_conn_obs::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__;
    this->_video_sink.reset();
    this->_audio_sink.reset();
}

void w_peer_conn_obs::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> p_channel)
{
    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__;
    this->_remote_channel = new w_data_channel_obs(p_channel);
}

void w_peer_conn_obs::OnRenegotiationNeeded()
{
    // RTC_LOG(LS_ERROR)
    //     << __PRETTY_FUNCTION__ << " peerid:" << m_peerid;
    // ;
}

void w_peer_conn_obs::OnIceCandidate(const webrtc::IceCandidateInterface *p_candidate)
{
    // RTC_LOG(LS_INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();

    std::string sdp;
    if (!p_candidate->ToString(&sdp))
    {
        // RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
    }
    else
    {
        // RTC_LOG(LS_INFO) << sdp;

        // Json::Value jmessage;
        // jmessage[kCandidateSdpMidName] = p_candidate->sdp_mid();
        // jmessage[kCandidateSdpMlineIndexName] = p_candidate->sdp_mline_index();
        // jmessage[kCandidateSdpName] = sdp;
        // m_iceCandidateList.append(jmessage);
    }
}

void w_peer_conn_obs::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state)
{
    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " state:"
    //                   << state << " peerid:" << m_peerid;
}

void w_peer_conn_obs::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState p_state)
{
    // RTC_LOG(LS_INFO) << __PRETTY_FUNCTION__ << "
    //     state : " << state  << " peerid : " << m_peerid;

    if ((p_state == webrtc::PeerConnectionInterface::kIceConnectionFailed) ||
        (p_state == webrtc::PeerConnectionInterface::kIceConnectionClosed))
    {
        this->_ice_candidate_list.clear();
        if (!this->_deleting)
        {
            // std::thread([this]()
            //             {
            //                 //this->_peer_connection_manager->hang_up(this->_peer_id); })
            //     .detach();
        }
    }
}

void w_peer_conn_obs::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState)
{
}

uint64_t w_peer_conn_obs::get_creation_time() const { return this->_creation_time; }

std::string w_peer_conn_obs::get_peer_id() const { return this->_peer_id; }