/*
 * establish connection to WiFi
 * WiFi.begin() just needs to be kicked off.  It will manage the connection independtly
 * from that point on
 */

#ifndef WIRELESS_H
  #define WIRELESS_H

  /*****************************************************************/
  // forward declared dependencies

  class Wireless {
    private:
      // accessible only from within other members of the same class (or from their "friends")

    protected:
      // accessible from other members of the same class (or from their "friends"), 
      // but also from members of their derived classes

    public:
      // accessible from anywhere where the object is visible
      
      String WiFi_ssid;  // WiFi SSID
      String WiFi_pass;  // WiFi Password
      
      void WiFi_Client_Start(void);
      int WiFi_Client_Connected(void);
  };

  /*****************************************************************/

#endif  // WIRELESS_H
