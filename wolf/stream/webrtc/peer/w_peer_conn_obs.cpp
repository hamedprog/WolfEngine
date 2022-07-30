#include "w_peer_conn_obs.hpp"
#include "w_peer_conn_manager.hpp"
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
      _deleting(false),
      _creation_time(rtc::TimeMicros())
{
    webrtc::PeerConnectionDependencies _dependencies(this);

    // webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::PeerConnectionInterface>> result =
    //   this->_peer_conn_manager->peer_connection_factory->CreatePeerConnectionOrError(p_config, std::move(dependencies));
    // if (result.ok())
    //{
    //     this->_pc = result.MoveValue();

    //    webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::DataChannelInterface>>
    //        error_or_channel = this->_pc->CreateDataChannelOrError("ServerDataChannel", NULL);
    //    if (error_or_channel.ok())
    //    {
    //        this->_local_channel = new w_data_channel_obs(error_or_channel.MoveValue());
    //    }
    //    else
    //    {
    //        //LOG
    //    }
    //}
    // else
    //{
    //    // LOG
    //}

    // this->_stats_callback = new rtc::RefCountedObject<w_peer_conn_stats_collector_callback>();
}

w_peer_conn_obs::~w_peer_conn_obs()
{
    W_SAFE_DELETE(this->_local_channel);
    W_SAFE_DELETE(this->_remote_channel);
    if (this->_pc.get())
    {
        this->_deleting = true;
        this->_pc->Close();
    }
}

rapidjson::Value &w_peer_conn_obs::get_ice_candidate_list()
{
    return this->_ice_candidate_list;
}

rapidjson::Value &w_peer_conn_obs::get_stats()
{
    this->_stats_callback->clear_report();
    this->_pc->GetStats(this->_stats_callback.get());
    int count = 10;
    while ((this->_stats_callback->get_report().Empty()) && (--count > 0))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return this->_stats_callback->get_report();
}

rtc::scoped_refptr<webrtc::PeerConnectionInterface> w_peer_conn_obs::get_peer_conn()
{
    return this->_pc;
}

void w_peer_conn_obs::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> p_stream)
{
    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " nb
    //     video tracks : " << stream->GetVideoTracks().size();
    //                    webrtc::VideoTrackVector videoTracks = stream->GetVideoTracks();

    /* if (this->_video_tracks.size() > 0)
     {
         this->_video_sink.reset(new w_video_sink(this->_video_tracks.at(0)));
     }*/

    // RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " nb
    //     audio tracks : " << stream->GetAudioTracks().size();
    //                    webrtc::AudioTrackVector audioTracks = stream->GetAudioTracks();

    /*  if (this->_audio_tracks.size() > 0)
      {
          this->_audio_sink.reset(new w_audio_sink(this->_audio_tracks.at(0)));
      }*/
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
}

void w_peer_conn_obs::OnIceCandidate(
    const webrtc::IceCandidateInterface *p_candidate)
{
    std::string sdp;
    if (!p_candidate->ToString(&sdp))
    {
    }
    else
    {
        //////rapidjson::Value jmessage;
        //////jmessage[kCandidateSdpMidName] = p_candidate->sdp_mid();
        //////jmessage[kCandidateSdpMlineIndexName] = p_candidate->sdp_mline_index();
        //////jmessage[kCandidateSdpName] = sdp;
        //////this->_ice_candidate_list.append(jmessage);
    }
}

void w_peer_conn_obs::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState state)
{
}

void w_peer_conn_obs::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState p_state)
{
    if ((p_state == webrtc::PeerConnectionInterface::kIceConnectionFailed) ||
        (p_state == webrtc::PeerConnectionInterface::kIceConnectionClosed))
    {
        this->_ice_candidate_list.Clear();
        if (!this->_deleting)
        {
            std::thread([this]()
                        {
                            //////////this->_peer_connection_manager->hang_up(this->_peer_id); })
                        })
                .detach();
        }
    }
}

void w_peer_conn_obs::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState)
{
}

uint64_t w_peer_conn_obs::get_creation_time() const
{
    return this->_creation_time;
}

std::string w_peer_conn_obs::get_peer_id() const
{
    return this->_peer_id;
}
