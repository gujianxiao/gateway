/**
 * Portions Copyright (C) 2013 Regents of the University of California.
 * 
 * Based on the CCNx C Library by PARC.
 * Copyright (C) 2009,2010 Palo Alto Research Center, Inc.
 *
 * This work is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 * This work is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details. You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * This module replaces ndnd_main on the android platform.  It includes the
 * methods a JNI interface would use to start ndnd.
 */

#include <stdarg.h>
#include <android/log.h>
#include <ndnd_private.h>

static int
logger(void *loggerdata, const char *format, va_list ap)
{
    __android_log_vprint(ANDROID_LOG_INFO, "NDND", format, ap);
}

int
start_ndnd(void)
{
    struct ndnd_handle *h = NULL;
    
    h = ndnd_create("ndnd", &logger, NULL);
    ndnd_msg(h, "ndnd_create h=%p", h);
    ndnd_run(h);
    ndnd_msg(h, "exiting.");
}
