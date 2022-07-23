/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <api/scoped_refptr.h>
#include <api/stats/rtc_stats_collector_callback.h>
#include <json/json.h>

namespace wolf::stream::webRTC
{
	class w_peer_conn_stats_collector_callback : public webrtc::RTCStatsCollectorCallback
	{
	public:
		w_peer_conn_stats_collector_callback() {}
		void clear_report() { this->_report.clear(); }
		Json::Value get_report() { return this->_report; }

	protected:
		virtual void OnStatsDelivered(const rtc::scoped_refptr<const webrtc::RTCStatsReport> &report)
		{
			for (const webrtc::RTCStats &stats : *report)
			{
				Json::Value stats_members;
				for (const auto *member : stats.Members())
				{
					stats_members[member->name()] = member->ValueToString();
				}
				this->_report[stats.id()] = stats_members;
			}
		}

		Json::Value _report;
	};
}