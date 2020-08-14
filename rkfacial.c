/*
 * Copyright (C) 2019 Rockchip Electronics Co., Ltd.
 * author: Zhihua Wang, hogan.wang@rock-chips.com
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL), available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "face_common.h"
#include "rockface_control.h"
#include "load_feature.h"
#include "play_wav.h"
#include "camrgb_control.h"
#include "camir_control.h"
#include "video_common.h"
#include "usb_camera.h"
#include "db_monitor.h"
#include "rkfacial.h"

static int check_isp_server_status(int cam_id)
{
    int fd;
    char status[32];
    char pipe_name[128];
    sprintf(pipe_name, "/tmp/.ispserver_cam%d", cam_id);

    fd = open(pipe_name, O_RDONLY);
    if (fd < 0)
        return 0;
    memset(status, 0, sizeof(status));
    read(fd, status, sizeof(status) - 1);
    close(fd);
    return atoi(status);
}

int rkfacial_init(void)
{
    register_get_path_feature(rockface_control_get_path_feature);

#ifdef CAMERA_ENGINE_RKAIQ
    while (check_isp_server_status(0) != 1) {
        printf("%s: check isp server status\n", __func__);
        usleep(10000);
    }
    while (check_isp_server_status(1) != 1) {
        printf("%s: check isp server status\n", __func__);
        usleep(10000);
    }
#endif

    camrgb_control_init();

    camir_control_init();

    usb_camera_init();

    play_wav_thread_init();
    play_wav_signal(WELCOME_WAV);

    rockface_control_init_thread();

    db_monitor_init();

    return 0;
}

void rkfacial_exit(void)
{
    camrgb_control_exit();

    camir_control_exit();

    usb_camera_exit();

    rockface_control_exit();

    play_wav_thread_exit();
}

void rkfacial_delete(void)
{
    rockface_control_set_delete();
}

void rkfacial_register(void)
{
    rockface_control_set_register();
}

rkfacial_paint_box_callback rkfacial_paint_box_cb = NULL;
void register_rkfacial_paint_box(rkfacial_paint_box_callback cb)
{
    rkfacial_paint_box_cb = cb;
}

rkfacial_paint_info_callback rkfacial_paint_info_cb = NULL;
void register_rkfacial_paint_info(rkfacial_paint_info_callback cb)
{
    rkfacial_paint_info_cb = cb;
}

rkfacial_paint_face_callback rkfacial_paint_face_cb = NULL;
void register_rkfacial_paint_face(rkfacial_paint_face_callback cb)
{
    rkfacial_paint_face_cb = cb;
}

get_face_config_region_callback get_face_config_region_cb = NULL;
void register_get_face_config_region(get_face_config_region_callback cb)
{
    get_face_config_region_cb = cb;
}
