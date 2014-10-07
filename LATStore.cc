#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <libmemcached/memcached.hpp>
#include <boost/noncopyable.hpp>

using namespace std;
using namespace memcache;

class LATStore : private boost::noncopyable
{
    public:
        static const uint32_t numClients = 10;

        static LATStore& getLATStore()
        {
            static LATStore instance;
            return instance;
        }

        bool update(string& sessionId, time_t lastAccessTime)
        {
            time_t expiry = 30*60;
            uint32_t flags = 0;
            if (!m_client->set(sessionId, (const char*) &lastAccessTime, sizeof(time_t),  expiry, flags)) {
                cout<<"Failed to update session LAT"<<endl;
                string errorMsg;
                if (m_client->error(errorMsg)) {
                    cout<<"Failed to update session "<<errorMsg<<endl;
                }
                return false;
            }
            cout<<"Updated "<<sessionId<<" with "<<lastAccessTime<<endl;
            return true;
        }
        
        bool getLAT(string& sessionId, time_t& lastAccessTime)
        {
            vector<char> retValue;
            if (!m_client->get(sessionId, retValue)) {
                string errorMsg;
                if (m_client->error(errorMsg)) {
                    cout<<"Failed to read session "<<errorMsg<<endl;
                }
                return false;
            }
            string buf = string(retValue.begin(), retValue.end());
            memcpy(&lastAccessTime, buf.c_str(), sizeof(time_t));
            cout<<" Last access time for sessionId is "<<lastAccessTime<<endl;
            return true;
        }


    private:
        
        Memcache* m_client;

        LATStore() 
        {
            m_client = new Memcache("127.0.0.1", 11211);
            string errorMsg;
            if (m_client->error(errorMsg)) {
                cout<<"Failed to create "<<errorMsg<<endl;
            }
        }

        ~LATStore()
        { 
            delete m_client;
        }
};


int main()
{
    vector<string> sids;
    for (int ii = 0; ii < 100; ii++) {
        ostringstream sidstream;
        sidstream<<"sid"<<(rand()%10000 + ii);
        sids.push_back(sidstream.str());
    }

    for (int ii = 0; ii < 1000; ii++) {
        ostringstream sidstream;
        int index = rand() % 100;
        LATStore::getLATStore().update(sids[index], time(NULL));
        time_t time;
        LATStore::getLATStore().getLAT(sids[index], time);
        sleep(1);
    }
    
}

