/*
 * Copyright (c) 2006-2015 David Schor (david@zigwap.com), ZigWap LLC
 * 
 * License: MIT
 */
a_str a_new_size_raw(size_t l)
{
    struct a_header *h;
    size_t size;
    
    for (++l, size = A_MIN_STR_SIZE; size < l;)
        size <<= 1;
    
    if (!(h = A_MALLOC(sizeof *h + size))) /* TODO l+1 overflow situation */
        return NULL;
    
    h->mem = size;
    return a_buff(h);
}

/* creates a new Aleph string with enough space to hold l bytes. */
a_str a_new_size(size_t l)
{
    a_str s;
    struct a_header *h;
    
    if ((s = a_new_size_raw(l)))
    {
        h = a_header(s);
        *s = '\0';
        h->len = 0;
        h->size = 0;
    }
    return s;
}


a_str a_new(const char *s)
{
    A_ASSERT_UTF8(s ? s : "");
    return a_new_len(s ? s : "", s ? strlen(s) : 0);
}

a_str a_new_len(const char *s, size_t l)
{
    a_str str;
    struct a_header *h;
    assert(s != NULL);
    PASSTHROUGH_ON_FAIL(s != NULL, NULL);
    
    if ((str = a_new_size_raw(l+1)))
    {
        h = a_header(str);
        memcpy(str, s, l);
        str[l] = '\0';
        h->len = a_len_cstr(s);
        h->size = l;
    }
    return str;
}

a_str a_new_dup(a_cstr s)
{
    a_str dup;
    assert(s != NULL);
    PASSTHROUGH_ON_FAIL(s != NULL, NULL);
    
    if ((dup = a_new_size_raw(a_size(s) + 1)))
        memcpy(a_header(dup), a_header(s), sizeof (struct a_header) + a_size(s) + 1);
    return dup;
}


static a_str a_new_chr_internal(const char *chr, size_t size, size_t repeat)
{
    size_t l;
    a_str str;
    struct a_header *h;
    
    l = size * repeat;
    if ((str = a_new_size_raw(l + 1)))
    {
        size_t i;
        
        for (i = 0; i < l; i += size)
            memcpy(str + i, chr, size);
        str[l] = '\0';
        
        h = a_header(str);
        h->len = repeat;
        h->size = l;
    }
    return str;
}

a_str a_new_cp(a_cp cp, size_t repeat)
{
    char b[7];
    int size;
    assert(cp != 0);
    A_ASSERT_CODEPOINT(cp);
    
    a_to_utf8_size(cp, b, &size);
    return a_new_chr_internal(b, size, repeat);
}
a_str a_new_chr(char *chr, size_t repeat)
{
    assert(*chr != 0);
    return a_new_chr_internal(chr, a_size_chr_cstr(chr), repeat);
}