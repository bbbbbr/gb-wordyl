	.include "global.s"
		
        .module tiny_flasher

_flash_sector_address = 0x7C00                  ; last kilobyte
.globl _flash_sector_address

        .area   _CODE

.macro .wb addr, val
        ld a, val
        ld (addr), a
.endm

; write data to flash ROM
; params: source, size on stack
; return: result in E
; callee cleanups stack
_flash_data_routine:
        di

        pop     hl
        pop     de                              ; de = source
        pop     bc                              ; bc = size
        push    hl

        ld      hl, #_flash_sector_address      ; destination

1$:
        .wb     #0x5555, #0xAA
        .wb     #0x2AAA, #0x55
        .wb     #0x5555, #0xA0                  ; perform magic

        ld      a, (de)
        ld      (hl), a                         ; write byte

        push    bc
        ld      b, #0                           ; wait counter
2$:
        cp      (hl)
        jr      z, 3$                           ; check byte

        push    hl
        pop     hl
        push    hl
        pop     hl                              ; delay 4+3+4+3=14

        dec     b
        jr      nz, 2$

        pop     bc

        ld      e, #0                           ; fail                  
        jr      5$
3$:
        pop     bc

        inc     de                              ; next source
        inc     hl                              ; next destination

        dec     bc
        ld      a, b
        or      c 
        jr      nz, 1$

        ld      e, #1                           ; success
5$:
        .wb     #_flash_sector_address, #0xF0   ; reset

        xor     a
        ldh     (.IF), a                        ; cancel pending interrupts
        ei 

        ret

_end_flash_data_routine:

; copies the flash writing routine onto CPU stack and calls it
; uint8_t write_flash(uint8_t * source, uint16_t size) OLDCALL;
_write_flash::
        push    bc
        lda     hl, 4(sp)
        ld      a, (hl+)
        ld      e, a
        ld      a, (hl+)
        ld      d, a                            ; de = source

        ld      a, (hl+)
        ld      c, a
        ld      b, (hl)                         ; bc = size

        ld      hl, #(_flash_data_routine - _end_flash_data_routine)
        add     hl, sp
        ld      sp, hl                          ; allocate ram on stack for the routine
        
        push    bc
        push    de

        push    hl
        ld      c, #(_end_flash_data_routine - _flash_data_routine)
        ld      de, #_flash_data_routine
        rst     0x30                            ; copy up to 256 bytes in C from DE to HL

        pop     hl
        rst     0x20                            ; call routine, callee cleanups stack

        ld      hl, #(_end_flash_data_routine - _flash_data_routine)
        add     hl, sp
        ld      sp, hl

        pop     bc
        ret


; erase 4kb sector
; params: none
; return: result in E
_erase_flash_sector_routine:
        di

        .wb     #_flash_sector_address, #0xF0   ; reset

        .wb     #0x5555, #0xAA
        .wb     #0x2AAA, #0x55
        .wb     #0x5555, #0x80
        .wb     #0x5555, #0xAA
        .wb     #0x2AAA, #0x55
        .wb     #_flash_sector_address, #0x30   ; perform erase sector magic

        ld      de, #0                          ; wait counter
1$:
        ld      a, (#_flash_sector_address)
        cp      #0xFF
        jr      z, 2$                           ; check byte

        push    hl
        pop     hl
        push    hl
        pop     hl                              ; delay 4+3+4+3=14

        dec     e
        jr      nz, 1$
        dec     d
        jr      nz, 1$

        ld      e, #0                           ; fail
        jr      3$
2$:
        ld      e, #1                           ; success
3$:
        xor     a
        ldh     (.IF), a                        ; cancel pending interrupts
        ei

        ret
_end_erase_flash_sector_routine:

; copies the flash sector erasing routine onto CPU stack and calls it 
; uint8_t erase_flash() OLDCALL;
_erase_flash::
        lda     hl, 0(sp)
        ld      d, h
        ld      e, l                            ; de = sp

        ld      hl, #(_erase_flash_sector_routine - _end_erase_flash_sector_routine)
        add     hl, sp
        ld      sp, hl                          ; allocate ram on stack for the routine

        push    de
        push    hl

        ld      c, #(_end_erase_flash_sector_routine - _erase_flash_sector_routine)
        ld      de, #_erase_flash_sector_routine
        rst     0x30                            ; copy up to 256 bytes in C from DE to HL

        pop     hl
        rst     0x20                            ; call routine on stack using call hl

        pop     hl
        ld      sp, hl

        ret

