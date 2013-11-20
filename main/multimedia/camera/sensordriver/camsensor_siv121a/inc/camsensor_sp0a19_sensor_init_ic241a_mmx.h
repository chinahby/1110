LOCAL boolean camsensor_sp0a19_sensor_init(void)
{
		MSG_FATAL("-----camsensor_SP0A19_sensor_init-----enter",0,0,0);

      camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1C,0x28);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0f,0x2f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x10,0x2e);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x11,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x12,0x18);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x13,0x2f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x14,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x15,0x3f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x16,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x17,0x18);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x25,0x40);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1a,0x0b);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1b,0xc );
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1e,0xb );
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x20,0x3f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x21,0x13);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x22,0x19);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x26,0x1a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x27,0xab);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xfd);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x30,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfb,0x33); //0x31 PZT 2013-5-22
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1f,0x08);     
  	//Blacklevel
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x65,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x66,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x67,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x68,0x00);//06
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x45,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x46,0x0f);

#if 1	
         /*//ae setting  24M 50hz 20-8FPS maxgain:0x70 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04,0x32);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0x46);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x66);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x0c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0x66);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0x00);
  	  
  	//Status
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce,0xc8);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf,0x04);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0,0xc8);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1,0x04);  */
  	  
 /* //ae setting  24M 50hz 18-6FPS maxgain:0x70 
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04,0x14);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0xc6);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x5c);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x10);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0x5c);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0x00);
  //Status                                      
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce,0xc0);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf,0x05);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0,0xc0);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1,0x05);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  */
  // //ae setting  24M 50hz fix 7FPS maxgain:0x70 
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04,0xa2);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06,0xff);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x05);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0x5c);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x36);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x0e);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0x36);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0x00);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0x00);
//Status                                           
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce,0xf4);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf,0x02);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0,0xf4);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1,0x02);
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);


#else
//;ae setting      SI13_SP0A19 24M 1¡¤??¦Ì 50Hz 20-20fps AE_Parameters_20130503155954                 
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x03 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x04 , 0x32);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x06 , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x09 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a , 0x46);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0 , 0x66);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf1 , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x90 , 0x05);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x92 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x98 , 0x66);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x99 , 0x00);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b , 0x00);                                               
//Status                                                                                            
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xce , 0xfe);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcf , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd0 , 0xfe);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd1 , 0x01);                                               
  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd , 0x00);                                               


