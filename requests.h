/**
 * @file requests.h
 * @brief  This file defines objects that allow us to send requests back and 
 * forth between the users, the server, and the cache
 * 
 */

/**
 * @brief This is used for our websiteUser components, and it keeps track 
 * of which state they are currently in
 * 
 */
enum userStatus {
	BROWSING,
	REFRESHING,
	REQUESTING,
	WAITING
};

/**
 * @brief This is used by the cache to keep track of whether incoming 
 * messages are coming from the user or from the server  
 * 
 */
enum requester {
	USER,
	SERVER
};

/**
 * @brief This is a request recieved by the user with url updates, used by the 
 * cache
 * 
 */
struct UserRequest { 
	std::string websiteUrl;
	bool validSite;
};

/**
 * @brief This is a request recieved by the cache with either requests for urls 
 * from the users, or updated urls from the server
 * 
 */
struct CacheRequest { 
	requester request;			// differentiates users from server
	int64_t id; 				// 0 for server, 1+ for users
	std::string pageRequested;	// name of website
	std::string urlRequested;	// url (only used by server)
	bool successfulReturn; 		// for server to use to mark success of request
};

/**
 * @brief This is a request recieved by the server with url requests from the 
 * cache
 * 
 */
struct ServerRequest { 
	std::string pageRequested;
	int64_t id; // id of user requesting page (may or may not be necessary)

};

/**
 * @brief This is an object to be used in the map for the cache, which keeps 
 * traack of both the url, and how recently this url was last accessed so we 
 * can implement a LRU policy.
 * 
 */
struct cacheObject {
	std::string websiteUrl;
	long unsigned int websiteAge;
};

/*! 
 * This class creates a message that the dining table sends over to one of the 
 * philosophers when they request a chopstick.  The ChopstickRequest object is 
 * created separatley, then passed into this class and serialized.  By doing so, 
 * the data is organized in such a way that after its sent over its link to 
 * the philosopher, that component knows how to read over and extract 
 * information from this event.
 *
 */
class UserRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & userreq.websiteUrl;
		ser & userreq.validSite;
	}
	
	/**
	 * @brief Construct a new User Request Event object that wraps the 
	 * UserRequest struct
	 * 
	 * @param userreq User Request Struct that contains information 
	 * regarding whether or not the philosopher can pick up a chopstick
	 */
	UserRequestEvent(UserRequest userreq) :
		Event(),
		userreq(userreq)
	{}
	
	UserRequestEvent() {} // For Serialization only
	UserRequest userreq; 

	// this serializes the event we created so it can be sent over a link
	ImplementSerializable(UserRequestEvent); 
};

/*! 
 * This class creates a message that the dining philosopher sends over to the 
 * table when they request a chopstick.  The PhilosopherRequest object is 
 * created separatley, then passed into this class and serialized.  By doing so, 
 * the data is organized in such a way that after its sent over its link to 
 * the table, that component knows how to read over and extract 
 * information from this event.
 *
 */
class CacheRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & cachereq.request;
		ser & cachereq.id;
		ser & cachereq.pageRequested;
		ser & cachereq.urlRequested;
		ser & cachereq.successfulReturn;
	}
	
	/**
	 * @brief Construct a new Philosopher Request Event object that wraps the 
	 * PhilosopherRequest struct
	 * 
	 * @param cachereq Philosopher Request struct that contains information 
	 * regarding which chopstick is being requested or sent over the link
	 */
	CacheRequestEvent(CacheRequest cachereq) :
		Event(),
		cachereq(cachereq)
	{}

	CacheRequestEvent() {} // For Serialization only
	CacheRequest cachereq; 

	// this serializes the event we created so it can be sent over a link
	ImplementSerializable(CacheRequestEvent); 
};

class ServerRequestEvent : public SST::Event {

public:
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & serverreq.pageRequested;
		ser & serverreq.id;
	}
	
	/**
	 * @brief Construct a new Philosopher Request Event object that wraps the 
	 * PhilosopherRequest struct
	 * 
	 * @param serverreq Philosopher Request struct that contains information 
	 * regarding which chopstick is being requested or sent over the link
	 */
	ServerRequestEvent(ServerRequest serverreq) :
		Event(),
		serverreq(serverreq)
	{}

	ServerRequestEvent() {} // For Serialization only
	ServerRequest serverreq; 

	// this serializes the event we created so it can be sent over a link
	ImplementSerializable(ServerRequestEvent); 
};
