#ifndef INCLUDED_MTL_MESSAGEDISPATCHER_H
#define INCLUDED_MTL_MESSAGEDISPATCHER_H

#include "cacti/mtl/ServiceManager.h"
#include "cacti/socket/ActiveServer.h"
#include "cacti/socket/ActiveClient.h"
#include "cacti/mtl/PeersManager.h"
#include "cacti/mtl/MessageTimer.h"
#include "cacti/mtl/DispatcherInterface.h"
#include "cacti/mtl/TransferClient.h"
#include "cacti/mtl/TransferServer.h"
#include "cacti/logging/Logger.h"
#include "tag.h"

#include <string>
#include <set>

namespace cacti
{
	class MessageDispatcher : public DispatcherInterface
	{
	public:
		MessageDispatcher();
		bool start(const char* inifile);
		void stop();
		u32  appId() const						{ return m_appid; }
		u32  cfgAppId() const					{return m_cfgAppId;};
		const std::string& getAppName() const	{return m_serviceName;};
		ServiceIdentifier myIdentifier() const	{ return ServiceIdentifier(m_appid, AppPort::_apMessageD, 0);}
		const char* getErrorMesssage() const;
		const char* versioninfo()
		{
			return m_version.c_str();
		}
	public:

		bool hook(u32 portid);				// a service make a hook to me	
		void inited(u32 portid);
		bool ready(u32 portid);				// a service is ready for service
		void unhook(u32 portid);			// the service leave my hook
		
		///////////////////////////////////////////////////////////////////////
		bool getMessage(u32 portid, ServiceIdentifier& req, UserTransferMessagePtr& utm);
		bool postMessage(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		bool postMessage(const ServiceIdentifier& req, const ServiceIdentifier& res, UserTransferMessagePtr utm, bool atHead = false);

		//////////////////////////////////////////////////////////////////////////
		TimerID setTimer(u32  expires, const ServiceIdentifier& sid, UserTransferMessagePtr utm);
		void killTimer(TimerID id);
		///////////////////////////////////////////////////////////////////////
		bool isRemoteConfigure(){return m_remoteConfigure;};
		
		ApplicationData getApplicationInfo(u32 appid);

		///////////////////////////////////////////////////////////////////////////
		std::string&	getPrimAddress(){ return m_primAddress; };
		std::string&	getBackAddress(){ return m_backAddress; };
		u32				getServicePort(){ return m_serviceportid; };

		void setRemoteConfigureData(UserTransferMessagePtr utm);

	private:
		void parseURL(const std::string& url);
		bool loadLocalConfigure(const char* inifile);
		bool loadRemoteConfigure();

		void broadcastMessage(UserTransferMessagePtr utm);
		void broadcastServiceActive(u32 portid);
		void broadcastServiceReady(u32 portid);

		void notifyMissedActive(u32 portid); 

		bool isPeerActive(const ServiceIdentifier& res);
		//bool postMessageToActivePeer(const ServiceIdentifier& req, const ServiceIdentifier& res, UserTransferMessagePtr utm);
		bool postMessageToPeer(const ServiceIdentifier& req, const ServiceIdentifier& res, UserTransferMessagePtr utm);

		void onSystemMessage(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		void onConnectedToPeer(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		void onPeerDisconnected(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		void onClientClosed(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		void onServiceActive(const ServiceIdentifier& req, UserTransferMessagePtr utm, bool redundance);
		void onAppDead(u32 peerappid);
		void onCheckPingServer(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		void setPingTimer();

	private:
		std::string m_errmsg;
		cacti::Logger& m_logger;
		ServiceManager m_services;			// the shared queue for all service
		PeersManager m_peers;				// peers status manager
		MessageTimer m_msgTimer;			// message based timer

		std::string m_nameServer;			// the address of name server
		std::string m_serviceName;			// this service's name
		std::string m_groupName;			// in which group we are
		std::string m_primAddress;			// the primary address
		std::string m_backAddress;			// the backup address
		u32         m_serviceportid;		// listen portid of this service
		u32			m_appid;				// this service id
		u32			m_cfgAppId;				// configure server id

		volatile bool m_stopSignaled;		
		Thread      m_thread;
		TransferServer m_tcpserver;
		bool		m_remoteConfigure;		// is configure from remote
		std::string	m_version;

		//int			m_checkPingInterval;	// ping peers timer interval value
	};

	class SelfDispatcher : public DispatcherInterface
	{
	public:
		SelfDispatcher(MessageDispatcher* parent, const ServiceIdentifier& sid);

		bool getMessage(u32 portid, ServiceIdentifier& req, UserTransferMessagePtr& utm);
		bool postMessage(const ServiceIdentifier& req, UserTransferMessagePtr utm);
		bool postMessage(const ServiceIdentifier& req, const ServiceIdentifier& res, UserTransferMessagePtr utm, bool atHead);

	private:
		MessageDispatcher* m_parent;
		ServiceIdentifier  m_selfsid;
	};
}
#endif	//INCLUDED_MTL_MESSAGEDISPATCHER_H
