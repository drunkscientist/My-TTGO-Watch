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
#include <HTTPClient.h>
#include <SPIFFS.h>

#include "uri_load.h"
#include "utils/alloc.h"

uri_load_dsc_t *uri_load_create_dsc( void );
void uri_load_set_filename_from_uri( uri_load_dsc_t *uri_load_dsc, const char *uri );
void uri_load_set_url_from_uri( uri_load_dsc_t *uri_load_dsc, const char *uri );
uri_load_dsc_t *uri_load_file_to_ram( uri_load_dsc_t *uri_load_dsc );
uri_load_dsc_t *uri_load_http_to_ram( uri_load_dsc_t *uri_load_dsc );

uri_load_dsc_t *uri_load_to_ram( const char *uri ) {
    /**
     * alloc uri_load_dsc structure
     */
    uri_load_dsc_t *uri_load_dsc = uri_load_create_dsc();
    /**
     * 
     */
    if ( uri_load_dsc ) {
        log_d("uri_load_dsc: alloc %d bytes at %p", sizeof( uri_load_dsc_t ), uri_load_dsc );
        /**
         * set download timestamp
         */
        uri_load_dsc->timestamp = millis();
        /**
         * set filename in the uri_load_dsc
         */
        uri_load_set_filename_from_uri( uri_load_dsc, uri );
        /**
         * set filename in the uri_load_dsc
         */
        uri_load_set_url_from_uri( uri_load_dsc, uri );
        /**
         * check for uri file source
         */
        if ( strstr( uri, "http://" ) || strstr( uri, "https://" ) ) {
            log_d("http/s source");
            uri_load_dsc = uri_load_http_to_ram( uri_load_dsc );
        }
        else if ( strstr( uri, "file://" ) ) {
            log_d("local files source");
            uri_load_dsc = uri_load_file_to_ram( uri_load_dsc );
        }
        else {
            log_e("uri not supported");
            uri_load_free_all( uri_load_dsc );
            uri_load_dsc = NULL;
        }
    }
    else {
        log_e("uri_load_dsc: alloc failed");
    }
    return( uri_load_dsc );
}

uri_load_dsc_t *uri_load_http_to_ram( uri_load_dsc_t *uri_load_dsc ) {
    /**
     * check if alloc was failed
     */
    if ( uri_load_dsc ) {
        log_i("load file from: %s", uri_load_dsc->uri );
        /**
         * open http connection
         */
        HTTPClient download_client;                     /** @brief http download client */
        download_client.begin( uri_load_dsc->uri );
        int httpCode = download_client.GET();
        /**
         * request successfull?
         */
        if ( httpCode > 0 && httpCode == HTTP_CODE_OK  ) {
            /**
             * get file size and alloc memory for the file
             */
            uri_load_dsc->size = download_client.getSize();
            uri_load_dsc->data = (uint8_t*)MALLOC( uri_load_dsc->size );
            log_d("uri_load_dsc->data: alloc %d bytes at %p", uri_load_dsc->size, uri_load_dsc->data );
            /**
             * check if alloc success
             */
            if ( uri_load_dsc->data ) {
                /**
                 * setup data write counter/pointer/buffer and data stream
                 */
                uint32_t bytes_left = uri_load_dsc->size;                          /** @brief download left byte counter */
                uint8_t *data_write_p = uri_load_dsc->data;                        /** @brief write pointer for the raw file download */

                WiFiClient *download_stream = download_client.getStreamPtr();
                /**
                 * get download data
                 */
                while( download_client.connected() && ( bytes_left > 0 ) ) {
                    /**
                     * get bytes in buffer and store them
                     */
                    size_t size = download_stream->available();
                    if ( size > 0 ) {
                        size_t c = download_stream->readBytes( data_write_p, size < bytes_left ? size : bytes_left );
                        bytes_left -= c;
                        data_write_p = data_write_p + c;
                    }
                }
                if ( bytes_left != 0 ) {
                    log_e("download failed");
                    download_client.end();
                    uri_load_free_all( uri_load_dsc );
                    return( NULL );
                }
            }
            else {
                log_e("data alloc failed");
                download_client.end();
                uri_load_free_all( uri_load_dsc );
                return( NULL );
            }
        }
        else {
            log_e("http connection abort");
            download_client.end();
            uri_load_free_all( uri_load_dsc );
            return( NULL );
        }
        /**
         * close http connection
         */
        download_client.end();
    }
    else {
        log_e("uri_load_dsc: alloc failed");
    }
    return( uri_load_dsc );
}

uri_load_dsc_t *uri_load_file_to_ram( uri_load_dsc_t *uri_load_dsc ) {
    /**
     * check if alloc was failed
     */
    if ( uri_load_dsc ) {
        log_i("load file from: %s", uri_load_dsc->uri );
        /**
         * try to open file
         */
        const char *filepath = strstr( uri_load_dsc->uri, "://" ) + 3;
        log_d("open file from %s", filepath );
        FILE* file;
        file = fopen( filepath, "rb" );

        if ( file ) {
            log_i("file open success");
            /**
             * get file len
             */
            fseek( file, 0, SEEK_END );
            uri_load_dsc->size = ftell( file );
            fseek( file, 0, SEEK_SET );
            /**
             * alloc data mamory
             */
            uri_load_dsc->data = (uint8_t*)MALLOC( uri_load_dsc->size );
            if( uri_load_dsc->data ) {
                fread( uri_load_dsc->data, uri_load_dsc->size, 1, file );
                uri_load_dsc->timestamp = millis();
            }
            else {
                log_e("uri_load_dsc->data: alloc failed");
                uri_load_free_all( uri_load_dsc );
                uri_load_dsc = NULL;
            }
        }
        else {
            log_e("file open failed");
            uri_load_free_all( uri_load_dsc );
            uri_load_dsc = NULL;
        }
        fclose( file );
    }
    else {
        log_e("uri_load_dsc: alloc failed");
    }
    return( uri_load_dsc );
}