#endif
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc4,0x56);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc5,0x8f);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xca,0x30);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcb,0x45);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcc,0x70);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xcd,0x48);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  
  	 //lsc  for st
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x35,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x36,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x37,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x38,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x39,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3a,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3b,0x13);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3c,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3d,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3e,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x3f,0x15);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x40,0x18);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x41,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x42,0x04);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x43,0x04);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x44,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x45,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x46,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x47,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x48,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x49,0xfd);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4a,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4b,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4c,0xfd);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);  
  	//awb 1
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x28,0xc5);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x29,0x9b);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2e,0x02);	
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2f,0x16);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x17,0x17);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x18,0x19);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x19,0x45);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2a,0xef);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x2b,0x15);  
  	  //awb2
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x73,0x80);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1a,0x80);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x1b,0x80); 
  	//d65
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x65,0xd5);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x66,0xfa);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x67,0x72);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x68,0x8a);
  	//indoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x69,0xc6);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x6a,0xee);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x6b,0x94);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x6c,0xab);
  	//f
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x61,0x7a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x62,0x98);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x63,0xc5);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x64,0xe6);
  	//cwf
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x6d,0xb9);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x6e,0xde);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x6f,0xb2);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x70,0xd5);
  	 
  	//////////////////skin detect//////
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x08,0x15);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x09,0x04);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0a,0x20);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0b,0x12);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0c,0x27);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0d,0x06);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x0f,0x63);  
  	//BPC_grad
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x79,0xf0);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x7a,0x80);  //f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x7b,0x80);   //f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x7c,0x20);     //f0
  	  
  	//smooth
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  		//¦Ì£¤¨ª¡§¦Ì¨¤???????D?¦Ì	
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x57,0x06);	//raw_dif_thr_outdoor         0x07 //modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x58,0x08); //raw_dif_thr_normal //0x0d   0x10
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x56,0x0d); //raw_dif_thr_dummy //0x10    0x12
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x59,0x15); //raw_dif_thr_lowlight //0x10 0x16
  		//GrGb?????D?¦Ì
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x89,0x06);	//raw_grgb_thr_outdoor         0x07 //modify by sp_yjp,20131120 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x8a,0x08); //raw_grgb_thr_normal   //0x0d 0x10                             
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9c,0x0d); //raw_grgb_thr_dummy    //0x10 0x12                             
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9d,0x15); //raw_grgb_thr_lowlight //0x10 0x16                             
  
  		//Gr\Gb???????????¨¨
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x81,0xb0);    //raw_gflt_fac_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x82,0x60); //80//raw_gflt_fac_normal //0xe0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x83,0x30);    //raw_gflt_fac_dummy //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x84,0x20);    //raw_gflt_fac_lowlight //0x40
  		//Gr?¡éGb¦Ì£¤¨ª¡§¦Ì¨¤?¨²???????¨¨  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x85,0xb0); //raw_gf_fac_outdoor  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x86,0x60); //raw_gf_fac_normal  //0xc0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x87,0x30); //raw_gf_fac_dummy    //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x88,0x20); //raw_gf_fac_lowlight //0x40
  		//R?¡éB???????¨¨  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5a,0xff);		 //raw_rb_fac_outdoor
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5b,0xb0); //40//raw_rb_fac_normal //0xe0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5c,0x60); 	 //raw_rb_fac_dummy //0x80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5d,0x20); 	 //raw_rb_fac_lowlight
  	  
  	//sharpen 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe2,0x30);	//sharpen_y_base
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe4,0xa0);	//sharpen_y_max
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe5,0x05); //rangek_neg_outdoor //0x04
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd3,0x05); //rangek_pos_outdoor   //0x04 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd7,0x05); //range_base_outdoor    //0x04
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe6,0x06); //rangek_neg_normal //0x04    0x0a  //modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd4,0x06); //rangek_pos_normal  //0x04   0x0a
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd8,0x06); //range_base_normal   //0x04  0x0a
  	    
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe7,0x0d); //rangek_neg_dummy //0x07     0x10	//modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd5,0x0d); //rangek_pos_dummy //0x08     0x10
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd9,0x0d); //range_base_dummy //0x08     0x10
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd2,0x12); //rangek_neg_lowlight
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd6,0x12); //rangek_pos_lowlight
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xda,0x12); //range_base_lowlight
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe8,0x28);//sharp_fac_pos_outdoor	//0x20 //modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xec,0x35);//sharp_fac_neg_outdoor
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe9,0x28);//sharp_fac_pos_nr				//0x20 //modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xed,0x35);//sharp_fac_neg_nr
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xea,0x20);//sharp_fac_pos_dummy
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xef,0x28);//sharp_fac_neg_dummy    //0x30 //modify by sp_yjp,20131120
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xeb,0x10);//sharp_fac_pos_low
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf0,0x20);//sharp_fac_neg_low 
  	  #if 1 //default PZT 2013-5-22
  	//CCM
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa0,0x66);//80(o¨¬¨¦??¨®?¨¹¡ê?¡¤?¨¦?2?¨¤¨ª??)
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa1,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa2,0x19);//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa3,0xf3);//f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa4,0x8e);//a6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa5,0x0 );//ea
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa6,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa7,0xe6);//e6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa8,0x9a);//9a
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa9,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xaa,0x3 );//33
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xab,0xc );//c 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  #else
	    	//CCM
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa0,0x80);//80(o¨¬¨¦??¨®?¨¹¡ê?¡¤?¨¦?2?¨¤¨ª??)
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa1,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa2,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa3,0xf4);//f0
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa4,0xa6);//a6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa5,0xe7);//ea
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa6,0xf4);//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa7,0xe7);//e6
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa8,0xa6);//9a
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xa9,0x0 );//0 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xaa,0x33);//33
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xab,0x0f);//c 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
         #endif
  	//gamma  
  	  
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8b,0x0 );//0 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8c,0xC );//11
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8d,0x19);//19 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8e,0x2C);//28 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8f,0x49);//46 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x90,0x61);//61 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x91,0x77);//78 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x92,0x8A);//8A 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x93,0x9B);//9B 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x94,0xA9);//A9 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x95,0xB5);//B5 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x96,0xC0);//C0 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x97,0xCA);//CA 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x98,0xD4);//D4 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x99,0xDD);//DD 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x9a,0xE6);//E6 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x9b,0xEF);//EF 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);//01 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8d,0xF7);//F7 
  	 camsensor_SP0A19_ycbcr_i2c_write_byte(0x8e,0xFF);//FF 
  	             
  	//rpc
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe0,0x4c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe1,0x3c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe2,0x34);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe3,0x2e);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe4,0x2e);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe5,0x2c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe6,0x2c);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe8,0x2a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xe9,0x2a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xea,0x2a);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xeb,0x28);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf5,0x28);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf6,0x28);
  	//ae min gain  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
	
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x94,0x90);//rpc_max_indr   //0xa0 //modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x95,0x28);//1e//rpc_min_indr 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9c,0x90);//rpc_max_outdr	//0xa0 //modify by sp_yjp,20131120    
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x9d,0x28);//rpc_min_outdr    
	#if 0  
  	//ae target
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xed,0x8c);//80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf7,0x88);//7c 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf8,0x80);//70 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xec,0x7c);//6c  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xef,0x74);//99
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xf9,0x70);//90
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfa,0x68);//80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xee,0x64);//78    
	 #else
	 //ae target modify 20130503
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xed,0x84);//80   //0x80   //modify by sp_yjp,20131120
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xf7,0x80);//7c   //0x7c
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xf8,0x74);//70   //0x70
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xec,0x70);//6c   //0x6c
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xef,0x78);//99   //0x74
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xf9,0x74);//90   //0x70
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xfa,0x6c);//80   //0x68
	   camsensor_SP0A19_ycbcr_i2c_write_byte(0xee,0x68);//78   //0x64

	 #endif
  	//gray detect
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x01);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x30,0x40);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x31,0x70);//
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x40);//80
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x33,0xef);//
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x34,0x05);//04
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4d,0x2f);//40
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4e,0x20);//
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x4f,0x16);//13      
  	  //lowlight lum
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00); 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb2,0x18);//lum_limit //0x20 PZT 2013-5-22
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb3,0x1f);//lum_set
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb4,0x30);//black_vt
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xb5,0x45);//white_vt
  	//saturation
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xbe,0xff);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xbf,0x01); 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc0,0xff);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xc1,0xd8);
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd3,0x7c);//0x78 //0x88  //0x70    //modify by sp_yjp,20131120
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd4,0x7c);//0x78 //0x80  //0x70
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd6,0x70);//0x78 //0x78  //0x60
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xd7,0x50);//0x78 //0x60  //0x50
  	//HEQ
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xdc,0x00);   
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xdd,0x78); //0x80 
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xde,0xc0);//80	//0x9d //modify by sp_yjp,20131120
  	  
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xdf,0x80);    
  	//func enable
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0xfd,0x00);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x31,0x00);//10
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x32,0x15);//d
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x34,0x76);//16
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x33,0xef);
  	  camsensor_SP0A19_ycbcr_i2c_write_byte(0x5f,0x51);          
	return  TRUE;
} /* camsensor_ov7690_write_sensor */
