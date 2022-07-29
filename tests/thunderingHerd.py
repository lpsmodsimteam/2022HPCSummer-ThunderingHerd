import sst

userOne = sst.Component("userOne", "thunderingHerd.websiteUser")
userOne.addParams(
    {
        "websiteBrowsingLength": "10s",  # how long one thinks after eating
        "websiteRefreshLength": "2s",      # how long to wait between checking if they need to put down their chopsticks
        "requestTimeoutLength": "5",     # random seed
        "id": "1",                  # id for table to identify philosopher
    }
)

userTwo = sst.Component("userTwo", "thunderingHerd.websiteUser")
userTwo.addParams({"thinkingDuration": "7s", "waitingClock": "16s", "randomseed": "123", "eatingduration": "5", "id": "2", "livelockCheck": "1000", "windowSize": "100"})

userThree = sst.Component("userThree", "thunderingHerd.websiteUser")
userThree.addParams({"thinkingDuration": "11s", "waitingClock": "16s", "randomseed": "348734", "eatingduration": "5", "id": "3", "livelockCheck": "1000", "windowSize": "100"})

userFour = sst.Component("userFour", "thunderingHerd.websiteUser")
userFour.addParams({"thinkingDuration": "13s", "waitingClock": "16s", "randomseed": "123", "eatingduration": "5", "id": "4", "livelockCheck": "1000", "windowSize": "100"})

userFive = sst.Component("userFive", "thunderingHerd.websiteUser")
userFive.addParams({"thinkingDuration": "15s", "waitingClock": "16s", "randomseed": "348734", "eatingduration": "5", "id": "5", "livelockCheck": "1000", "windowSize": "100"})

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

### Enable statistics
sst.setStatisticLoadLevel(2)

# Output statistics to CSV. 
sst.setStatisticOutput("sst.statOutputCSV", { "filepath" : "statsoutput.csv", "separator" : "," } ) 

## Enable statistics on the components
sst.enableAllStatisticsForComponentType("thunderingHerd.websiteUser")


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