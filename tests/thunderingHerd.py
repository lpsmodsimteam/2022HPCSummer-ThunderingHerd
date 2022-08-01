import sst

userOne = sst.Component("userOne", "thunderingHerd.websiteUser")
userOne.addParams(
    {
        "websiteBrowsingLength": "10s",     # how often to check user status
        "websiteRefreshLength": "2s",       # how often to spam refresh if your request timed out
        "requestTimeoutLength": "5",        # how many cycles to wait for a cache response until one becomes impatient
        "id": "1",                          # id for cache to identify user
    }
)

userTwo = sst.Component("userTwo", "thunderingHerd.websiteUser")
userTwo.addParams({"websiteBrowsingLength": "10s", "websiteRefreshLength": "2s", "requestTimeoutLength": "5", "id": "2"})

userThree = sst.Component("userThree", "thunderingHerd.websiteUser")
userThree.addParams({"websiteBrowsingLength": "10s", "websiteRefreshLength": "2s", "requestTimeoutLength": "5", "id": "3"})

userFour = sst.Component("userFour", "thunderingHerd.websiteUser")
userFour.addParams({"websiteBrowsingLength": "10s", "websiteRefreshLength": "2s", "requestTimeoutLength": "5", "id": "4"})

userFive = sst.Component("userFive", "thunderingHerd.websiteUser")
userFive.addParams({"websiteBrowsingLength": "10s", "websiteRefreshLength": "2s", "requestTimeoutLength": "5", "id": "5"})

websiteCache = sst.Component("websiteCache", "thunderingHerd.websiteCache")
websiteCache.addParams(
    {
        "randomseed": "151515"  # random seed
    }
)

websiteServer = sst.Component("websiteServer", "thunderingHerd.websiteServer")
websiteServer.addParams(
    {
        "randomseed": "151515"  # random seed
    }
)

# Connect the nodes by their ports.
sst.Link("User_One_Link").connect(
    (userOne, "websiteCache", "1ps"), (websiteCache, "userOne", "1ps")
)
sst.Link("User_Two_Link").connect(
    (userTwo, "websiteCache", "1ps"), (websiteCache, "userTwo", "1ps")
)
sst.Link("User_Three_Link").connect(
    (userThree, "websiteCache", "1ps"), (websiteCache, "userThree", "1ps")
)
sst.Link("User_Four_Link").connect(
    (userFour, "websiteCache", "1ps"), (websiteCache, "userFour", "1ps")
)
sst.Link("User_Five_Link").connect(
    (userFive, "websiteCache", "1ps"), (websiteCache, "userFive", "1ps")
)
sst.Link("Server_Cache_Link").connect(
    (websiteServer, "websiteCache", "1ps"), (websiteCache, "websiteServer", "1ps")
)