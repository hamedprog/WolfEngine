/*
    Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/peer_connection_interface.h>
#include <api/jsep.h>
#include <api/scoped_refptr.h>
#include <future>

namespace wolf::stream::webRTC
{
    class w_set_session_des_obs
        : public webrtc::SetSessionDescriptionObserver
    {
    public:
        static w_set_session_des_obs *create(
            const rtc::scoped_refptr<webrtc::PeerConnectionInterface> &p_pc,
            std::promise<const webrtc::SessionDescriptionInterface *> &p_promise);

        w_set_session_des_obs(
            const rtc::scoped_refptr<webrtc::PeerConnectionInterface> &p_pc,
            std::promise<const webrtc::SessionDescriptionInterface *> &p_promise);

        void OnSuccess() override;
        void OnFailure(webrtc::RTCError p_error) override;
        void cancel();

    private:
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> _pc;
        std::promise<const webrtc::SessionDescriptionInterface *> &_promise;
        bool _cancelled = false;
    };
}