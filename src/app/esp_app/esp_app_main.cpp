/****************************************************************************
 *   Aug 3 12:17:11 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "config.h"
#include <TTGO.h>

#include "esp_app.h"
#include "esp_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "WiFi.h"
#include "quickglui/quickglui.h"


const uint16_t port = 8888;
const char * host = "192.168.1.215"; // ip or dns



SynchronizedApplication espApp;
JsonConfig esp3d_config("esp3d.json");


lv_obj_t *esp_app_main_tile = NULL;
lv_style_t esp_app_main_style;

lv_task_t * _esp_app_task;


lv_obj_t *returnDataObj;
lv_obj_t *heatGauge;

//#define MAX_STRING_SIZE len(returnData)
char testChar[50];

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(play_32px);
LV_IMG_DECLARE(esp3d_64px);
LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_32px);

static void exit_esp_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_esp_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void esp_app_task( lv_task_t * task );
void first_button_press_cb( lv_obj_t * obj, lv_event_t event );
void second_button_press_cb( lv_obj_t * obj, lv_event_t event );
void gauge_press_cb( lv_obj_t * obj, lv_event_t event );



WiFiClient client;
//Label ;
String command = "M105\n"; 
String returnData;

//void esp_app_setup( void ){
     // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    //espApp.init("esp3d", &esp3d_64px, true, 1, 1);

    /*
    // Executed when user click "refresh" button or when a WiFi connection is established
    espApp.synchronizeActionHandler([](SyncRequestSource source) {
        auto result = fetch_fx_rates();
        lblUpdatedAt.text(updatedAt);
        if (result)
        {
            fxratesApp.icon().widgetText(mainPairValue);
            lblCurrency1.text(mainPairValue).alignInParentCenter(0, -30);
            lblCurrency2.text(secondPairValue).alignOutsideBottomMid(lblCurrency1);
            fxratesApp.icon().showIndicator(ICON_INDICATOR_OK);
        } else {
            // In case of fail
            espApp.icon().showIndicator(ICON_INDICATOR_FAIL);
        }
    });
    */
//}

void build_more_esp_settings(){
    esp3d_config.addString("M105\n", 5 ).assign(&command);
    //esp3d_config.addString("192.168.1.215", 32).assign(&host);
    //espApp.useConfig(esp3d_config, false);
    esp3d_config.addBoolean("autosync", false);
    esp3d_config.addBoolean("widget", false);
/*
    // Switch desktop widget state based on the cuurent settings when changed
   esp3d_config.onLoadSaveHandler([](JsonConfig& cfg) {
        bool widgetEnabled = cfg.getBoolean("widget"); // Is app widget enabled?
        if (widgetEnabled)
            espApp.icon().registerDesktopWidget("esp3d", &esp3d_64px);
        else
            espApp.icon().unregisterDesktopWidget();
    });*/

    espApp.useConfig(esp3d_config, true);
}
    

/*bool esp3d_wifictl_event_cb(EventBits_t event, void *arg) {
    switch(event) {
        case WIFICTL_CONNECT:
            espApp.icon().hideIndicator();
            if ( config.getBoolean("autosync", false ) )
                espApp.startSynchronization(SyncRequestSource::ConnectionEvent);
            break;

        case WIFICTL_OFF:
            espApp.icon().hideIndicator();
            break;
    }
    return true;
}

*/



