/*
 * Copyright (c) 2016 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/**
 * file : pas_fuse_diag_mode.c
 * brief: pas daemon interface layer to display_led SDI API
 * date : 04/2015
 *
 */

#include "private/pas_fuse_handlers.h"


/*
 * PAS Daemon diag_mode read interface, returns error code on failure and number of bytes read on success
 */
int dn_pas_fuse_diag_mode_read(
        dev_node_t *node, 
        char       *buf, 
        size_t     size, 
        off_t      offset
        )
{
    int    res                               = -ENOTSUP;
    size_t len                               = 0;
    char   trans_buf[FUSE_FILE_DEFAULT_SIZE] = { 0 };
    memset(buf, 0, size);

    /** check for node & buffer validity */
    if ((NULL == node) || (NULL == buf)) {
        
        return res;
    }

    /** switch on filetype */
    switch (node->fuse_filetype) {

        /** diagnostic mode */
        case FUSE_DIAG_MODE_FILETYPE:
            {

                dn_pas_fuse_print(trans_buf, FUSE_FILE_DEFAULT_SIZE, &len, &res, 
                        "%-25s : %s", "Diagnostic Mode", (dn_pald_diag_mode_get() ? "up" : "down"));
                break;
            }

        default:
            {    
                res = -ENOENT;
                len = 0;
                break;
            }
    }

    if (offset < len) {

        size = dn_pas_fuse_calc_size(len, size, offset);
        memcpy(buf, &trans_buf[offset], size);
        res = size;
    }

    return res;
}


/*
 * PAS Daemon led write interface
 */
int dn_pas_fuse_diag_mode_write(
        dev_node_t *node, 
        const char *buf, 
        size_t     size, 
        off_t      offset
        )
{
    int    res   = -ENOTSUP;
    uint_t state = 0;


    /** check for node & buffer validity */
    if ((NULL == node) || (NULL == buf)) {

        return res;
    }

    /** switch on filetype */
    switch (node->fuse_filetype) {

        /** diagnostic mode */
        case FUSE_DIAG_MODE_FILETYPE:
            {
                if (dn_pas_fuse_atoui(buf, &state)) {

                    switch(state) {
                        case 1  :                    
                            {       
                                dn_pas_lock();
                                dn_pald_diag_mode_set(true); 
                                dn_pas_unlock();
                                res = size;
                                break;
                            }
            
                        case 0  :  
                            { 
                                dn_pas_lock();
                                dn_pald_reinit();
                                dn_pald_diag_mode_set(false);
                                dn_pas_unlock();
                                res = size;
                                break;
                            }

                        default : 
                            {
                                res = -EINVAL;
                                  break;
                            }
                    }

                } else {
                    
                    res = -EINVAL;
                }
                break;
            }

        default:
            {
                res = -ENOENT;
                break;
            }
    }

    return res;
}
