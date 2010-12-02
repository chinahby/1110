static void disp_ic_bitblt(void *src_ptr, dword copy_count)
{
    register uint16 *pdata = src_ptr;
    register uint16  data;
    dword   mod_count;
    uint16 *pend;
    
    mod_count   = copy_count&0x07;
    copy_count -= mod_count;
    pend  = pdata + copy_count;
    
    while(pdata<pend)
    {
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
    }
    
    while(mod_count--)
    {
        data = (uint16)(*pdata++);
        LCD_WRITE_DATA16(data);
    }
}

static void disp_ic_set(uint32 src, dword copy_count)
{
    dword   mod_count;
    
    mod_count   = copy_count&0x07;
    copy_count  = copy_count>>3;
    
    while(copy_count--)
    {
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
        LCD_WRITE_DATA16(src);
    }
    
    while(mod_count--)
    {
        LCD_WRITE_DATA16(src);
    }
}

