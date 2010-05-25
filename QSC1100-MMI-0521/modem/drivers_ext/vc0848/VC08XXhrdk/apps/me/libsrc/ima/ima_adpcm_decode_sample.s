       AREA |.text|, CODE, READONLY      ; name this block of code
       
ima_adpcm_decode_sample 	FUNCTION

code RN 0;
n_code RN 0;
value RN 0;
state RN 1;
predsample RN 2;
index RN 3;
step RN 12;
sign RN 12;
diffq RN 14;
tempreg RN 14;

		EXPORT ima_adpcm_decode_sample
        IMPORT StepSizeTable
        IMPORT IndexTable
                
        ; short ima_adpcm_decode_sample(char code, ima_ADPCMstate *state)
        ; R0 code
        ; R1 state
        ; 
        ; RETURN R0
        
        ;stmfd sp!, {r14} ; R14 is also called LR
         STR r14,[sp,#-4]!
        
        ; predsample = (int)(state->valprev);
		; index = (int)state->index;
        LDR index,[state];  
        MOV predsample,index,LSL#16
        MOV predsample,predsample,ASR#16
        MOV index,index,LSR#16
        AND index,index,#0xFF
        
        ; step = StepSizeTable[index];
        LDR tempreg,=StepSizeTable
        LDR step,[tempreg,index,LSL#2];
        
        ; index += IndexTable[n_code];
        LDR tempreg,=IndexTable
        LDR tempreg,[tempreg,n_code,LSL#2]
        ADD index,index,tempreg;
        
        ; if( index < 0 )
		; index = 0;
	    ; if( index > 88 )
		; index = 88;
		CMP index,#0;
        MOVLT index,#0;
        CMP index,#88
        MOVGT index,#88
        
        ; diffq = step >> 3;
        MOV diffq,step, ASR#3
        
        ; if( n_code & 4 )
		;    diffq += step;
	    ; if( n_code & 2 )
		;    diffq += step >> 1;
	    ; if( n_code & 1 )
		;    diffq += step >> 2; 
		TST n_code,#4
		ADDNE diffq,diffq,step
		TST n_code,#2
		ADDNE diffq,diffq,step,ASR#1
		TST n_code,#1
		ADDNE diffq,diffq,step,ASR#2 
        
       	; if( n_code & 8 )
		;	predsample -= diffq;
		; else
		;	predsample += diffq;
        TST n_code,#8
        SUBNE predsample,predsample,diffq
        ADDEQ predsample,predsample,diffq
                
        ; value=CLIPTOSHORT(predsample)
        LDR tempreg,=0x7FFF;
        MOV sign,predsample,ASR#31; sign = x >> 31;
        TEQ sign,predsample,ASR#15; if (sign != (x >> 15))
        EORNE predsample,sign,tempreg; x = sign ^ (32767);
        MOV value,predsample; return (short)x;
        
        ; state->valprev=value;
	    ; state->index = (char)index;
	    MOV index,index,LSL#16
	    MOV tempreg,value,LSL#16
	    ORR index,index,tempreg,LSR#16
	    STR index,[state];
         
        ;ldmfd sp!, {r15} ; update PC with LR content. 
        LDR r15,[sp],#4        
        
        ENDFUNC
        
        
        END
        