void build_buttons(){
    lv_obj_t * exit_btn = lv_imgbtn_create( esp_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &esp_app_main_style );
    lv_obj_align(exit_btn, esp_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_esp_app_main_event_cb );


/*//setup button

    lv_obj_t * setup_btn = lv_imgbtn_create( esp_app_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &esp_app_main_style );
    lv_obj_align(setup_btn, esp_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_esp_app_setup_event_cb );

*/


    lv_obj_t * first_button = lv_imgbtn_create( esp_app_main_tile, NULL);
    lv_imgbtn_set_src(first_button, LV_BTN_STATE_RELEASED, &play_32px);
    lv_imgbtn_set_src(first_button, LV_BTN_STATE_PRESSED, &play_32px);
    lv_imgbtn_set_src(first_button, LV_BTN_STATE_CHECKED_RELEASED, &play_32px);
    lv_imgbtn_set_src(first_button, LV_BTN_STATE_CHECKED_PRESSED, &play_32px);
    lv_obj_add_style(first_button, LV_IMGBTN_PART_MAIN, &esp_app_main_style );
    lv_obj_align(first_button, esp_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( first_button, first_button_press_cb );

    lv_obj_t * second_button = lv_imgbtn_create( esp_app_main_tile, NULL);
    lv_imgbtn_set_src(second_button, LV_BTN_STATE_RELEASED, &play_32px);
    lv_imgbtn_set_src(second_button, LV_BTN_STATE_PRESSED, &play_32px);
    lv_imgbtn_set_src(second_button, LV_BTN_STATE_CHECKED_RELEASED, &play_32px);
    lv_imgbtn_set_src(second_button, LV_BTN_STATE_CHECKED_PRESSED, &play_32px);
    lv_obj_add_style(second_button, LV_IMGBTN_PART_MAIN, &esp_app_main_style );
    lv_obj_align(second_button, esp_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_event_cb( second_button, second_button_press_cb );
}
void build_gauge(){

    static lv_style_t gaugeStyle;
    lv_style_init(&gaugeStyle);

    static lv_color_t needle_colors[2];
    needle_colors[0] = LV_COLOR_BLUE;
    needle_colors[1] = LV_COLOR_ORANGE;
    heatGauge = lv_gauge_create(esp_app_main_tile, NULL);
    lv_gauge_set_needle_count(heatGauge, 2, needle_colors);
    lv_obj_set_size(heatGauge, 200, 200);
    lv_obj_align(heatGauge, esp_app_main_tile, LV_ALIGN_CENTER, 120, 20);
    lv_gauge_set_range(heatGauge, 0, 260);
    lv_gauge_set_critical_value(heatGauge, 240);
    
   
    lv_gauge_set_scale(heatGauge, 180, 0, 4);
    lv_gauge_set_angle_offset(heatGauge, 270);
    lv_obj_set_style_local_scale_end_color(heatGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_scale_end_color(heatGauge, LV_GAUGE_PART_MAJOR, LV_STATE_DEFAULT, LV_COLOR_RED);
  
   // lv_style_set_scale_end_color(heatGauge, LV_STATE_DEFAULT , LV_COLOR_RED);

    lv_style_set_bg_opa(&gaugeStyle, LV_STATE_DEFAULT, LV_OPA_50);
    lv_obj_add_style(heatGauge, LV_GAUGE_PART_MAIN, &gaugeStyle);

    lv_gauge_set_value(heatGauge, 0, 0);
    lv_gauge_set_value(heatGauge, 1, 0);
    lv_obj_set_event_cb( heatGauge, gauge_press_cb );
    
   }
void esp_app_main_setup( uint32_t tile_num ) {

   
    esp_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &esp_app_main_style, mainbar_get_style() );

    build_gauge();
    build_buttons(); //for collapsablility

    lv_obj_move_background(heatGauge);
    //lv_gauge_set_needle_img(heatGauge, &img_hand, 4, 4);

//    build_more_esp_settings(); //still have some redundancy to reduce

    returnDataObj = lv_label_create(esp_app_main_tile, NULL);
    lv_label_set_long_mode(returnDataObj, LV_LABEL_LONG_SROLL_CIRC);     //Circular scroll
    lv_obj_set_width(returnDataObj, 150);
    lv_label_set_text(returnDataObj, "start");
    lv_obj_align(returnDataObj, esp_app_main_tile, LV_ALIGN_CENTER, 0, 30);


    lv_style_set_text_opa( &esp_app_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &esp_app_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_t *app_label = lv_label_create( esp_app_main_tile, NULL);
    //lv_label_set_text( app_label, "");
    lv_label_set_text( app_label,"esp3d hub");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &esp_app_main_style );
    lv_obj_align( app_label, esp_app_main_tile, LV_ALIGN_IN_TOP_MID, 0, 0);
/*
    */

   
    // create an task that runs every so often, REGARDLESS WHAT WATCH SCREEN IS ACTIVE
    //_esp_app_task = lv_task_create( esp_app_task, 3000, LV_TASK_PRIO_MID, NULL );

    //wifictl_register_cb(WIFICTL_CONNECT | WIFICTL_OFF, fxrates_wifictl_event_cb, "fxrates app widget");


}

static void enter_esp_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( esp_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_esp_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}


void updateGauge(){
     Serial.print("Connecting to ");
    Serial.println(host);
    
    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        lv_label_set_text(returnDataObj, "connection failed");
        return;
    }

    client.println("M105\n");

    int maxloops = 0;

    while (!client.available() && maxloops < 1000)
    {
        maxloops++;
        delay(1); //delay 1 msec
    }
    if (client.available() > 0){


        //read back from the server
        returnData = client.readStringUntil('\r');
        //returnData = client.readString();
        Serial.println(returnData);
        String extrTempStr = returnData.substring(5 , 7);
        String bedTempStr = returnData.substring(19, 21);

        Serial.println(extrTempStr);
        Serial.println(bedTempStr);

        
        int extrTemp = extrTempStr.toInt();
        int bedTemp = bedTempStr.toInt();


        lv_gauge_set_value(heatGauge, 0, extrTemp);
        lv_gauge_set_value(heatGauge, 1, bedTemp);
    }
    else
    {
        Serial.println("client.available() timed out ");
        lv_label_set_text(returnDataObj, "client timed out" );

    }

    Serial.println("Closing connection.");
    client.stop();

}

void sendLcdCmd(){
    Serial.print("Connecting to ");
    Serial.println(host);

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        lv_label_set_text(returnDataObj, "connection failed");
        return;
    }

    client.print("M117 hello world\n");

    //client.println("");

    Serial.println("Closing connection.");
    client.stop();
}
void sendGcode(){
    Serial.print("Connecting to ");
    Serial.println(host);


    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        lv_label_set_text(returnDataObj, "connection failed");
        return;
    }

    client.println(command);

    int maxloops = 0;

    while (!client.available() && maxloops < 1000)
    {
        maxloops++;
        delay(1); //delay 1 msec
    }
    if (client.available() > 0){


        //read back from the server
        returnData = client.readStringUntil('\r');
        //returnData = client.readString();
        Serial.println(returnData);
        //strncpy(testChar, returnData.c_str(), strlen(returnData));
        lv_label_set_text(returnDataObj, returnData.c_str());

        //snprintf(testChar, returnData.length, "%s", returnData.c_str());

    }
    else
    {
        Serial.println("client.available() timed out ");
        lv_label_set_text(returnDataObj, "client timed out" );

    }

    Serial.println("Closing connection.");
    client.stop();
}

void first_button_press_cb( lv_obj_t * obj, lv_event_t event ){
    switch ( event ){
                case( LV_EVENT_CLICKED ):
                    //sendLcdCmd();
                    sendGcode();
                break;
    }

}

void second_button_press_cb( lv_obj_t * obj, lv_event_t event ){
    switch ( event ){
                case( LV_EVENT_CLICKED ):
                    Serial.println("second button clicked");
                    lv_label_set_text(returnDataObj, "second button clicked");
                    sendLcdCmd();
                break;
    }
}

void gauge_press_cb( lv_obj_t * obj, lv_event_t event ){
    switch ( event ){
                case( LV_EVENT_CLICKED ):
                    Serial.println("gauge clicked, updating temp gauge");
                    lv_label_set_text(returnDataObj, "gauge clicked");
                    updateGauge();
                break;
    }
}

void esp_app_task( lv_task_t * task ) {
    // put your code here
   }

   




