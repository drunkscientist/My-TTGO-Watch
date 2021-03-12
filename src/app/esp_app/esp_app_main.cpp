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


//SynchronizedApplication esp3dApp;
JsonConfig esp3d_config("esp3d.json");


lv_obj_t *esp_app_main_tile = NULL;
lv_style_t esp_app_main_style;

lv_task_t * _esp_app_task;

lv_obj_t *returnDataObj;
lv_obj_t *lblReturnData;
//#define MAX_STRING_SIZE len(returnData)
char testChar[50];

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(play_32px);
LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_32px);

static void exit_esp_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_esp_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void esp_app_task( lv_task_t * task );
void first_button_press_cb( lv_obj_t * obj, lv_event_t event );
void second_button_press_cb( lv_obj_t * obj, lv_event_t event );


const uint16_t port = 8888;
const char * host = "192.168.1.215"; // ip or dns

WiFiClient client;
//Label ;
String command, returnData;


void esp_app_main_setup( uint32_t tile_num ) {

    esp_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &esp_app_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( esp_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &esp_app_main_style );
    lv_obj_align(exit_btn, esp_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_esp_app_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( esp_app_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &esp_app_main_style );
    lv_obj_align(setup_btn, esp_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_esp_app_setup_event_cb );

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



    

    //lv_obj_t * lblReturnData = lv_label_create(esp_app_main_tile, NULL);

    //lv_label_set_text_fmt

/*
*/


    // uncomment the following block of code to remove the "myapp" label in background
    
    lv_style_set_text_opa( &esp_app_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &esp_app_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_t *app_label = lv_label_create( esp_app_main_tile, NULL);
    //lv_label_set_text( app_label, "");
    lv_label_set_text( app_label,"babysteps");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &esp_app_main_style );
    lv_obj_align( app_label, esp_app_main_tile, LV_ALIGN_CENTER, 0, 0);
/*
    */

    //lblReturnData.text(returnData).alignInParentCenter();
    // create an task that runs every secound
    _esp_app_task = lv_task_create( esp_app_task, 1000, LV_TASK_PRIO_MID, NULL );


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
void sendLcdCmd(){
    Serial.print("Connecting to ");
    Serial.println(host);


    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        return;
    }
    
// client.connect( host, port);

    client.print("M117 doit ASSHOLE\n");

    //client.println("");

    Serial.println("Closing connection.");
    client.stop();
}
void sendGcode(){
    Serial.print("Connecting to ");
    Serial.println(host);


    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        return;
    }
    
    client.print("M105\n");

    int maxloops = 0;

    while (!client.available() && maxloops < 1000)
    {
        maxloops++;
        delay(1); //delay 1 msec
    }
    if (client.available() > 0){

        
        //read back one line from the server
        returnData = client.readStringUntil('\r');
        Serial.println(returnData);        
        //lv_label_set_text(lblReturnData, returnData);

        strncpy(testChar, returnData.c_str(), sizeof(returnData));
        //snprintf(testChar, returnData.length, "%s", returnData.c_str());
        //lv_label_set_text(returnDataObj, testChar);
        //lv_event_send_refresh(returnDataObj);
    
    }
    else
    {
        Serial.println("client.available() timed out ");
        
        
        //testChar = "client timed out";
        //lv_label_set_text(returnDataObj, "client timed out");
        //lv_event_send_refresh(returnDataObj);
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
                    
                break;
    }
}

void esp_app_task( lv_task_t * task ) {
    // put your code here

    //lv_event_send_refresh_recursive(esp_app_main_tile);

    lv_obj_t * returnDataObj = lv_label_create(esp_app_main_tile, NULL);
    lv_label_set_long_mode(returnDataObj, LV_LABEL_LONG_SROLL_CIRC);     //Circular scroll
    lv_obj_set_width(returnDataObj, 150);
    lv_label_set_text(returnDataObj, testChar);
    lv_obj_align(returnDataObj, esp_app_main_tile, LV_ALIGN_CENTER, 0, 30);
   
    
}