#ifndef NDNPOKE_H
#define NDNPOKE_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ndn/ndn.h>
#include <ndn/uri.h>
#include <ndn/keystore.h>
#include <ndn/signing.h>

#include "define.h"


int pack_data_content(char* name, char *content)
{
    DEBUG printf("start to pack the data content!\n");
    const char *progname = name;
    struct ndn *ndn = NULL;
    char *uri_name=NULL;
    char *raw_content=NULL;
    struct ndn_charbuf *ndn_name = NULL;
    struct ndn_charbuf *pname = NULL;
    struct ndn_charbuf *temp = NULL;
    int res;
    ssize_t content_length;
    long expire = -1;
    enum ndn_content_type content_type = NDN_CONTENT_DATA;
    int force = 0;
    int prefixcomps = -1;
    struct ndn_signing_params sp = NDN_SIGNING_PARAMS_INIT;
    
    uri_name = (char*)malloc(sizeof(char)*1024);
    raw_content = (char*)malloc(sizeof(char)*1024);
    memset(uri_name, 0, 1024);
    memset(raw_content, 0, 1024);
    strcpy(uri_name, name);
    strcpy(raw_content, content);

    force = 1;
    //pContent = content;
    expire = 5;
    //uri_name = name;

    DEBUG printf("the content=%s\n", raw_content);
    DEBUG printf("the name=%s\n", progname);

    ndn_name = ndn_charbuf_create();
    res = ndn_name_from_uri(ndn_name, uri_name);
    if (res < 0) {
        fprintf(stderr, "%s: bad ndn URI: %s\n", progname, ndn_name->buf);
        return 1;
    }

    /* Preserve the original prefix, in case we add versioning,
     * but trim it down if requested for the interest filter registration
     */
    pname = ndn_charbuf_create();
    ndn_charbuf_append(pname, ndn_name->buf, ndn_name->length);
    if (prefixcomps >= 0) {
        res = ndn_name_chop(pname, NULL, prefixcomps);
        if (res < 0) {
            fprintf(stderr, "%s: unable to trim name to %d component%s.\n",
                    progname, prefixcomps, prefixcomps == 1 ? "" : "s");
            return 1;
        }
    }
    /* Connect to ndnd */
    ndn = ndn_create();
    if (ndn_connect(ndn, NULL) == -1) {
        perror("Could not connect to ndnd");
        return 1;
    }
 
    temp = ndn_charbuf_create();
    
    /* Set content type */
    sp.type = content_type;
    
    /* Set freshness */
    if (expire >= 0) {
        if (sp.template_ndnb == NULL) {
            sp.template_ndnb = ndn_charbuf_create();
            ndnb_element_begin(sp.template_ndnb, NDN_DTAG_SignedInfo);
        }
        else if (sp.template_ndnb->length > 0) {
            sp.template_ndnb->length--;
        }
        ndnb_tagged_putf(sp.template_ndnb, NDN_DTAG_FreshnessSeconds, "%ld", expire);
        sp.sp_flags |= NDN_SP_TEMPL_FRESHNESS;
        ndnb_element_end(sp.template_ndnb);
    }

    /* Create the signed content object, ready to go */
    temp->length = 0;
    content_length =strlen(raw_content);
    res = ndn_sign_content(ndn, temp, ndn_name, &sp, raw_content, content_length);
    if (res != 0) {
        fprintf(stderr, "Failed to encode ContentObject (res == %d)\n", res);
        return 1;
    }
    if (force) {
        /* At user request, send without waiting to see an interest */
        DEBUG printf("get ready to ndn_put!!~~\n");
        res = ndn_put(ndn, temp->buf, temp->length);
        if (res < 0) {
            fprintf(stderr, "ndn_put failed (res == %d)\n", res);
            return 1;
        }
    }
   
    free(uri_name);
    free(raw_content);
    ndn_destroy(&ndn);
    ndn_charbuf_destroy(&ndn_name);
    ndn_charbuf_destroy(&pname);
    ndn_charbuf_destroy(&temp);
    ndn_charbuf_destroy(&sp.template_ndnb);
    return 0;
}

#endif