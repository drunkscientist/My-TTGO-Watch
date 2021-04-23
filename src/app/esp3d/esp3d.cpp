#include "config.h"
#include <TTGO.h>
#include "quickglui/quickglui.h"

#include "esp3d.h"
#include "gui/mainbar/mainbar.h"
#include "hardware/wifictl.h"
//#include <WiFi.h>



//WiFiClient client;

// App icon must have an size of 64x64 pixel with an alpha channel *******************
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(example_64px); 
LV_FONT_DECLARE(Ubuntu_48px);

SynchronizedApplication exampleApp;
JsonConfig exampleconfig("example.json");

String esp3dServer, espDataPort, gCodeCmd;
String ESPmainPairValue, ESPsecondPairValue, espupdatedAt;
Label esp3dResponse, esp3dStatus, lblEspUpdatedAt;
int val = 0;

Style Ebig;

/*
 * setup routine for application
 */
void example_app_setup() {
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    exampleApp.init("example", &example_64px, true, 1, 1);
    
    // Build and configure application
    build_main_example_page();
    build_example_settings();

    // Executed when user click "refresh" button or when a WiFi connection is established
    exampleApp.synchronizeActionHandler([](SyncRequestSource source) {
        auto result = fetch_example_data(esp3dServer, espDataPort, gCodeCmd);//
        lblEspUpdatedAt.text(espupdatedAt);
        if (result)
        {
            exampleApp.icon().widgetText(ESPmainPairValue);
            esp3dResponse.text(ESPmainPairValue).alignInParentCenter(0, -30); //display data on widget
            esp3dStatus.text(ESPsecondPairValue).alignOutsideBottomMid(esp3dResponse);
            exampleApp.icon().showIndicator(ICON_INDICATOR_OK);
        } else {
            // In case of fail
            exampleApp.icon().showIndicator(ICON_INDICATOR_FAIL);
        }
    });
    
    // We want to start syncronization every time when WiFi connection is established. So we will listen system events:
    wifictl_register_cb(WIFICTL_CONNECT | WIFICTL_OFF, esp3d_wifictl_event_cb, "esp3d app widget");
}

bool esp3d_wifictl_event_cb(EventBits_t event, void *arg) {
    switch(event) {
        case WIFICTL_CONNECT:
            exampleApp.icon().hideIndicator();
            if ( exampleconfig.getBoolean("autosync", false ) )
                exampleApp.startSynchronization(SyncRequestSource::ConnectionEvent);
            break;

        case WIFICTL_OFF:
            exampleApp.icon().hideIndicator();
            break;
    }
    return true;
}

void build_main_example_page()
{
    Ebig = Style::Create(mainbar_get_style(), true);
    Ebig.textFont(&Ubuntu_48px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = exampleApp.mainPage(); // This is parent for all main screen widgets

    esp3dResponse = Label(&screen);
    esp3dResponse.text("something here")
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(Ebig, true)
        .alignInParentCenter(0, -30);

    esp3dStatus = Label(&screen);
    esp3dStatus.text(gCodeCmd)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(Ebig, true)
        .alignOutsideBottomMid(esp3dResponse);

    lblEspUpdatedAt = Label(&screen);
    lblEspUpdatedAt.text("loading...")
        .alignText(LV_LABEL_ALIGN_LEFT)
        .alignInParentTopLeft(5, 5);
}

void build_example_settings()
{
    // Create full options list and attach items to variables
    exampleconfig.addString("192.168.1.215", 32).assign(&esp3dServer);
    exampleconfig.addString("8888", 12).assign(&espDataPort);
    exampleconfig.addString("M105", 12).assign(&gCodeCmd);
    exampleconfig.addBoolean("autosync", false);
    exampleconfig.addBoolean("widget", false);

    // Switch desktop widget state based on the current settings when changed
    exampleconfig.onLoadSaveHandler([](JsonConfig& cfg) {
        bool widgetEnabled = cfg.getBoolean("widget"); // Is app widget enabled?
        if (widgetEnabled)
            exampleApp.icon().registerDesktopWidget("esp3d", &example_64px);
        else
            exampleApp.icon().unregisterDesktopWidget();
    });

    exampleApp.useConfig(exampleconfig, true); // true - auto create settings page widgets
}

bool fetch_example_data(String esp3dServer, String espDataPort, String esppair2) {

  /* 

    if (!client.connect(esp3dServer, espDataPort)){
        Serial.println("Connection failed.");
        return false;
    }
    const uint16_t port = 8888;
    const char * host = "192.168.1.215"; // ip or dns

    Serial.print("Connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    client.connect( host, port);

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        return false;
    }

    client.print("M105");
    int maxloops = 0;

    //wait for the server's reply to become available
    while (!client.available() && maxloops < 1000)
    {
        maxloops++;
        delay(1); //delay 1 msec
    }
    if (client.available() > 0)
    {
        //read back one line from the server
        espDataPort = client.readStringUntil('\r');
        
    }
    else{
    Serial.println("client.available() timed out ");
    return false;
  }

    Serial.println("Closing connection.");
    client.stop();

 
  
    char url[256]=""; float p1=0, p2=0;
    snprintf(url, sizeof(url), "192.168.1.215", esp3dServer.c_str(), esppair1.c_str(), esppair2.c_str());

    JsonRequest request(320);
    if (!request.process(url)) {
        espupdatedAt = request.errorString();
        return false;
    }

    ESPmainPairValue = ESPsecondPairValue = "";
    p1 = request[espDataPort].as<float>();
    ESPmainPairValue = String(p1, 2);
    if (request.size() > 1) { // Second currency pair available
        p2 = request[gCodeCmd].as<float>();
        ESPsecondPairValue = String(p2, 2);
    }
    
    espupdatedAt = request.formatCompletedAt("Upd: %d.%m %H:%M.%S");
    //log_i("fx rates: %d = %f, %f", doc.size(), p1, p2);
*/
  
    return true;
}
