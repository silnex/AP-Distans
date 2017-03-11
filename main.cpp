#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
#include <math.h>
#include <map>
#include <unistd.h>

using namespace std;
using namespace Tins;

struct APinfo{
    int count = 1;
    double distans = 0;
    string BSSID_;
    int pwd_;
    int ch_;
    string SSID_;
    /*
    Distans Distraction
    */
};

pair<double, double> AP_Direction(double a, double b, double c){
    double a_ = 0; // shadow of a
    double h = 0 ;
    a_ = ((pow(c,2)-pow(a,2)-pow(b,2))/2*b);
    h = sqrt(pow(a,2)-pow(a_,2));

    return pair<double, double>(a_,h);
}

double AP_Distans(int levelInDb, int freqInMHz){
    freqInMHz = 2412+5*freqInMHz;
    double exp = (27.55 - (20 * log10(freqInMHz)) + abs(levelInDb)) / 20.0;
    double distanceM = pow(10.0, exp);
    return distanceM;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Usage: " <<* argv << " <interface> <interface2>" << endl;
        return 1;
    }
    string iface = argv[1];
    string iface2 = argv[2];

    SnifferConfiguration config;
    config.set_snap_len(2000);
    //config.set_promisc_mode(true);
    config.set_filter("wlan type mgt subtype beacon");
    Sniffer sniffer(iface, config);
    Sniffer sniffer2(iface2, config);

    PDU *pdu = sniffer.next_packet();
    PDU *pdu2 = sniffer.next_packet();
    map<string, APinfo> APs;
    map<string, APinfo> APs2;

    while(1){
        RadioTap& Tap = pdu->rfind_pdu<RadioTap>();
        Dot11Beacon& beacon = Tap.rfind_pdu<Dot11Beacon>();

        RadioTap& Tap2 = pdu2->rfind_pdu<RadioTap>();
        Dot11Beacon& beacon2 = Tap2.rfind_pdu<Dot11Beacon>();
        usleep(1000000);
        // insert map
        if(beacon.ssid() == "BoBMil"){
            APinfo& aps = APs[beacon.addr2().to_string()];
            aps.BSSID_   = beacon.addr2().to_string();
            aps.pwd_     = static_cast<int>(Tap.dbm_signal());
            aps.ch_      = static_cast<int>(beacon.ds_parameter_set());
            aps.distans += AP_Distans(aps.pwd_,aps.ch_);
            aps.SSID_    = beacon.ssid();
            aps.count++;
        }
        pdu = sniffer.next_packet();
        if(beacon2.ssid() == "BoBMil"){
            APinfo& aps2 = APs2[beacon2.addr2().to_string()];
            aps2.BSSID_   = beacon2.addr2().to_string();
            aps2.pwd_     = static_cast<int>(Tap2.dbm_signal());
            aps2.ch_      = static_cast<int>(beacon2.ds_parameter_set());
            aps2.distans += AP_Distans(aps2.pwd_,aps2.ch_);
            aps2.SSID_    = beacon2.ssid();
            aps2.count++;
        }
        pdu2 = sniffer2.next_packet();
        double a, b, c = 0.5;
        system("clear");
        cout << "BSSID\t\t\t" << "Distans(Meter)\t" << "dBm \t" <<"SSID" << endl;
        for(auto& p : APs){
            auto& ap = p.second;
            double avg_distans = ap.distans/static_cast<double>(ap.count);
            cout << ap.BSSID_ << "\t" << avg_distans << " M \t"<< ap.pwd_<< "\t" << ap.SSID_ << endl;
            a = avg_distans;
        }
        cout << "BSSID\t\t\t" << "Distans(Meter)\t" << "dBm \t" <<"SSID" << endl;
        for(auto& p : APs2){
            auto& ap = p.second;
            double avg_distans = ap.distans/static_cast<double>(ap.count);
            cout << ap.BSSID_ << "\t" << avg_distans << " M \t"<< ap.pwd_<< "\t" << ap.SSID_ << endl;
            b = avg_distans;
        }

        pair<double,double> point;
        point = AP_Direction(b,c,a);
        cout << endl << point.first << "    " << point.second << endl;

    }
}


/*#include <iostream>
#include <set>
#include <string>
#include <tins/tins.h>
#include <math.h>
#include <map>
#include <unistd.h>
using namespace std;
using namespace Tins;

struct APinfo{
    int count = 1;
    double distans = 0;
    string BSSID_;
    int pwd_;
    int ch_;
    string SSID_;
    /*
    Distans Distraction

};

double AP_Distans(int levelInDb, int freqInMHz){
    freqInMHz = 2407+5*freqInMHz;
    double exp = (27.55 - (20 * log10(freqInMHz)) + abs(levelInDb)) / 20.0;
    double distanceM = pow(10.0, exp);
    return distanceM;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cout << "Usage: " <<* argv << " <interface>" << endl;
        return 1;
    }
    string iface = argv[1];

    SnifferConfiguration config;
    config.set_snap_len(2000);
    //config.set_promisc_mode(true);
    config.set_filter("wlan type mgt subtype beacon");
    Sniffer sniffer(iface, config);

    PDU *pdu = sniffer.next_packet();
    map<string, APinfo> APs;
    while(1){
        RadioTap& Tap = pdu->rfind_pdu<RadioTap>();
        Dot11Beacon& beacon = Tap.rfind_pdu<Dot11Beacon>();
        usleep(100000);
        // insert map

        APinfo& aps = APs[beacon.addr2().to_string()];
        aps.BSSID_   = beacon.addr2().to_string();
        aps.pwd_     = static_cast<int>(Tap.dbm_signal());
        aps.ch_      = static_cast<int>(beacon.ds_parameter_set());
        aps.distans += AP_Distans(aps.pwd_,aps.ch_);
        aps.SSID_    = beacon.ssid();
        aps.count++;
        pdu = sniffer.next_packet();

        system("clear");
        cout << "BSSID\t\t\t" << "Distans(Meter)\t" << "dBm \t" <<"SSID" << endl;
        for(auto& p : APs){
            auto& ap = p.second;
            cout << ap.BSSID_ << "\t" << ap.distans/static_cast<double>(ap.count) << " M \t"<< ap.pwd_<< "\t" << ap.SSID_ << endl;
        }
    }
}
*/
