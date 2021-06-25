#include "config.h"
#include <TTGO.h>
#include "quickglui/quickglui.h"

#include "bleKeyRem.h"
#include "gui/mainbar/mainbar.h"
#include "hardware/wifictl.h"

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(bleKey_64px);
LV_FONT_DECLARE(Ubuntu_48px);

static SynchronizedApplication bleKeyRemApp;
static JsonConfig config("bleKeyRem.json");

static String apiKey, mainPair, secondPair;
static String mainPairValue, secondPairValue, updatedAt;
static Label lblCurrency1, lblCurrency2, lblUpdatedAt;

static Style fig;

/*
 * setup routine for application
 */
void bleKeyRem_app_setup() {
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    bleKeyRemApp.init("BT Remote", &bleKey_64px, false, 1, 1);
    
    // Build and configure application
    build_btRem_page();
    build_btRem_settings();

    
}

void build_btRem_page()
{
   // fig = Style::Create(mainbar_get_style(), true);
    fig.textFont(&Ubuntu_48px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = bleKeyRemApp.mainPage(); // This is parent for all main screen widgets

    lblCurrency1 = Label(&screen);
    lblCurrency1.text(mainPair)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(fig, true)
        .alignInParentCenter(0, -30);

    lblCurrency2 = Label(&screen);
    lblCurrency2.text(secondPairValue)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(fig, true)
        .alignOutsideBottomMid(lblCurrency1);

    lblUpdatedAt = Label(&screen);
    lblUpdatedAt.text("loading...")
        .alignText(LV_LABEL_ALIGN_LEFT)
        .alignInParentTopLeft(5, 5);
}

void build_btRem_settings()
{
    // Create full options list and attach items to variables
    config.addString("apikey", 32).assign(&apiKey);
    config.addString("pair1", 12, "EUR_USD").assign(&mainPair);
    config.addString("pair2", 12).assign(&secondPair);
    config.addBoolean("autosync", false);
    config.addBoolean("widget", false);

    // Switch desktop widget state based on the cuurent settings when changed
    config.onLoadSaveHandler([](JsonConfig& cfg) {
        bool widgetEnabled = cfg.getBoolean("widget"); // Is app widget enabled?
        if (widgetEnabled)
            bleKeyRemApp.icon().registerDesktopWidget("btR", &bleKey_64px);
        else
            bleKeyRemApp.icon().unregisterDesktopWidget();
    });

    bleKeyRemApp.useConfig(config, true); // true - auto create settings page widgets
}

