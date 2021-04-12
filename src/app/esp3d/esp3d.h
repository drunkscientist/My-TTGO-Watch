#ifndef _esp3d_APP_H
    #define _esp3d_APP_H

    #include <TTGO.h>

    void example_app_setup();

    bool esp3d_wifictl_event_cb(EventBits_t event, void *arg);
    bool fetch_example_data(String esp3dServer, String espDataPort, String gCodeCmd);

    void build_main_example_page();//might neeed to edit this and the next too
    void build_example_settings();

#endif // _esp3d_APP_H