uri_load_dsc_t *uri_load_create_dsc( void ) {
    uri_load_dsc_t *uri_load_dsc = (uri_load_dsc_t *)MALLOC( sizeof( uri_load_dsc_t) );
    /**
     * if alloc was success, init structure
     */
    if( uri_load_dsc ) {
        uri_load_dsc->data = NULL;
        uri_load_dsc->filename = NULL;
        uri_load_dsc->uri = NULL;
        uri_load_dsc->timestamp = millis();
        uri_load_dsc->size = 0;
    }
    return( uri_load_dsc );
}

void uri_load_free_all( uri_load_dsc_t *uri_load ) {
    if ( uri_load ) {
        /**
         * free name momory
         */
        uri_load_free_name( uri_load );
        /**
         * free url momory
         */
        uri_load_free_url( uri_load );
        /**
         * free data momory
         */
        uri_load_free_data( uri_load );
        /**
         * free uri_load_dsc memory
         */
        uri_load_free_dsc( uri_load );
    }
}

void uri_load_free_name( uri_load_dsc_t *uri_load ) {
    if ( uri_load->filename ) {
        log_d("free: %p", uri_load->filename );
        free( uri_load->filename );
        uri_load->filename = NULL;
    }
}

void uri_load_free_url( uri_load_dsc_t *uri_load ) {
    if ( uri_load->uri ) {
        log_d("free: %p", uri_load->url );
        free( uri_load->uri );
        uri_load->uri = NULL;
    }
}

void uri_load_free_data( uri_load_dsc_t *uri_load ) {
    if ( uri_load->data ) {
        log_d("free: %p", uri_load->data );
        free( uri_load->data );
        uri_load->data = NULL;
    }
}

void uri_load_free_dsc( uri_load_dsc_t *uri_load ) {
    if ( uri_load ) {
        /**
         * free uri_load_dsc memory
         */
        log_d("free: %p", uri_load );
        free( uri_load );
    }
}

void uri_load_free_without_data( uri_load_dsc_t *uri_load ) {
    if ( uri_load ) {
        /**
         * free name momory
         */
        uri_load_free_name( uri_load );
        /**
         * free url momory
         */
        uri_load_free_url( uri_load );
        /**
         * free uri_load_dsc memory
         */
        uri_load_free_dsc( uri_load );
    }
}

void uri_load_set_filename_from_uri( uri_load_dsc_t *uri_load_dsc, const char *uri ) {
    const char *filename_from_uri = NULL;
    const char *tld = NULL;
    const char index[] = "index.html";
    /**
     * check for http or https url
     */
    if ( uri_load_dsc ) {
        if ( strstr( uri, "http://" ) || strstr( uri, "https://" )) {
            tld = strstr( uri, "://" ) + 3;
            /**
             * reverse search for the lash
             */
            filename_from_uri = (const char*)strrchr( tld, '/' );
            /**
             * if not lash found, set index.html as filename
             */
            if ( !filename_from_uri ) {
                filename_from_uri = index;
            }
            /**
             * alloc filename momory
             */
            uri_load_dsc->filename = (char *)MALLOC( strlen( filename_from_uri ) + 2 );
            if ( uri_load_dsc->filename ) {
                /**
                 * copy filename into alloc mamory
                 */
                log_d("uri_load_dsc->filename: alloc %d bytes at %p", strlen( filename_from_uri ), uri_load_dsc->filename );
                strncpy( uri_load_dsc->filename, filename_from_uri, strlen( filename_from_uri ) + 2 );
            }
            else {
                log_e("uri_load_dsc->filename: alloc failed");
            }
        }
        else if ( strstr( uri, "file://" ) ) {
            filename_from_uri = strstr( uri, "://" ) + 3;
            /**
             * alloc filename momory
             */
            uri_load_dsc->filename = (char *)MALLOC( strlen( filename_from_uri ) + 2 );
            if ( uri_load_dsc->filename ) {
                /**
                 * copy filename into alloc mamory
                 */
                log_d("uri_load_dsc->filename: alloc %d bytes at %p", strlen( filename_from_uri ), uri_load_dsc->filename );
                strncpy( uri_load_dsc->filename, filename_from_uri, strlen( filename_from_uri ) + 2 );
            }
            else {
                log_e("uri_load_dsc->filename: alloc failed");
            }
        }
    }
}

void uri_load_set_url_from_uri( uri_load_dsc_t *uri_load_dsc, const char *uri ) {
    /**
     * check for http or https url
     */
    if ( uri_load_dsc ) {
        /**
         * alloc url momory
         */
        uri_load_dsc->uri = (char *)MALLOC( strlen( uri ) + 2 );
        if ( uri_load_dsc->uri ) {
            /**
             * copy url into alloc mamory
             */
            log_d("uri_load_dsc->uri: alloc %d bytes at %p", strlen( uri ), uri_load_dsc->uri );
            strncpy( uri_load_dsc->uri, uri, strlen( uri )  + 2 );
        }
        else {
            log_e("uri_load->uri: alloc failed");
        }
    }
}