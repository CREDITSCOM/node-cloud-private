/* Send blaming letters to @yrtimd */
#ifndef __TRANSPORT_HPP__
#define __TRANSPORT_HPP__
#include <boost/asio.hpp>

#include <client/config.hpp>
#include <csnode/node.hpp>
#include <csnode/packstream.hpp>
#include <lib/system/allocators.hpp>
#include <lib/system/keys.hpp>
#include <lib/system/logger.hpp>
#include <net/network.hpp>

#include "neighbourhood.hpp"
#include "packet.hpp"
#include "pacmans.hpp"

using namespace boost::asio;

typedef uint64_t ConnectionId;
typedef uint64_t Tick;

enum class NetworkCommand : uint8_t {
  Registration = 2,
  ConfirmationRequest,
  ConfirmationResponse,
  RegistrationConfirmed,
  RegistrationRefused,
  Ping,
  PackInform,
  PackRequest,
  PackRenounce,
  BlockSyncRequest,
  SSRegistration        = 1,
  SSFirstRound          = 31,
  SSRegistrationRefused = 25,
  SSPingWhiteNode       = 32,
  SSLastBlock           = 34
};

enum class RegistrationRefuseReasons : uint8_t {
  Unspecified,
  LimitReached,
  BadId,
  BadClientVersion,
  Timeout,
  BadResponse
};

enum class SSBootstrapStatus : uint8_t { Empty, Requested, RegisteredWait, Complete, Denied };

template <>
uint16_t getHashIndex(const ip::udp::endpoint&);

class Transport {
 public:
  Transport(const Config& config, Node* node)
  : config_(config)
  , remoteNodes_(MaxRemoteNodes + 1)
  , netPacksAllocator_(1 << 24, 1)
  , myPublicKey_(node->getMyPublicKey())
  , oPackStream_(&netPacksAllocator_, node->getMyPublicKey())
  , net_(new Network(config, this))
  , node_(node)
  , nh_(this) {
    good_ = net_->isGood();
  }

  ~Transport() {
    delete net_;
  }

  void run();

  RemoteNodePtr getPackSenderEntry(const ip::udp::endpoint&);

  void processNetworkTask(const TaskPtr<IPacMan>&, RemoteNodePtr&);
  void processNodeMessage(const Message&);
  void processNodeMessage(const Packet&);

  void addTask(Packet*, const uint32_t packNum, bool incrementWhenResend = false, bool sendToNeighbours = true);
  void clearTasks();

  const PublicKey& getMyPublicKey() const {
    return myPublicKey_;
  }
  bool isGood() const {
    return good_;
  }

  void sendBroadcast(const Packet* pack) {
    nh_.sendByNeighbours(pack);
  }
  void sendDirect(const Packet*, const Connection&);

  void gotPacket(const Packet&, RemoteNodePtr&);
  void redirectPacket(const Packet&, RemoteNodePtr&);
  bool shouldSendPacket(const Packet&);

  void refillNeighbourhood();
  void processPostponed(const RoundNum);

  void sendRegistrationRequest(Connection&);
  void sendRegistrationConfirmation(const Connection&, const Connection::Id);
  void sendRegistrationRefusal(const Connection&, const RegistrationRefuseReasons);
  void sendPackRenounce(const Hash&, const Connection&);
  void sendPackInform(const Packet&, const Connection&);

  void sendPingPack(const Connection&);

  void registerMessage(MessagePtr);

  void registerTask(Packet* pack, const uint32_t packNum, const bool);

 private:
  void postponePacket(const RoundNum, const MsgTypes, const Packet&);

  // Dealing with network connections
  bool parseSSSignal(const TaskPtr<IPacMan>&);

  void dispatchNodeMessage(const MsgTypes, const RoundNum, const Packet&, const uint8_t* data, size_t);

  /* Network packages processing */
  bool gotRegistrationRequest(const TaskPtr<IPacMan>&, RemoteNodePtr&);

  bool gotRegistrationConfirmation(const TaskPtr<IPacMan>&, RemoteNodePtr&);

  bool gotRegistrationRefusal(const TaskPtr<IPacMan>&, RemoteNodePtr&);

  bool gotSSRegistration(const TaskPtr<IPacMan>&, RemoteNodePtr&);
  bool gotSSRefusal(const TaskPtr<IPacMan>&);
  bool gotSSDispatch(const TaskPtr<IPacMan>&);
  bool gotSSPingWhiteNode(const TaskPtr<IPacMan>&);
  bool gotSSLastBlock(const TaskPtr<IPacMan>&, uint32_t lastBlock);

  bool gotPackInform(const TaskPtr<IPacMan>&, RemoteNodePtr&);
  bool gotPackRenounce(const TaskPtr<IPacMan>&, RemoteNodePtr&);
  bool gotPackRequest(const TaskPtr<IPacMan>&, RemoteNodePtr&);

  bool gotPing(const TaskPtr<IPacMan>&, RemoteNodePtr&);

  void askForMissingPackages();
  void requestMissing(const Hash&, const uint16_t, const uint64_t);

  /* Actions */
  bool   good_;
  Config config_;

  static const uint32_t MaxPacksQueue  = 2048;
  static const uint32_t MaxRemoteNodes = 4096;

  std::atomic_flag sendPacksFlag_ = ATOMIC_FLAG_INIT;
  struct PackSendTask {
    Packet   pack;
    uint32_t resendTimes = 0;
    bool     incrementId;
  };
  FixedCircularBuffer<PackSendTask, MaxPacksQueue> sendPacks_;

  TypedAllocator<RemoteNode> remoteNodes_;

  FixedHashMap<ip::udp::endpoint, RemoteNodePtr, uint16_t, MaxRemoteNodes> remoteNodesMap_;

  RegionAllocator netPacksAllocator_;
  PublicKey       myPublicKey_;

  IPackStream iPackStream_;

  std::atomic_flag oLock_ = ATOMIC_FLAG_INIT;
  OPackStream      oPackStream_;

  // SS Data
  SSBootstrapStatus ssStatus_ = SSBootstrapStatus::Empty;
  ip::udp::endpoint ssEp_;

  // Registration data
  Packet    regPack_;
  uint64_t* regPackConnId_;
  bool      acceptRegistrations_ = false;

  struct PostponedPacket {
    RoundNum round;
    MsgTypes type;
    Packet   pack;

    PostponedPacket(const RoundNum r, const MsgTypes t, const Packet& p)
    : round(r)
    , type(t)
    , pack(p) {
    }
  };
  typedef FixedCircularBuffer<PostponedPacket, 1024> PPBuf;
  PPBuf                                              postponedPacketsFirst_;
  PPBuf                                              postponedPacketsSecond_;
  PPBuf* postponed_[2] = {&postponedPacketsFirst_, &postponedPacketsSecond_};

  std::atomic_flag                                                         uLock_ = ATOMIC_FLAG_INIT;
  FixedCircularBuffer<MessagePtr, PacketCollector::MaxParallelCollections> uncollected_;

  Network* net_;
  Node*    node_;

  Neighbourhood nh_;
  FixedHashMap<Hash, RoundNum, uint16_t, 10000> fragOnRound_;
};

#endif  // __TRANSPORT_HPP__
