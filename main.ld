ENTRY (main)

MEMORY
{
ram : ORIGIN = 0x00218000, LENGTH = 32M /* See step 7/8 of A0.pdf: RedBoot has fixed kernel start addr */
rom : ORIGIN = 0x60000000, LENGTH = 8M
}

SECTIONS
{
data : /* Initialized data. */
{
_DataStart = . ;
*(.data)
_DataEnd = . ;
} >ram

.bss : /* Uninitialized data. */
{
_BssStart = . ;
*(.bss)
*(COMMON)
_BssEnd = . ;
} >ram

text : /* The actual instructions. */
{
*(.text)
*(.got)
*(.got.plt)
*(.rodata)
*(.glue_7)
*(.glue_7t)
*(.rodata.str1.4)
} >ram
}
