#include "w_create_session_des_obs.hpp"
#include "w_set_session_des_obs.hpp"
#include <wolf.hpp>

using w_create_session_des_obs = wolf::stream::webRTC::w_create_session_des_obs;

w_create_session_des_obs::w_create_session_des_obs(
	const rtc::scoped_refptr<webrtc::PeerConnectionInterface> &p_pc,
	std::promise<const webrtc::SessionDescriptionInterface *> &p_promise) : _pc(p_pc),
																			_promise(p_promise)
{
}

w_create_session_des_obs *w_create_session_des_obs::create(
	const rtc::scoped_refptr<webrtc::PeerConnectionInterface> &p_pc,
	std::promise<const webrtc::SessionDescriptionInterface *> &p_promise)
{
	return new rtc::RefCountedObject<w_create_session_des_obs>(p_pc, p_promise);
}

void w_create_session_des_obs::OnSuccess(webrtc::SessionDescriptionInterface *p_desc)
{
	std::string sdp;
	p_desc->ToString(&sdp);

	if (!this->_cancelled)
	{
		auto *session = wolf::stream::webRTC::w_set_session_des_obs::create(
			this->_pc,
			this->_promise);
		this->_pc->SetLocalDescription(session, p_desc);
	}
}

void w_create_session_des_obs::OnFailure(webrtc::RTCError p_error)
{
	// RTC_LOG(LS_ERROR) << __PRETTY_FUNCTION__ << " "
	// 				  << error.message();
	W_UNUSED(p_error);
	if (!this->_cancelled)
	{
		this->_promise.set_value(nullptr);
	}
}

void w_create_session_des_obs::cancel()
{
	this->_cancelled = true;
}
